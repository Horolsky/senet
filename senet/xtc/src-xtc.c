#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "src-xtc.h"

/*
 * xMoves manipulation
 */

xMoves _moves_add_move(xMoves moves, ui8 move);
//return 1 if true, 0 if false
ui8 _moves_contain_move(xMoves moves, ui8 move);

/*
 * GAME MECHANICS DECLARATIONS
 */

ui32 get_moves_meub(ui64 seed);
ui32 get_moves_kendal(ui64 seed);
typedef ui64 (*state_increment_func_internal)(xState state, xMoves moves, ui8 move);

//internal unsafe function, movement validation must be done before
ui64 increment_meub_internal(xState state, xMoves moves, ui8 move);
//internal unsafe function, movement validation must be done before
ui64 increment_kendal_internal(xState state, xMoves moves, ui8 move);
const state_increment_func_internal internal_incr[2] = {increment_meub_internal, increment_kendal_internal};
state_increment_func_internal get_increment_func_internal(rules_e id);

/*
 * GAME MECHANICS EXTERNAL DEFINITIONS
 */

ui64 increment_kendal_external(ui64 seed, ui8 move){
    return increment_kendal_internal((xState){._seed=seed}, (xMoves) {._seed=get_moves_meub(seed)}, move);
}
ui64 increment_meub_external(ui64 seed, ui8 move){ 
    return increment_meub_internal((xState){._seed=seed}, (xMoves) {._seed=get_moves_meub(seed)}, move);
}

const state_increment_func external_increment[2] = {
    increment_kendal_external, 
    increment_meub_external
    };

state_increment_func get_increment_func(rules_e id){
    return external_increment[(int) id];
}

/*
 * EVALUATION
 */
//evaluation source data
typedef struct _eval_src {
    float steps;    //ratio of total steps left
    float mobility; //ratio of mean mobility
    float attack;   //ratio of vulnerability to attacks
} eval_src;
eval_src _get_eval_src(ui64 seed);
//steps, defence, defence, attacks
const float default_coefs[4] = {1,0,0,0};

float eval_basic(ui64 seed, float *coefs);
float eval_linear(ui64 seed, float *coefs);
float eval_meub(ui64 seed, float *coefs);

const state_evaluation_func external_eval[3] = {
    eval_basic,
    eval_linear,
    eval_meub
};

state_evaluation_func get_evaluation_func(eval_e id){
    return external_eval[(int) id];
}

/*
 * EXPECTIMINIMAX
 */

static state_increment_func_internal _increment;
//static ui64 (*_increment)(xState state, xMoves moves, ui8 move);
static state_evaluation_func _eval;
static state_legal_moves_func _get_moves;
static ui64 _node_counter;
static time_t _stoptime;
static ui8 _stopdepth;
static float * _user_coefs;
static ui64 * _strat_seeds;
static float * _strat_utils;
static int _player;

float _expectimax(ui64 seed, ui8 depth);
void * _threadwork(void * param);



xMoves _moves_add_move(xMoves moves, ui8 move){
    MOVES_SET(moves, moves._len, move)
    moves._len++;
    return moves;
}

ui8 _moves_contain_move(xMoves moves, ui8 move){
    ui8 mn = 0;
    for (ui8 i = 0; i < moves._len; i++ ) mn += (MOVES_GET(moves, i) == move);
    return mn > 0 ? 1 : 0;
}

ui32 get_moves_kendal(ui64 seed){
    xState state = {._seed=seed};
    xMoves moves;
    moves._seed = 0;
    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 dest, trgt;

    /* HOUSE OF WATER PENALTY */                            
    if (BOARD_GET(state, 26) == agent){
        if (state._steps == 4){
            moves = _moves_add_move(moves, 26);                                   //escaping
            moves._dir = 1; 
        }
        else if (BOARD_GET(state, 14) == 0){
            moves = _moves_add_move(moves, 26);                                   //reborning    
            moves = _moves_add_move(moves, 30);                                   //skipping
            moves._dir = 2;                                                 //keep this not 0
        }
        return moves._seed;
    }
    /* DIRECT MOVE */
    moves._dir = 1;                                                                 
    for (ui8 i =0; i < 30; i++){
        ui8 cell = BOARD_GET(state, i);
        if (cell != agent) continue;
        dest = i + state._steps;
        trgt = BOARD_GET(state, dest);
        if (i == 29) moves = _moves_add_move(moves, i);                           // always escaping
        else if (i > 24 && dest == 30) moves = _moves_add_move(moves, i);         // correct escaping
        else if (i != 25 && dest > 25) continue;                            // forbidden house arriving
        else if (dest > 29) continue;                                       // corrupted logic
        else if (trgt == 0) moves = _moves_add_move(moves, i);                    // normal movement
        else if (trgt == enemy) {                                           // attack
            ui8 prev = dest > 0 ? BOARD_GET(state, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(state, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = _moves_add_move(moves, i); //defended trgt
            else if (dest == 26) moves = _moves_add_move(moves, i);               //drowed trgt
        }
    }
    /* REVERSE MOVE */
    if (moves._len == 0) {   
        moves._dir = 2;                      
        ui8 cell;
        trgt = BOARD_GET(state, 0);// state._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(state, i);
            if (cell == agent && trgt == 0) moves = _moves_add_move(moves, i);
            else if (cell == agent && trgt == enemy) {                                                   // attack backward
                ui8 prev = i > 1 ? BOARD_GET(state, (i-2)) : 0;
                if (prev != enemy) moves = _moves_add_move(moves, i);
                else if (i == 27) moves = _moves_add_move(moves, i);                          //drowed trgt
            }
            trgt = cell;
        }
    }
    /* SKIP MOVE */
    if (moves._len == 0) moves._dir = 0;                                              
    return moves._seed;
}

ui32 get_moves_meub(ui64 seed){
    xState state = {._seed=seed};
    xMoves moves;
    moves._seed = 0;
    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 dest, trgt;
    /* DIRECT MOVE */
    moves._dir = 1;
    for (ui8 i = 0; i < 30; i++){
        ui8 cell = BOARD_GET(state, i);
        if (cell != agent) continue;
        dest = i + state._steps;
        trgt = BOARD_GET(state, dest);
        if (i == 29) moves = _moves_add_move(moves, i);                       // always escaping
        else if (i > 24 && dest == 30) moves = _moves_add_move(moves, i);     // correct escaping
        else if (i != 25 && dest > 25) continue;                        // forbidden house arriving
        else if (dest > 29) continue;                                   // forbidden escaping
        else if (trgt == 0) moves = _moves_add_move(moves, i);                // normal movement
        else if (trgt == enemy) {                                       // attack
            ui8 prev = dest > 0 ? BOARD_GET(state, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(state, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = _moves_add_move(moves, i);
        }
    }
    /* REVERSE MOVE */
    if (moves._len == 0){   
        moves._dir = 2;                      
        ui8 cell, trgt = BOARD_GET(state, 0);
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(state, i);
            if (cell == agent && trgt == 0) moves = _moves_add_move(moves, i);
            trgt = cell;
        }
    }
    /* SKIP MOVE */
    if (moves._len == 0) moves._dir = 0;
    return moves._seed;
}

state_increment_func_internal get_increment_func_internal(rules_e id){
    return internal_incr[(int) id];
}

ui64 increment_kendal_internal(xState state, xMoves moves, ui8 move){
    if (moves._len > 0 && !_moves_contain_move(moves, move)) return 0; //illegal move

    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 steps = state._steps;
    state._steps = 0;

    /* next step preparation */
    state._agent = enemy - 1;                                               //normal move
    if (steps == 1 || steps == 4 || steps == 5) state._agent = agent - 1;   //bonus move

    /* skipping moves */
    if (move == 30 && _moves_contain_move(moves, 30)) return state._seed;           //penalty skipping
    if (moves._len == 0) return state._seed;                                //normal skipping
    
    /* movement destination */
    ui8 dest = moves._dir  == 1 ? dest = move + steps : move - 1; 
    //skipping case was resolved in prev. step 
    
    /* STATE UPD */
    
    //escaping
    if (dest == 30 || move == 29) {BOARD_SET(state,move,0)}
    //reborning from House of Water
    else if (move == 26 && BOARD_GET(state, 14) == 0){
        BOARD_SET(state,move,0)
        BOARD_SET(state,14,agent)
    }
    //attacking
    else if (BOARD_GET(state, dest) == enemy){
        BOARD_SET(state, dest, agent)
        if (dest > 26 && BOARD_GET(state, 26) == 0){ //attacking Houses rule    
            BOARD_SET(state, 26, enemy)
            BOARD_SET(state,move,0)
        } else { BOARD_SET(state, move, enemy) }
    }
    //moving to empty target
    else if (BOARD_GET(state, dest) == 0){
        BOARD_SET(state,dest,agent)
        BOARD_SET(state,move,0)
    }
    //corrupted logic
    else state._seed = 0; 
    
    return state._seed;
}

ui64 increment_meub_internal(xState state, xMoves moves, ui8 move){
    if (moves._len > 0 && !_moves_contain_move(moves, move)) return 0; //illegal move

    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 steps = state._steps;
    state._steps = 0;

    /* next step preparation */
    state._agent = enemy - 1;                                               //normal move
    if (steps == 1 || steps == 4 || steps == 5) state._agent = agent - 1;   //bonus move

    /* skipping moves */
    if (moves._len == 0) return state._seed;                                //normal skipping
    
    /* movement destination */
    ui8 dest = moves._dir  == 1 ? dest = move + steps : move - 1; 
    //skipping case was resolved in prev. step 

    /* STATE UPD */
    //escaping
    if (move == 29 || dest == 30) {BOARD_SET(state,move,0)}
    //drowing in House of Water
    else if (dest == 26){
        for (ui8 i = 14; i >= 0; i--){
            if (BOARD_GET(state, i) == 0) {
                dest = i;
                break;
            }
        }
        BOARD_SET(state,move,0)
        BOARD_SET(state,dest,agent)
    }
    //attacking
    else if (BOARD_GET(state, dest) == enemy){
        BOARD_SET(state, dest, agent)
        if (dest > 26){ //attacking Houses rule
            for (ui8 i = 14; i >= 0; i--){
                if (BOARD_GET(state, i) == 0){
                    BOARD_SET(state, i, enemy)
                    break;
                }
            }
            BOARD_SET(state,move,0)
        } else { BOARD_SET(state, move, enemy) }
    }
    //moving to empty target
    else if (BOARD_GET(state, dest) == 0){
        BOARD_SET(state,dest,agent)
        BOARD_SET(state,move,0)
    }
    //corrupted logic
    else state._seed = 0; 
    
    return state._seed;
}

float eval_basic(ui64 seed, float*coefs){
    xState state = {._seed=seed};
    int maxSum = 0;
    int minSum = 0;
    int cell;
    for (ui8 i =0; i < 30; i++){
        cell = BOARD_GET(state, i);
        if (cell == 1) maxSum += (30 - i);
        else if (cell == 2) minSum += (30 - i);
    }
    float ut;
    if (minSum == 0) ut = 0;
    else if (maxSum == 0) ut = 0;
    else ut = (float) (minSum) / (maxSum + minSum);
    return ut;
}

float eval_meub(ui64 seed, float*coefs){
    /*
    +- 100 pts for each pawn on bench
    +- sum of steps
    +- 7 for each guarded pawn (if enemy behind)
    +- 20 for the foremost blockade
    -+ 10 pts penalty if cant move 2 steps
    -+ 10 pts penalty if cant move 3 steps
    */
    xState state = {._seed=seed};
    ui8 prev, cell, next;
    signed char first[2] = {-1, -1};
    ui8 defended, foremost = 0;
    ui8 bench[2] = {5,5};
    float eval = 0;
    
    for (ui8 i =0; i < 30; i++){
        cell = BOARD_GET(state, i);
        if (!cell) continue;
        prev = i > 0 ? BOARD_GET(state, (i-1)) : 0;
        next = i < 29 ? BOARD_GET(state, (i+1)) : 0;
        if (first[cell-1] < 0) first[cell-1] = i;

        eval += cell == 1 ? i : -i;
        bench[cell-1]--;
        defended = (cell == prev) || (cell == next);
        if (defended && first[cell%2] < i) {
            eval += cell == 1 ? 7 : -7;
            foremost = cell; 
        }
    }
    if (bench[0] == 5 || bench[1] == 5) {
        eval = bench[0] == 5 ? 1 : 0;
        eval = bench[1] == 5 ? -1 : 0; //corrupted logic or empty board
    }
    else {
        eval += foremost == 1 ? 20 : -20;
        eval += -100*bench[0] + 100*bench[1];

        ui8 penalty = 0;
        state._steps = 2;
        xMoves moves = {._seed=get_moves_meub(state._seed)};
        penalty += 5 - bench[state._agent] - moves._len;
        state._steps = 3;
        moves._seed=get_moves_meub(state._seed);
        penalty += 5 - bench[state._agent] - moves._len;
        eval += state._agent == 0 ? -10 * penalty : 10 * penalty;
        eval = (eval + 500) / 1000;
    }
    return eval;
}

eval_src _get_eval_src(ui64 seed){
    /*
    rules dependent features not counted
    */
    xState state = {._seed=seed};

    ui8 _S[2] = {0,0};//steps sum
    float _M[2] = {0,0};//mobility, only count forward moves
    float _V[2] = {0,0};//vulnerability

    ui8 drawed = BOARD_GET(state, 26);
    ui8 pawns[2] = {0,0};//num of pawns on board in each troupe
    ui8 first[2] = {30, 30};
    /* COLLECTING DATA */
    ui8 defended, prev, cell = 0, next = BOARD_GET(state, 0);
    for (si8 i =0; i < 30; i++) {
        prev = cell;
        cell = next;
        next = i < 29 ? BOARD_GET(state, (i+1)) : 0;
        defended = 0;
        if (cell != 0) {
            if (first[cell-1] == 30) first[cell-1] = i;
            pawns[cell-1]++;
            _S[cell-1] += 30-i;
            defended = (cell == prev || cell == next) && first[cell%2] < i;
            if (i == 26) defended = 0;   
        }
        /* mobility and vulnerability SCORE*/
        for (si8 j = 1; j < 6; j++) {
            if (i-j < 0) continue;
            ui8 attacker = BOARD_GET(state, (i-j));
            if (attacker == drawed) continue;
            if (cell!=0 && !defended && attacker == cell%2) _V[cell-1] += P[j-1]*i;
            if (attacker!=0) _M[attacker-1] += j;
        }
    }
    if (drawed!=0) _M[drawed-1] = 4;//only possible forward move
    //getting mean mobility
    _M[0] /= pawns[0];
    _M[1] /= pawns[1];
    
    eval_src res;
    if (pawns[0] & pawns[1]){
        res.steps = _S[0] + _S[1] == 0 ? 0.5 : (float) _S[1] / (_S[0] + _S[1]);
        res.mobility = _M[0] + _M[1] == 0 ? 0.5 : _M[0] / (_M[0] + _M[1]);
        res.attack = _V[0] + _V[1] == 0 ? 0.5 : _V[1] / (_V[0] + _V[1]);
    }
    else if (pawns[0] | pawns[1]){
        res.steps = pawns[0] ? 1 : 0;
        res.mobility = pawns[0] ? 1 : 0;
        res.attack = pawns[0] ? 1 : 0;
    }
    else {
        res.steps = -1;
        res.mobility = -1;
        res.attack = -1;
    }
    return res;
}

float eval_linear(ui64 seed, float *coefs) {
    /*
    0: steps sum
    1: priority
    2: mobility
    3: attack
    */
    eval_src src = _get_eval_src(seed);
    float priority = (seed-1) % 2;
    
    float eval = 0;
    float csum = 0;
    if (coefs == NULL) {
        for (ui8 i = 0; i < 4; i++) csum += default_coefs[i];
        eval = 
        default_coefs[0] * src.steps + 
        default_coefs[1] * priority +
        default_coefs[2] * src.mobility + 
        default_coefs[3] * src.attack;
        eval /= 1;
    }
    else {
        for (ui8 i = 0; i < 4; i++) {
            if (coefs[i] < 0) coefs[i] = 0;
            csum += coefs[i];
        }
        eval = 
        coefs[0] * src.steps + 
        coefs[1] * priority +
        coefs[2] * src.mobility + 
        coefs[3] * src.attack;
        eval /= csum;
    }
    return eval;
}

float _expectimax(ui64 seed, ui8 depth){
    _node_counter += 1;
    float util = _eval(seed, _user_coefs);
    if (util  == 1 || util == 0 || depth >= _stopdepth || clock() > _stoptime){
        
        return util;
    }
    depth++;
    float cutil = 0;//chance util
    util = 0;
    xState chance_state;
    xMoves chance_moves;
    for (ui8 chance = 0; chance < 5; chance++){
        cutil = 0;
        chance_state._seed = seed; 
        chance_state._steps=chance+1;
        chance_moves._seed=_get_moves(chance_state._seed);
        
        for (ui8 move = 0; move < chance_moves._len; move++){
            ui64 childstate = _increment(chance_state, chance_moves, MOVES_GET(chance_moves, move));            
            cutil += _expectimax(childstate, depth);
        }
        _player ? (util = cutil > util ? cutil : util) : (util = cutil < util ? cutil : util);
        //wrong:
        // _player ? (util = cutil < util ? cutil : util) : (util = cutil > util ? cutil : util);
        // util += chance_moves._len != 0 ? cutil * P[chance] / chance_moves._len : cutil * P[chance];
    }
    return util;
}

void * _threadwork(void * param){
    ui8 id = (ui8) param;
    float res = _expectimax(_strat_seeds[id], (ui8) 1);
    _strat_utils[id] = res;
    return NULL;
}

state_legal_moves_func get_legal_moves_func(rules_e id){
    switch (id){
        case id_incr_meub:
            return get_moves_meub;
            break;
        case id_incr_kendal:
            return get_moves_kendal;
            break;
        default:
            return NULL;
            break;
    }
}


emax_res get_strategy_emax_mt(ui64 seed, ui8 depth, ui8 sec, eval_e eval_id, rules_e incr_id, float *coefs){
    _eval = get_evaluation_func(eval_id);
    _increment = get_increment_func_internal(incr_id);
    _get_moves = get_legal_moves_func(incr_id);
    _node_counter = 0;
    _stoptime = clock() + sec * CLOCKS_PER_SEC;
    _stopdepth = depth;
    _user_coefs = coefs;

    xState state = {._seed=seed};
    _player = state._agent ? 1 : 0;
    xMoves strat_moves = {._seed=_get_moves(seed)};
    _strat_seeds = malloc (sizeof(ui64) * strat_moves._len);
    _strat_utils = malloc (sizeof(float) * strat_moves._len);
    pthread_t * threads = malloc (sizeof(pthread_t) * strat_moves._len);
    for (ui8 s = 0; s < strat_moves._len; s++){
        _strat_seeds[s] = _increment(state, strat_moves, MOVES_GET(strat_moves, s));
        pthread_create(&threads[s], NULL, _threadwork, (void*) s);
        if (!threads[s]) _strat_utils[s] = -1;//TODO handle error
    }   
    for (ui8 s = 0; s < strat_moves._len; s++) pthread_join(threads[s], NULL);
    emax_res result;
    result.searched_nodes = _node_counter;
    result.strategy = 0;
    for (ui8 s = 0; s < strat_moves._len; s++) {
        if (state._agent == 0){
            if (_strat_utils[s] > _strat_utils[result.strategy]) result.strategy = s;
        }
        else {
            if (_strat_utils[s] < _strat_utils[result.strategy]) result.strategy = s;
        }
        
    }
    free(_strat_seeds);
    free(_strat_utils);
    free(threads);
    return result;
}