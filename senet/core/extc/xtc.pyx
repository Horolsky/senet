from libc.stdlib cimport malloc, free



cdef extern from "xtcstate.h":
    ctypedef unsigned long long int statedata
    cdef struct _SenetState:
        statedata _bitvalue
        statedata _agent
        statedata _steps
        statedata _board
    ctypedef _SenetState sState
        
    cdef char sState_agent(sState)
    cdef char sState_steps(sState)
    cdef void sState_board(sState, char* board)

    cdef sState sState_setAgent(sState, char agent)
    cdef sState sState_setSteps(sState, char steps)
    cdef sState sState_setBoard(sState, char* board)

    cdef sState sState_setState(char agent, char steps, char* board)

cdef test_state():
    cdef char * b = <char *> malloc(sizeof(char) * 30)
    for i in range(30):
        b[i] = 0
    b[3] = 1
    b[5] = 2
    cdef int a = 1
    cdef int s = 3
    cdef sState test = sState_setState(a, s, b)
    free(b)

    print("agent: " + str(sState_agent(test)))
    print("steps: " + str(sState_steps(test)))
    print("size: " + str(sizeof(sState)))

    print("size sd: " + str(sizeof(statedata)))
    print("bitval: " + str(test._bitvalue))
    print("bit ag: " + str(test._agent))
    print("bit st: " + str(test._steps))
    print("bit bd: " + str(test._board))


def test():
    print("XTC is here!")
    test_state()
cdef sState _set_board(sState s, char * board):
    s._board = 0
    for i in range(30):
        s._board += (board[i] << i * 2)
    return s

cdef sState _makeState(int a, int b):
    cdef sState test
    test._agent = a
    test._steps = b
    cdef char * brd = <char *> malloc(sizeof(char) * 30)
    for i in range(30):
        brd[i] = 0
    brd[3] = 1
    brd[5] = 2
    test = _set_board(test, brd)
    free(brd)
    print(bin(test))
    return test

def makeState(a, b):
    a = int(a)
    b = int(b)
    test = _makeState(a, b)
    print(test)
    print(sizeof(test))

    return test