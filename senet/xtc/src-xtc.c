#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "src-xtc.h"

xMoves _add_move(xMoves moves, ui8 move){
    MOVES_SET(moves, moves._len, move)
    moves._len++;
    return moves;
}

ui8 _move_is_in(xMoves moves, ui8 move){
    ui8 mn = 0;
    for (ui8 i = 0; i < moves._len; i++ ) mn += (MOVES_GET(moves, i) == move);
    return mn > 0 ? 1 : 0;
}

ui32 get_moves_meub(ui64 seed){
    xState state = {._seed=seed};
    xMoves moves;
    moves._seed = 0;
    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 dest, trgt;
    
    moves._dir = 1;                                                                 // direct move
    for (ui8 i =0; i < 30; i++){
        ui8 cell = BOARD_GET(state, i);
        if (cell != agent) continue;
        dest = i + state._steps;
        trgt = BOARD_GET(state, dest);
        if (i == 29) moves = _add_move(moves, i);                                   // always escaping
        else if ((i == 27 || i == 28) && dest == 30) moves = _add_move(moves, i);   // correct escaping
        else if (i != 25 && dest > 25) continue;                                    // forbidden house arriving
        else if (dest > 29) continue;                                               // forbidden escaping
        else if (trgt == 0) moves = _add_move(moves, i);                            // normal movement
        else if (trgt == enemy) {                                                   // attack
            ui8 prev = dest > 0 ? BOARD_GET(state, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(state, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = _add_move(moves, i);
        }
    }
    if (moves._len == 0){   // reverse move
        moves._dir = 2;                      
        ui8 cell, prev = BOARD_GET(state, 0);// state._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(state, i);
            if (cell == agent && prev == 0) moves = _add_move(moves, i);
            prev = cell;
        }
    }
    if (moves._len == 0) moves._dir = 0;                                              // skip move
    return moves._seed;
}

ui64 increment_meub(ui64 seed, ui8 move){
    xState state = {._seed=seed};
    xMoves moves = {._seed=get_moves_meub(seed)};
    ui8 agent = state._agent + 1;
    if (BOARD_GET(state, move) != agent) return 0;

    ui8 enemy = agent % 2 + 1;
    state._agent = enemy - 1;
    ui8 steps = state._steps;
    state._steps = 0;
    ui8 dest = move + steps;

    if (steps == 1 || steps == 4 || steps == 5) state._agent = agent - 1; //bonus 
    if (moves._len == 0) return state._seed;
    if (_move_is_in(moves, move) == 0) return 0;
    
    if (moves._dir  == 1) dest = move + steps;
    //drowing in House of Water
    if (dest == 26){
        for (ui8 i = 14; i >= 0; i--){
            if (BOARD_GET(state, i) == 0) {
                dest = i;
                break;
            }
        }
        BOARD_SET(state,move,0)
        BOARD_SET(state,dest,agent)
    }
    //escaping
    else if (move == 29 || dest == 30) {BOARD_SET(state,move,0)}
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

ui32 get_moves_kendal(ui64 seed){
    xState state = {._seed=seed};
    xMoves moves;
    moves._seed = 0;
    ui8 agent = state._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 dest, trgt;
    
    moves._dir = 1;                                                                 // direct move
    for (ui8 i =0; i < 30; i++){
        ui8 cell = BOARD_GET(state, i);
        if (cell != agent) continue;
        dest = i + state._steps;
        trgt = BOARD_GET(state, dest);
        if (i == 29) moves = _add_move(moves, i);                                   // always escaping
        else if ((i == 27 || i == 28) && dest == 30) moves = _add_move(moves, i);   // correct escaping
        else if (i != 25 && dest > 25) continue;                                    // forbidden house arriving
        else if (dest > 29) continue;                                               // forbidden escaping
        else if (trgt == 0) moves = _add_move(moves, i);                            // normal movement
        else if (trgt == enemy) {                                                   // attack
            ui8 prev = dest > 0 ? BOARD_GET(state, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(state, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = _add_move(moves, i);
        }
    }
    if (moves._len == 0){   // reverse move
        moves._dir = 2;                      
        ui8 cell, prev = BOARD_GET(state, 0);// state._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(state, i);
            if (cell == agent && prev == 0) moves = _add_move(moves, i);
            prev = cell;
        }
    }
    if (moves._len == 0) moves._dir = 0;                                              // skip move
    return moves._seed;
}

ui64 increment_kendal(ui64 seed, ui8 move){
    xState state = {._seed=seed};
    xMoves moves = {._seed=get_moves_meub(seed)};
    ui8 agent = state._agent + 1;
    if (BOARD_GET(state, move) != agent) return 0;

    ui8 enemy = agent % 2 + 1;
    state._agent = enemy - 1;
    ui8 steps = state._steps;
    state._steps = 0;
    ui8 dest = move + steps;

    if (steps == 1 || steps == 4 || steps == 5) state._agent = agent - 1; //bonus 
    if (moves._len == 0) return state._seed;
    if (_move_is_in(moves, move) == 0) return 0;
    
    if (moves._dir  == 1) dest = move + steps;
    //drowing in House of Water
    if (dest == 26){
        for (ui8 i = 14; i >= 0; i--){
            if (BOARD_GET(state, i) == 0) {
                dest = i;
                break;
            }
        }
        BOARD_SET(state,move,0)
        BOARD_SET(state,dest,agent)
    }
    //escaping
    else if (move == 29 || dest == 30) {BOARD_SET(state,move,0)}
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

state_increment_func get_increment_func(incr_id_e id){
    switch (id){
        case id_incr_meub:
            return increment_meub;
            break;
        case id_incr_kendal:
            return increment_kendal;
            break;
        default:
            return NULL;
            break;
    }
}

state_legal_moves_func get_legal_moves_func(incr_id_e id){
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

float eval_basic(ui64 seed){
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

state_evaluation_func get_evaluation_func(eval_id_e id){
    switch (id){
        case id_eval_basic:
            return eval_basic;
            break;
        default:
            return NULL;
            break;
    }
}

static state_evaluation_func _eval;
static state_increment_func _increment;
static state_legal_moves_func _get_moves;
static ui64 _node_counter;
static time_t _stoptime;
static ui8 _stopdepth;
static ui64 * _strat_seeds;
static float * _strat_utils;

float _expectimax(ui64 seed, ui8 depth){
    _node_counter += 1;
    float util = _eval(seed);
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
            ui64 childstate = _increment(chance_state._seed, MOVES_GET(chance_moves, move));            
            cutil += _expectimax(childstate, depth);
        }
        util += chance_moves._len != 0 ? cutil * P[chance] / chance_moves._len : cutil * P[chance];
    }
    return util;
}

void * _threadwork(void * param){
    ui8 id = (ui8) param;
    float res = _expectimax(_strat_seeds[id], (ui8) 1);
    _strat_utils[id] = res;
    return NULL;
}

emax_res get_strategy_emax_mt(ui64 seed, ui8 depth, ui8 sec, eval_id_e eval_id, incr_id_e incr_id){
    _eval = get_evaluation_func(eval_id);
    _increment = get_increment_func(incr_id);
    _get_moves = get_legal_moves_func(incr_id);
    _node_counter = 0;
    _stoptime = clock() + sec * CLOCKS_PER_SEC;
    _stopdepth = depth;
    xMoves smoves = {._seed=_get_moves(seed)};
    _strat_seeds = malloc (sizeof(ui64) * smoves._len);
    _strat_utils = malloc (sizeof(float) * smoves._len);
    pthread_t * threads = malloc (sizeof(pthread_t) * smoves._len);
    for (ui8 s = 0; s < smoves._len; s++){
        ui64 substate = _increment(seed, MOVES_GET(smoves, s));
        _strat_seeds[s] = substate;
        pthread_create(&threads[s], NULL, _threadwork, (void*) s);
        if (!threads[s]) _strat_utils[s] = -1;//TODO handle error
    }   
    for (ui8 s = 0; s < smoves._len; s++) pthread_join(threads[s], NULL);
    emax_res result;
    result.searched_nodes = _node_counter;
    result.strategy = 0;
    for (ui8 s = 0; s < smoves._len; s++) {
        if (_strat_utils[s] > _strat_utils[result.strategy]) result.strategy = s;
    }
    free(_strat_seeds);
    free(_strat_utils);
    free(threads);
    return result;
}