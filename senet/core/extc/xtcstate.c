#include "xtcstate.h"
//#include "xtcrules.h"

char sState_agent(sState s){
    return (char) s._agent;
}
char sState_steps(sState s){
    return (char) s._steps;
}
void sState_board(sState s, char*board){
    for (int i = 0; i < 30; i++) board[i] = (s._board >> i * 2) % 4;    
}

sState sState_setAgent(sState s, char agent){
    s._agent = agent;
    return s;
}
sState sState_setSteps(sState s, char steps){
    s._steps = steps;
    return s;
}
sState sState_setBoard(sState s, char* board){
    s._board = 0;
    for (int i = 0; i < 30; i++) s._board += ((statedata) board[i] << i * 2);
}

sState sState_setState(char agent, char steps, char* board){
    sState s = {0};
    s._agent = agent;
    s._steps = steps;
    for (int i = 0; i < 30; i++) s._board += ((statedata) board[i] << i * 2);
}

sMoves __addMove(sMoves moves, char m){
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

sMoves sState_getMoves(sState s){
    char* board = malloc(sizeof(char) * 30);
    sState_board(s, board);
    char agent = s._agent, steps = s._steps;
    char enemy = s._agent % 2 + 1;
    char dest;//dest
    sMoves moves = {0};
    moves._dir = 1;
    for (int i = 0; i < 30; i++){
        if (board[i] != s._agent) continue;
        dest = i + s._steps;
        if (i == 29 ) __addMove(moves, i); // always escaping
        else if ((i == 28 || i == 29) && dest == 30 ) __addMove(moves, i); //correct escaping
        else if (i != 25 && dest > 25 ) continue; // forbidden house arriving
        else if (dest > 29) continue; //forbidden escaping
        else if (board[dest] == 0) moves = __addMove(moves, i);
        else if (board[dest] == enemy){
            char prev, next;
            prev = dest >  0 ? board[dest-1] : 0;
            next = dest < 29 ? board[dest+1] : 0;
            if (prev != enemy && next != enemy) moves = __addMove(moves, i);
        }
    }
    //backward moves
    if (moves._len == 0){
        moves._dir = 2;
        for (int i = 1; i < 30; i++){
            if (board[i] == s._agent && board[i-1] == 0) __addMove(moves, i);
        }   
    }
    if (moves._len == 0) moves._dir = 0;
    free(board);
    return moves;
}

char _check_move(sMoves moves, char m){
    if (m < 1 || m > 29) return 0;
    if (moves._mv0 == m) return 1;
    if (moves._mv1 == m) return 1;
    if (moves._mv2 == m) return 1;
    if (moves._mv3 == m) return 1;
    if (moves._mv4 == m) return 1;
    return 0;
}

static char _board[30];
void _get_board(sState s){ for (int i = 0; i < 30; i++) _board[i] = (s._board >> i * 2) % 4; }
sState _set_board(sState s){
    s._board = 0;
    for (int i = 0; i < 30; i++) s._board += ((statedata) _board[i] << i * 2);
}

sState sState_iterate(sState s, char m){
    //const sRules* RULES = sRules_get();
    const sMoves moves = sState_getMoves(s);
    sState next = s;
    next._steps = 0;
    next._agent = s._agent % 2 + 1;
    if (s._steps == 1 || s._steps == 4 || s._steps == 5) next._agent = s._agent;
    /*for (int i = 0; i < 5; i++){
        if (RULES->addmoves[i] == s._steps){
            next._agent = s._agent;
            break;
        }
    }*/
    
    if (moves._len == 0) return next;
    if (_check_move(moves, m) == 0) return (sState){0};//invalid arg

    _get_board(s);
    char enemy = s._agent % 2 + 1;
    char dest = moves._dir  == 1 ? m + s._steps : m - 1;

    //drowing in House of Water
    if (dest == 26){
        for (int i = 14; i >=0; i--) {
                    if (_board[i] == 0){
                        dest = i;
                        break;
                    }       
        }
        _board[m] = 0;
        _board[dest] = s._agent;
    } 
    //escaping
    else if (m == 29 || dest == 30) {
        _board[m] = 0;
    }
    //attacking
    else if (_board[dest] == enemy) {
        _board[dest] = s._agent;
        if (dest > 26) { //attacking Houses rule
            for (int i = 14; i >=0; i--) {
                    if (_board[i] == 0){
                        _board[i] = enemy;
                        break;
                    }       
            }
        }
        else {
            _board[m] = enemy;
        }
    }
    //moving to empty target
    else if (_board[dest] == 0){
        _board[dest] = s._agent;
        _board[m] = 0;
    }
    else {
        next = (sState) {0};//corrupted logic
    }

    next = _set_board(next);
    return next;
}