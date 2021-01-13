#ifndef SENET_STATE_H
#define SENET_STATE_H

typedef struct _SenetState sState;

int sState_agent(sState*);
int sState_steps(sState*);
int sState_board(sState*);

#endif
