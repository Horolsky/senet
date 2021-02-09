#include "src-xtc.h"
#include "time.h"
static ui64 counter;
time_t stoptime;
xMoves _add_move(xMoves moves, ui8 m){
    switch (moves._len){
    case 0:
        moves._mv0 = m;
        moves._len = 1;
        break;
    case 1:
        moves._mv1 = m;
        moves._len = 2;
        break;
    case 2:
        moves._mv2 = m;
        moves._len = 3;
        break;
    case 3:
        moves._mv3 = m;
        moves._len = 4;
        break;
    case 4:
        moves._mv4 = m;
        moves._len = 5;
        break;
    default:
        break;
    }
    return moves;
}

ui8 _move_is_in(xMoves moves, ui8 m){
    ui8 mn = 0;
    if (moves._mv0 == m) mn = 1;
    else if (moves._mv1 == m) mn = 2;
    else if (moves._mv2 == m) mn = 3;
    else if (moves._mv3 == m) mn = 4;
    else if (moves._mv4 == m) mn = 5;
    return mn > moves._len ? 0 : 1;
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
        if (i == 29) moves = _add_move(moves, i);                                   // always escaping
        else if ((i == 27 || i == 28) && dest == 30) moves = _add_move(moves, i);   // correct escaping
        else if (i != 25 && dest > 25) continue;                                    // forbidden house arriving
        else if (dest > 29) continue;                                               // forbidden escaping
        else if (trgt == 0) moves = _add_move(moves, i);                            // normal movement
        else if (trgt == enemy) {                                                   // attack
            ui8 prev = dest > 0 ? BOARD_GET(s, (dest-1)) : 0;
            ui8 nxt = dest < 29 ? BOARD_GET(s, (dest+1)) : 0;
            if (prev != enemy && nxt != enemy) moves = _add_move(moves, i);
        }
    }
    if (moves._len == 0){   // reverse move
        moves._dir = 2;                      
        ui8 cell, prev = BOARD_GET(s, 0);// s._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(s, i);
            if (cell == agent && prev == 0) moves = _add_move(moves, i);
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

    if (s._steps == 1 || s._steps == 4 || s._steps == 5) s._agent = agent - 1;
    if (moves._len == 0) return s._seed;
    if (_move_is_in(moves, m) == 0) return 0;
    ui8 dest = m - 1;
    if (moves._dir  == 1) dest = m + s._steps;
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
        } else { BOARD_SET(s, dest, enemy) }
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
    float util = eval_basic(seed);
    if (util  == 1 || util == 0 || depth == 0){
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
        if (cm._len == 0) cutil += expectimax_brute(0, depth-1);
        if (cm._len >= 1) cutil += expectimax_brute(increment_1(state._seed, cm._mv0), depth-1);
        if (cm._len >= 2) cutil += expectimax_brute(increment_1(state._seed,cm._mv1), depth-1);
        if (cm._len >= 3) cutil += expectimax_brute(increment_1(state._seed,cm._mv2), depth-1);
        if (cm._len >= 4) cutil += expectimax_brute(increment_1(state._seed,cm._mv3), depth-1);
        if (cm._len == 5) cutil += expectimax_brute(increment_1(state._seed,cm._mv4), depth-1);
        
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
        if (cm._len == 0) cutil += expectimax_timedbrute(0, depth-1);
        if (cm._len >= 1) cutil += expectimax_timedbrute(increment_1(state._seed, cm._mv0), depth-1);
        if (cm._len >= 2) cutil += expectimax_timedbrute(increment_1(state._seed,cm._mv1), depth-1);
        if (cm._len >= 3) cutil += expectimax_timedbrute(increment_1(state._seed,cm._mv2), depth-1);
        if (cm._len >= 4) cutil += expectimax_timedbrute(increment_1(state._seed,cm._mv3), depth-1);
        if (cm._len == 5) cutil += expectimax_timedbrute(increment_1(state._seed,cm._mv4), depth-1);
        
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