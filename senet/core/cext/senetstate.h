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
            statedata _steps:3; // 1-5, 0 for unset
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

typedef long unsigned int movesdata;
typedef struct _sMoves{
    union {
        /*
         * 1st bit for direction, 2d for length, other for 5 movement indices 
         * right to left indexation
         */
        movesdata _bitvalue; 
        struct {
            movesdata _dir:2; // 0: no movement, 1: normal, 2: backward
            movesdata _len:3; // length of avaliable movements
            movesdata _mv0:5;
            movesdata _mv1:5;
            movesdata _mv2:5;
            movesdata _mv3:5;
            movesdata _mv4:5;
        };
    };
} sMoves;

sMoves sState_getMoves(sState); //get avaliable moves

sState sState_iterate(sState, char move); //return {0} if movement is unavaliable

#endif
