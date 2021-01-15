from libc.stdlib cimport malloc, free
cimport xply
from xply cimport ui8, ui32, ui64, xState, xMoves

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
    cdef ui8 agent = s._agent + 1
    cdef ui8 enemy = agent % 2 + 1
    cdef ui8 dest
    cdef ui8 prev = 0
    cdef ui8 nxt = 0

    moves._dir = 1
    for i from 0 <= i < 30 by 1:
        if board[i] != agent:
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

    cdef ui8 agent = s._agent + 1
    cdef ui8 enemy = agent % 2 + 1

    nxt._agent = enemy - 1
    if (s._steps == 1 or s._steps == 4 or s._steps == 5): #TODO: SETTINGS dependency
        nxt._agent = agent - 1
    if (moves._len == 0):
        return nxt
    if (is_in(moves, m) == 0):
        nxt._bitvalue = 0
        return nxt
    
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
        board[dest] = agent
    #escaping
    elif (m == 29 or dest == 30):
        board[m] = 0
    #attacking
    elif (board[dest] == enemy):
        board[dest] = agent
        if dest > 26: #attacking Houses rule
            for i from 14 > i >= 0 by -1:
                if (board[i] == 0):
                    board[i] = enemy
                    break
        else: 
            board[m] = enemy
    #moving to empty target
    elif board[dest] == 0:
        board[dest] = agent
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

cdef class xPly():
    """
    class for low-level game-state managment
    """
    cdef readonly xState _xstate
    cdef readonly xMoves _xmoves

    def __init__(self, ui64 bitval):
        self._xstate._bitvalue = bitval
        self.upd_xmoves()

    cdef void set_steps(self, ui8 steps):
        self._xstate._steps = steps
        self.upd_xmoves()

    cdef void set_agent(self, ui8 agent):
        self._xstate._agent = agent
        self.upd_xmoves()
    
    cdef void upd_xmoves(self):
        self._xmoves = get_moves(self._xstate)

    cdef ui64 iterate(self, ui8 move):
        return iterate(self._xstate, move)._bitvalue

    cdef double get_utility(self):
        return utility(self._xstate)
    
cdef class Ply(xPly):
    """
    xPly Python wrapper 
    """
    cdef dict _cache

    def __init__(self, bitval=10066320):
        """
        @param bitval: 64-bit integer xstate bitvalue
        """
        self._cache = {
            "moves": None,
            "board": None,
            "utility": None
        }
        xPly.__init__(self, bitval)
    @property
    def agent(self):
        return self._xstate._agent + 1
    @agent.setter
    def agent(self, ui8 agent):
        self.set_agent(agent - 1)
        self._cache["moves"] = None

    @property
    def steps(self):
        return self._xstate._steps
    @steps.setter
    def steps(self, ui8 val):
        self.set_steps(val)
        self._cache["moves"] = None
    @property
    def board(self):
        if self._cache["board"] is None:
            _board = []
            for i from 0 <= i < 30 by 1:
                _board.append((self._xstate._board >> i * 2) % 4)
            self._cache["board"] = tuple(_board)
        return self._cache["board"]
    @board.setter
    def board(self, board):
        if type(board) not in (list, tuple):
            return
        if len(board) != 30:
            return
        for i from 0 <= i < 30 by 1:
            self._xstate._board += ( <ui64> board[i] << i * 2)
        self.upd_xmoves()
        self._cache["moves"] = None
        self._cache["board"] = None
        self._cache["utility"] = None

    def __upd_moves(self):
        moves = []
        if (self._xmoves._len >= 1):
            moves.append(self._xmoves._mv0)
        if (self._xmoves._len >= 2):
            moves.append(self._xmoves._mv1)
        if (self._xmoves._len >= 3):
            moves.append(self._xmoves._mv2)
        if (self._xmoves._len >= 4):
            moves.append(self._xmoves._mv3)
        if (self._xmoves._len == 5):
            moves.append(self._xmoves._mv4)
        self._cache["moves"] = tuple(moves)

    @property
    def moves(self):
        if self._cache["moves"] is None:
            self.__upd_moves()
        return self._cache["moves"]

    def iterate(self, move):
        """
        call xply.iterate for movement handling
        return new xstate bitvalue
        0 if given move is impossible
        """
        return xPly.iterate(self, move)

    @property
    def utility(self):
        if self._cache["utility"] is None:
            self._cache["utility"] = self.get_utility() 
        return self._cache["utility"]