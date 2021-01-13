#ifndef SENET_STATE_H
#define SENET_STATE_H

typedef unsigned long long int statedata;
/*
 * game state data, compressed through bitfields
 * avoid to change values directly
 */
typedef struct _SenetState {
    union {
        /*
         * 1st bit for agent, 2d-4d for steps, other for board
         * right to left indexation
         */
        statedata _bitvalue; 
        struct {
            statedata _agent:1; // current ply active agent 
            statedata _steps:3; // 1-5
            statedata _board:60;// 30x array, 
        };
    };
} sState; 

char sState_agent(sState);
char sState_steps(sState);
void sState_board(sState, char* board);

sState sState_setAgent(sState, char agent);
sState sState_setSteps(sState, char steps);
sState sState_setBoard(sState, char* board);

sState sState_setState(char agent, char steps, char* board);

#endif
