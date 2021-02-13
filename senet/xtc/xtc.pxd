cdef extern from "src-xtc.h": 
    ctypedef char ui8
    ctypedef unsigned long int ui32
    ctypedef unsigned long long int ui64
    cdef struct _xState: 
        ui64 _seed 
        ui64 _agent
        ui64 _steps
        ui64 _board
    ctypedef _xState xState
    cdef struct _xMoves: 
        ui32 _seed 
        ui32 _dir
        ui32 _len
        ui32 _mvs
    ctypedef _xMoves xMoves
    cdef ui32 get_moves(ui64 seed)
    cdef ui64 increment_1(ui64 seed, ui8 m)
    cdef float eval_basic(ui64 seed)
    cdef struct _emax_test:
        float res
        ui32 count
    ctypedef _emax_test emax_test
    cdef emax_test expectimax_count(ui64 seed, ui8 depth)
    cdef emax_test expectimax_timecount(ui64 seed, ui8 depth, ui8 sec)

    cdef struct _emax_res:
        ui8 res
        ui32 count
    ctypedef _emax_res emax_res
    cdef emax_res expectimax_multithread(ui64 seed, ui8 depth, ui8 sec)
    