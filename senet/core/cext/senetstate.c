#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "senetstate.h"
typedef unsigned long long int statedata;
typedef struct _SenetState {
    union {
        statedata bitvalue; //1 bit for agent, 2-4 for steps, other for board
        struct {
            statedata agent:1; // current ply active agent 
            statedata steps:3; // 1-5
            statedata board:60;// 30x array, 
        };
    };
} sState; // 64 bit state data

