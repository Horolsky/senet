cimport xstate
from xstate cimport ui8, ui64, xState, xMoves

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
        self._xmoves = xstate.get_moves(self._xstate)

    cdef ui64 iterate(self, ui8 move):
        return xstate.iterate(self._xstate, move)._bitvalue

    cdef double get_utility(self):
        return xstate.utility(self._xstate)
    
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
        call xstate.iterate for movement handling
        return new xstate bitvalue
        0 if given move is impossible
        """
        return xPly.iterate(self, move)

    @property
    def utility(self):
        if self._cache["utility"] is None:
            self._cache["utility"] = self.get_utility() 
        return self._cache["utility"]