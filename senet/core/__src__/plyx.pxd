cdef int _start = 0
cdef int _stop = 30
cdef int _step = 1

cdef extern from *: 
    """
    typedef unsigned char ui8;
    typedef unsigned int ui16;
    typedef unsigned long int ui32; 
    typedef unsigned long long int ui64;
    typedef struct _xState {
        union {
            /*
             * 1st bit for agent, 2d-4d for steps, other for board
             * right to left indexation
             */
            ui64 _bitvalue; 
            struct {
                ui64 _agent:1; // current ply active agent 
                ui64 _steps:3; // 1-5, 0 for unset
                ui64 _board:60;// 30x array, 
            };
        };
    } xState; 
    typedef struct _xMoves{
        union {
            /*
             * 1st bit for direction, 2d for length, other for 5 movement indices 
             * right to left indexation
             */
            ui32 _bitvalue; 
            struct {
                ui32 _dir:2; // 0: no movement, 1: normal, 2: backward
                ui32 _len:3; // length of avaliable movements
                ui32 _mv0:5;
                ui32 _mv1:5;
                ui32 _mv2:5;
                ui32 _mv3:5;
                ui32 _mv4:5;
            };
        };
    } xMoves;
    """ 
    ctypedef char ui8
    ctypedef unsigned long int ui32
    ctypedef unsigned long long int ui64
    cdef struct _xState: 
        ui64 _bitvalue 
        ui64 _agent
        ui64 _steps
        ui64 _board
    ctypedef _xState xState
    cdef struct _xMoves: 
        ui32 _bitvalue 
        ui32 _dir
        ui32 _len
        ui32 _mv0
        ui32 _mv1
        ui32 _mv2
        ui32 _mv3
        ui32 _mv4
    ctypedef _xMoves xMoves
    
cdef ui8* get_board(xState s, ui8 * board)
cdef xState set_board(xState s, ui8 * board)
cdef xMoves get_moves(xState s)
cdef xState iterate(xState s, ui8 m)