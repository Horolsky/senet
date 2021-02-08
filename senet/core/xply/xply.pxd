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
    cdef ui32 get_moves(ui64 seed)
    cdef ui64 increment_1(ui64 seed, ui8 m)
    cdef float eval_basic(ui64 seed)
