//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include "senetstate.h"

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