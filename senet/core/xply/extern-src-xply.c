typedef unsigned char ui8;
typedef unsigned int ui16;
typedef unsigned long int ui32; 
typedef unsigned long long int ui64;
typedef struct _xState {
    union {
        /*
         * 1st bit for agent, 2d-4d for steps, other for board
         * right to left indexation
         */
        ui64 _seed; 
        struct {
            ui64 _agent:1; // current ply active agent 
            ui64 _steps:3; // 1-5, 0 for unset
            ui64 _board:60;// 30x array, 
        };
    };
} xState; 
typedef struct _xMoves{
    union {
        /*
         * 1st bit for direction, 2d for length, other for 5 movement indices 
         * right to left indexation
         */
        ui32 _seed; 
        struct {
            ui32 _dir:2; // 0: no movement, 1: normal, 2: backward
            ui32 _len:3; // length of avaliable movements
            ui32 _mv0:5;
            ui32 _mv1:5;
            ui32 _mv2:5;
            ui32 _mv3:5;
            ui32 _mv4:5;
        };
    };
} xMoves;
#define BOARD_GET(state, cell) ((state._board >> (cell * 2)) % 4)
#define BOARD_SET(state, cell, val) \
state._board &= ~(3 << (cell * 2));    \
state._board ^= (val << (cell * 2));   \

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

ui8 move_is_in(xMoves moves, ui8 m){
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
        ui8 cell, prev = s._board % 4;                                       
        for (ui8 i = 1; i < 30; i++) {
            cell = (s._board >> i * 2) % 4;
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
    if (move_is_in(moves, m) == 0) return 0;
    ui8 dest = m - 1;
    if (moves._dir  == 1) dest = m + s._steps;
    //drowing in House of Water
    if (dest == 26){
        for (ui8 i = 13; i >= 0; i--){
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
            for (ui8 i = 13; i >= 0; i--){
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
        cell = BOARD_GET(s, i);//(s._board >> i * 2) % 4;
        if (cell == 1) maxSum += (30 - i);
        else if (cell == 2) minSum += (30 - i);
    }
    float ut;
    if (minSum == 0) ut = 0;
    else if (maxSum == 0) ut = 0;
    else ut = (float) (minSum) / (maxSum + minSum);
    return ut;
}
//from senet.core.xply import Ply, incr, get_state, get_moves