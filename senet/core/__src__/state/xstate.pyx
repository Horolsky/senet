from libc.stdlib cimport malloc, free

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

cdef ui8* get_board(xState s, ui8 * board):
    if board is NULL:
        board = <ui8 *> malloc(sizeof(ui8) * 30)
    for i in range(<ui8> 0, <ui8> 30, <ui8> 1):
        board[i] = (s._board >> i * 2) % 4
    return board

cdef xState set_board(xState s, ui8 * board):
    if board is NULL:
        s._bitvalue = 0
    for i in range(<ui8> 0, <ui8> 30, <ui8> 1):
        s._board += (<ui8> board[i] << i * 2)
    return s
cdef xMoves add_move(xMoves moves, ui8 m):
    if moves._len == 0:
        moves._mv0 = m
        moves._len = 1
    elif moves._len == 1:
        moves._mv1 = m
        moves._len = 2
    elif moves._len == 2:
        moves._mv2 = m
        moves._len = 3
    elif moves._len == 3:
        moves._mv3 = m
        moves._len = 4
    elif moves._len == 4:
        moves._mv4 = m
        moves._len = 5
    return moves

cdef xMoves get_moves(xState s):
    cdef ui8* board = get_board(s, NULL)
    cdef xMoves moves
    moves._bitvalue = 0
    cdef ui8 enemy = s._agent % 2 + 1
    cdef ui8 dest

    moves._dir = 1
    for i in range (<ui8> 0, <ui8> 30, <ui8> 1):
        if board[i] != s._agent:
            continue
        dest = i + s._steps
        if i == 29:                                     # always escaping
            moves = add_move(moves, i) 
        elif (i == 28 or i == 29) and dest == 30:       # correct escaping
            moves = add_move(moves, i) 
        elif i != 25 and dest > 25:                     # forbidden house arriving
            continue
        elif dest > 29:                                 # forbidden escaping
            continue
        elif board[dest] == 0:                          # normal movement
            moves = add_move(moves, i)
        elif board[dest] == enemy                       # attack
            cdef ui8 prev = 0
            cdef ui8 nxt = 0
            if dest > 0:
                prev = board[dest-1]
            if dest < 29:
                nxt = board[dest+1]
            if prev != enemy and nxt != enemy:
                moves = add_move(moves, i)
    free(brd)
    return moves
    
    
