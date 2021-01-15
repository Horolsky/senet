from libc.stdlib cimport malloc, free
cimport plyx
from plyx cimport ui8, ui32, ui64, xState, xMoves

cdef ui8* get_board(xState s, ui8 * board):
    if board is NULL:
        board = <ui8 *> malloc(sizeof(ui8) * 30)
    for i from 0 <= i < 30 by 1:
        board[i] = (s._board >> i * 2) % 4
    return board

cdef xState set_board(xState s, ui8 * board):
    if board is NULL:
        s._bitvalue = 0
    for i from 0 <= i < 30 by 1:
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
    for i from 0 <= i < 30 by 1:
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

cdef ui8 is_in(xMoves moves, ui8 m):
    cdef ui8 isin = 0
    if (moves._mv0 == m):
        isin = 1
    elif (moves._mv1 == m):
        isin = 2
    elif (moves._mv2 == m):
        isin = 3
    elif (moves._mv3 == m):
        isin = 4
    elif (moves._mv4 == m):
        isin = 5
    if isin > moves._len:
        isin = 0
    return isin


cdef xState iterate(xState s, ui8 m): #static rules
    cdef xMoves moves = get_moves(s)
    cdef xState nxt = s

    nxt._agent = s._agent % 2 + 1
    if (s._steps == 1 or s._steps == 4 or s._steps == 5): #TODO: SETTINGS dependency
        nxt._agent = s._agent
    if (moves._len == 0):
        return nxt
    if (is_in(moves, m) == 0):
        nxt._bitvalue = 0
        return nxt
    
    cdef ui8 enemy = s._agent % 2 + 1
    cdef ui8 dest = m - 1
    if moves._dir  == 1:
        dest = m + s._steps

    cdef ui8 board[30]
    for i from 0 <= i < 30 by 1:
        board[i] = (s._board >> i * 2) % 4

    #drowing in House of Water
    if dest == 26:
        for i from 14 > i >= 0 by -1:
            if board[i] == 0:
                dest = <ui8>i
                break
        board[m] = 0
        board[dest] = s._agent
    #escaping
    elif (m == 29 or dest == 30):
        board[m] = 0
    #attacking
    elif (board[dest] == enemy):
        board[dest] = s._agent
        if dest > 26: #attacking Houses rule
            for i from 14 > i >= 0 by -1:
                if (board[i] == 0):
                    board[i] = enemy
                    break
        else: 
            board[m] = enemy
    #moving to empty target
    elif board[dest] == 0:
        board[dest] = s._agent
        board[m] = 0
    #corrupted logic
    else: 
        nxt._bitvalue = 0 
        return nxt

    nxt._board = 0
    for i from 0 <= i < 30 by 1:
        nxt._board += ( <ui64> board[i] << i * 2)

    return nxt

cdef double utility(xState s):
    cdef int maxSum = 0
    cdef int minSum = 0
    cdef int cell
    for i from 0 <= i < 30 by 1:
        cell = (s._board >> i * 2) % 4
        if cell == 1:
            maxSum += (30 - i)
        elif cell == 2:
            minSum += (30 - i)
    return <double> (30 - maxSum + minSum) / 60  # ut2: diff ratio to start
