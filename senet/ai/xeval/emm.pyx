from senet.core.xply cimport ui8, ui64, xState, xMoves, xPly# get_utility, get_moves, increment
#def test():
#    cdef xPly x = xPly(1)
#    return x
cdef ui64 counter = 0
cdef float xemm(ui64 bitval, ui8 depth):
    global counter
    cdef xPly ply = xPly(bitval)
    cdef float util = ply.get_utility()
    if util  == 1 or util == 0 or depth == 0:
        return util
    
    cdef xMoves cm
    cdef float P[5]
    P[:] = [ .25, .375, .25, .0625, .0625 ]
    cdef float result = 0
    cdef float cutil
    
    for steps from 1 <= steps < 6 by 1:
        cutil = 0
        ply.set_steps(steps)
        cm = ply._xmoves
        if (cm._len == 0):
            cutil += xemm(0, depth-1)
        if (cm._len >= 1):
            cutil += xemm(ply.increment(cm._mv0), depth-1)
        if (cm._len >= 2):
            cutil += xemm(ply.increment(cm._mv1), depth-1)
        if (cm._len >= 3):
            cutil += xemm(ply.increment(cm._mv2), depth-1)
        if (cm._len >= 4):
            cutil += xemm(ply.increment(cm._mv3), depth-1)
        if (cm._len == 5):
            cutil += xemm(ply.increment(cm._mv4), depth-1)
        
        result += cutil * P[steps-1]
        if cm._len != 0:
            result /= cm._len
        counter += cm._len
    return result

def emm(ui64 state, ui8 depth):
    global counter
    counter = 0
    cdef float res = xemm(state, depth)
    return (res, counter)
