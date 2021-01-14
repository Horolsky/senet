from libc.stdlib cimport malloc, free
cimport xstate
from xstate cimport ui8, ui32, ui64, xState, xMoves

cdef int _start = 0
cdef int _stop = 30
cdef int _step = 1

cdef ui8* get_board(xState s, ui8 * board):
    if board is NULL:
        board = <ui8 *> malloc(sizeof(ui8) * 30)
    #for i in range(_start, _stop, _step):
    for i from _start <= i < _stop by _step:
        board[i] = (s._board >> i * 2) % 4
    return board

cdef xState set_board(xState s, ui8 * board):
    if board is NULL:
        s._bitvalue = 0
    for i from _start <= i < _stop by _step:
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
    cdef ui8 prev = 0
    cdef ui8 nxt = 0

    moves._dir = 1
    for i from _start <= i < _stop by _step:
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
        elif board[dest] == enemy:                      # attack
            if dest > 0:
                prev = board[dest-1]
            if dest < 29:
                nxt = board[dest+1]
            if prev != enemy and nxt != enemy:
                moves = add_move(moves, i)
    free(board)
    return moves
