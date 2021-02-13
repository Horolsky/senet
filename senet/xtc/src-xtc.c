#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "src-xtc.h"

static ui64 counter;
static time_t stoptime;
static ui8 stopdepth;
xMoves add_move(xMoves moves, ui8 m){
    MOVES_SET(moves, moves._len, m)
    moves._len++;
    return moves;
}

ui8 _move_is_in(xMoves moves, ui8 m){
    ui8 mn = 0;
    for (ui8 i = 0; i < moves._len; i++ ) mn += (MOVES_GET(moves, i) == m);
    return mn > 0 ? 1 : 0;
}


ui32 get_moves(ui64 seed){
    xState s = {._seed=seed};
    xMoves moves;
    moves._seed = 0;
    ui8 agent = s._agent + 1;
    ui8 enemy = agent % 2 + 1;
    ui8 dest, trgt;
    
    moves._dir = 1;                                                                 // direct move
    for (ui8 i =0; i < 30; i++){
        ui8 cell = BOARD_GET(s, i);
        if (cell != agent) continue;
        dest = i + s._steps;
        trgt = BOARD_GET(s, dest);
        if (i == 29) moves = add_move(moves, i);                                   // always escaping
        else if ((i == 27 || i == 28) && dest == 30) moves = add_move(moves, i);   // correct escaping
        else if (i != 25 && dest > 25) continue;                                    // forbidden house arriving
        else if (dest > 29) continue;                                               // forbidden escaping
        else if (trgt == 0) moves = add_move(moves, i);                            // normal movement
        else if (trgt == enemy) {                                                   // attack
            ui8 prev = dest > 0 ? BOARD_GET(s, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(s, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = add_move(moves, i);
        }
    }
    if (moves._len == 0){   // reverse move
        moves._dir = 2;                      
        ui8 cell, prev = BOARD_GET(s, 0);// s._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(s, i);
            if (cell == agent && prev == 0) moves = add_move(moves, i);
            prev = cell;
        }
    }
    if (moves._len == 0) moves._dir = 0;                                              // skip move
    return moves._seed;
}

ui64 increment_1(ui64 seed, ui8 m){
    xState s = {._seed=seed};
    xMoves moves = {._seed=get_moves(seed)};
    ui8 agent = s._agent + 1;
    ui8 enemy = agent % 2 + 1;
    s._agent = enemy - 1;
    ui8 steps = s._steps;
    s._steps = 0;

    if (steps == 1 || steps == 4 || steps == 5) s._agent = agent - 1;
    if (moves._len == 0) return s._seed;
    if (_move_is_in(moves, m) == 0) return 0;
    ui8 dest = m - 1;
    if (moves._dir  == 1) dest = m + steps;
    //drowing in House of Water
    if (dest == 26){
        for (ui8 i = 14; i >= 0; i--){
            if (BOARD_GET(s, i) == 0) {
                dest = i;
                break;
            }
        }
        BOARD_SET(s,m,0)
        BOARD_SET(s,dest,agent)
    }
    //escaping
    else if (m == 29 || dest == 30) {BOARD_SET(s,m,0)}
    //attacking
    else if (BOARD_GET(s, dest) == enemy){
        BOARD_SET(s, dest, agent)
        if (dest > 26){ //attacking Houses rule
            for (ui8 i = 14; i >= 0; i--){
                if (BOARD_GET(s, i) == 0){
                    BOARD_SET(s, i, enemy)
                    break;
                }
            }
            BOARD_SET(s,m,0)
        } else { BOARD_SET(s, m, enemy) }
    }
    //moving to empty target
    else if (BOARD_GET(s, dest) == 0){
        BOARD_SET(s,dest,agent)
        BOARD_SET(s,m,0)
    }
    //corrupted logic
    else s._seed = 0; 
    return s._seed;
}

float eval_basic(ui64 seed){
    xState s = {._seed=seed};
    int maxSum = 0;
    int minSum = 0;
    int cell;
    for (ui8 i =0; i < 30; i++){
        cell = BOARD_GET(s, i);
        if (cell == 1) maxSum += (30 - i);
        else if (cell == 2) minSum += (30 - i);
    }
    float ut;
    if (minSum == 0) ut = 0;
    else if (maxSum == 0) ut = 0;
    else ut = (float) (minSum) / (maxSum + minSum);
    return ut;
}

float expectimax_brute(ui64 seed, ui8 depth){
    counter += 1;
    float util = eval_basic(seed);
    if (util  == 1 || util == 0 || depth == 0){
        return util;
    }

    xState state = {._seed=seed};
    xMoves cm;//chance moves
    float cutil = 0;//chance util
    util = 0;
    for (ui8 steps = 1; steps < 6; steps++){
        cutil = 0;
        state._steps = steps;
        cm._seed = get_moves(state._seed);
        for (ui8 m = 1; m < cm._len; m++) cutil += expectimax_brute(increment_1(state._seed, MOVES_GET(cm, m)), depth-1);
        
        util += cm._len != 0 ? cutil * P[steps-1] / cm._len : cutil * P[steps-1];
    }
    return util;
}


emax_test expectimax_count(ui64 seed, ui8 depth){
    counter = 0;
    emax_test result = {
        .res = expectimax_brute(seed, depth),
        .count = counter
    };
    return result;
}

float expectimax_timedbrute(ui64 seed, ui8 depth){
    float util = eval_basic(seed);
    if (util  == 1 || util == 0 || depth == 0 || clock() > stoptime){
        counter += 1;
        return util;
    }
    xState state = {._seed=seed};
    xMoves cm;//chance moves
    float cutil = 0;//chance util
    util = 0;
    for (ui8 steps = 1; steps < 6; steps++){
        cutil = 0;
        state._steps = steps;
        cm._seed = get_moves(state._seed);
        for (ui8 m = 1; m < cm._len; m++) cutil += expectimax_brute(increment_1(state._seed, MOVES_GET(cm, m)), depth-1);
        
        util += cm._len != 0 ? cutil * P[steps-1] / cm._len : cutil * P[steps-1];
    }
    return util;
}

emax_test expectimax_timecount(ui64 seed, ui8 depth, ui8 sec){
    counter = 0;
    stoptime = clock() + sec * CLOCKS_PER_SEC;
    emax_test result = {
        .res = expectimax_timedbrute(seed, depth),
        .count = counter
    };
    return result;
}


float emax_brute_t_mt(ui64 seed, ui8 depth){
    counter += 1;
    float util = eval_basic(seed);
    if (util  == 1 || util == 0 || depth >= stopdepth || clock() > stoptime){
        
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
        chance_moves._seed=get_moves(chance_state._seed);
        
        for (ui8 move = 0; move < chance_moves._len; move++){
            ui64 childstate = increment_1(chance_state._seed, MOVES_GET(chance_moves, move));            
            cutil += emax_brute_t_mt(childstate, depth);
        }
        util += chance_moves._len != 0 ? cutil * P[chance] / chance_moves._len : cutil * P[chance];
    }
    return util;
}

static ui64 * strat_seeds;
static float * strat_utils;

void * threadwork(void * param){
    ui8 id = (ui8) param;
    float res = emax_brute_t_mt(strat_seeds[id], (ui8) 1);
    strat_utils[id] = res;
    return NULL;
}
emax_res expectimax_multithread(ui64 seed, ui8 depth, ui8 sec){
    counter = 0;
    stoptime = clock() + sec * CLOCKS_PER_SEC;
    stopdepth = depth;
    xMoves smoves = {._seed=get_moves(seed)};
    strat_seeds = malloc (sizeof(ui64) * smoves._len);
    strat_utils = malloc (sizeof(float) * smoves._len);
    pthread_t * threads = malloc (sizeof(pthread_t) * smoves._len);
    for (ui8 s = 0; s < smoves._len; s++){
        ui64 substate = increment_1(seed, MOVES_GET(smoves, s));
        strat_seeds[s] = substate;
        pthread_create(&threads[s], NULL, threadwork, (void*) s);
        if (!threads[s]) strat_utils[s] = -1;//TODO handle error
    }   
    for (ui8 s = 0; s < smoves._len; s++) pthread_join(threads[s], NULL);
    emax_res result;
    result.count = counter;
    result.res = 0;
    for (ui8 s = 0; s < smoves._len; s++) {
        if (strat_utils[s] > strat_utils[result.res]) result.res = s;
    }
    free(strat_seeds);
    free(strat_utils);
    free(threads);
    return result;
}