# distutils: sources = src-xtc.c
# distutils: include_dirs = ./

from libc.stdlib cimport malloc, free
cimport xtc
from xtc cimport ui8, ui32, ui64, eval_id_e, incr_id_e 
from json import dumps

def emax(ui64 state, ui8 depth, ui8 sec, eval_id_e id_eval=id_eval_basic, incr_id_e id_incr=id_incr_1):
    """
    best strategy expectiminimax multithread search
    @param state: 64-bit uint state seed
    @param depth: 8-bit uint depth of search 
    @param sec: 8-bit uint stoptimer
    """
    cdef xtc.emax_res result
    result = xtc.get_strategy_emax_mt(state, depth, sec, id_eval, id_incr)
    return (result.strategy, result.searched_nodes)


cdef class Ply():
    """
    game state class
    """
    cdef dict __cache
    cdef tuple __event
    cdef tuple __bench
    cdef xtc.xState __xstate
    cdef xtc.xMoves __xmoves
    cdef xtc.state_increment_func _increment
    cdef xtc.state_legal_moves_func _get_moves
    cdef xtc.state_evaluation_func _eval
    cdef incr_id_e _incr_id
    cdef eval_id_e _eval_id

    def __init__(self, ui64 seed=10066320, incr_id_e incr_id=id_incr_1, eval_id_e eval_id=id_eval_basic):
        """
        @param seed: 64-bit integer xstate seed
        @param incr_id: increment func id
        @param eval_id: eval func id
        """
        self.incr_func = incr_id
        self.eval_func = eval_id
        
        self._eval = xtc.get_evaluation_func(eval_id)

        self.__xstate._seed = seed
        self.__cache = {
            "moves": None,
            "board": None,
            "utility": None,
        }
        self.__event = (seed, 0,0,0,0) #this updates on iteration after init
        self.__bench = Ply.get_bench(seed)

    @property
    def incr_func(self):
        """
        id of increment function
        0 - default
        """
        return self._incr_id

    @incr_func.setter
    def incr_func(self, incr_id_e func):
        self._incr_id = func
        self._increment = xtc.get_increment_func(func)
        self._get_moves = xtc.get_legal_moves_func(func)
    
    @property
    def eval_func(self):
        """
        id of evaluation function
        0 - basic, range  0, 1
        1 - basic, range -1, 1
        """
        return self._eval_id
        
    @eval_func.setter
    def eval_func(self, eval_id_e func):
        self._eval_id = func
        self._eval = xtc.get_evaluation_func(func)

    @property
    def agent(self):
        return self.__xstate._agent + 1
    @agent.setter
    def agent(self, ui8 agent): #unsafe
        self.__xstate._agent = agent -1
        self.__cache["moves"] = None
    @property
    def enemy(self):
        return self.agent % 2 + 1
    
    @property
    def steps(self):
        return self.__xstate._steps
    @steps.setter
    def steps(self, ui8 val): #unsafe
        self.__xstate._steps = val
        self.__cache["moves"] = None
    @property
    def board(self):
        if self.__cache["board"] is None:
            _board = []
            for i from 0 <= i < 30 by 1:
                _board.append((self.__xstate._board >> i * 2) % 4)
            self.__cache["board"] = tuple(_board)
        return self.__cache["board"]
    @board.setter
    def board(self, board): #unsafe
        if type(board) not in (list, tuple):
            print('fuck2')
            return
        if len(board) != 30:
            print('fuck2')
            return
        self.__xstate._board = 0
        for i from 0 <= i < 30 by 1:
            self.__xstate._board += ( <ui64> board[i] << i * 2)
        self.__cache["moves"] = None
        self.__cache["board"] = None
        self.__cache["utility"] = None

    def __upd_moves(self):
        moves = []
        cdef xtc.xMoves xmoves
        xmoves._seed = self._get_moves(self.__xstate._seed)
        #xmoves._seed = xtc.get_moves_1(self.__xstate._seed)
        for i in range(xmoves._len):
            moves.append((xmoves._mvs >> (i * 5)) % 32)

        self.__cache["moves"] = tuple(moves)
        self.__xmoves = xmoves

    @property
    def moves(self):
        if self.__cache["moves"] is None:
            self.__upd_moves()
        return self.__cache["moves"]
    
    @property
    def mobility(self):
        if self.__cache["moves"] is None:
            self.__upd_moves()
        return (0,1,-1)[self.__xmoves._dir]

    @property
    def event(self):
        """
        last turn info
        (<agent>, <eventcode>, <start>, <destination>)
        codes:
        0 - drow
        1 - reverse
        2 - skip
        3 - move
        4 - attack
        5 - escape 
        """
        return self.__event     

    @property
    def bench(self):
        return self.__bench


    def increment(self, start):
        """
        event handling wrapper
        calling xtc.increment for movement handling
        return new Ply instance
        None if given movement is impossible
        """

        if self.mobility != 0 and start not in self.moves:
            return None
        
        dest = 0                            # skip default
        code = 2    
        if self.mobility == -1:          # reverse
            dest = start - 1                    
            code = 1    
            if dest == 26:                  # drow
                code = 0    
        elif self.mobility == 1:        # forvard
            dest = start + self.steps       
            code = 3    
            if dest == 26:                  # drow
                code = 0
            elif dest > 29:                 # escape
                code = 5
            elif self.board[dest] != 0:     # attack
                code = 4

        event = (self.__xstate._seed, self.agent, code, start, dest)
        
        iteration = Ply(self._increment(self.__xstate._seed, start))
        iteration.__event = event
        return iteration
    @property
    def seed(self):
        return self.__xstate._seed

    @property
    def utility(self):
        if self.__cache["utility"] is None:
            self.__cache["utility"] = self._eval(self.__xstate._seed)
        return self.__cache["utility"]
    
    def to_json(self):
        return dumps(self.event)
    
    
    def to_csv(self):
        return " ".join([('_','V','X')[c] for c in self.board]) + f" ;{self.agent};{self.steps};{self.utility};{self.__xstate._seed}" 

    @staticmethod
    def get_bench(ui64 seed):
        cdef xtc.xState state
        state._seed = seed
        a1, a2 = 5, 5
        for i from 0 <= i < 30 by 1:
            cell = (state._board >> i * 2) % 4
            if cell == 1:
                a1 -= 1
            elif cell == 2:
                a2 -= 1
        return (a1, a2)



    