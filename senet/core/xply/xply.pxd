cdef extern from "extern-src-xply.c": 
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
        ui32 _mv0
        ui32 _mv1
        ui32 _mv2
        ui32 _mv3
        ui32 _mv4
    ctypedef _xMoves xMoves
    cdef xMoves _get_moves(xState s)
    cdef xState increment_1(xState s, ui8 m)
    cdef float eval_basic(xState s)
cdef ui8* get_board(xState s, ui8 * board)
cdef xState set_board(xState s, ui8 * board)
cdef xMoves get_moves(xState s)
cdef xState increment(xState s, ui8 m)
cdef float utility(xState s)
cdef class xPly:
    cdef readonly xState _xstate
    cdef readonly xMoves _xmoves
    #def __init__(self, ui64 seed)
    cdef void set_steps(self, ui8 steps)
    cdef void set_agent(self, ui8 agent)
    cdef void upd_xmoves(self)
    cdef ui64 increment(self, ui8 move)
    cdef float get_utility(self)