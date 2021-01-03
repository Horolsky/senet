from json import dumps
class state():
    def __init__(self, board, agent, steps, event = None):
        """
        immutable game state class
        @param board: list or tuple
        @param agent: int
        @param steps: int
        @param event: tuple
        """
        if not state.board_valid(board):
            raise ValueError("invalid board data")
        self.__board = tuple(board)
        if agent not in [1,2]:
            raise ValueError("invalid agent number")
        self.__agent = agent
        self.__enemy = agent % 2 + 1
        
        if event is None:
            event = (agent, 0,0,0,0)
        else:
            if type(event) is not tuple:
                raise TypeError("invalid event type")
            if len(event) != 5:
                raise ValueError("corrupted event data")
        self.__event = event
        if steps not in [1,2,3,4,5]:
            raise ValueError("invalid steps value")
        self.__steps = steps
        self.__am = steps in [1,4,5] #additional move
        self.__bench = state.get_bench(board)
        #cached data
        self.__moves = None
        self.__mobility = None
        self.__utility = None
    
    #main data getters
    @property
    def board(self):
        """
        a tuple of numbers, representing game board
        0 is for an empty cell
        1 and is for a pawn of a corresponding agent
        """
        return self.__board
    
    @property
    def agent(self): 
        """
        current player number
        1 or 2
        """
        return self.__agent
    
    @property
    def enemy(self):
        """
        current player's enemy number
        1 or 2
        """
        return self.__enemy
    
    @property
    def bench(self):
        """
        a tuple with number of escaped pawns for both teams
        """
        return self.__bench

    @property
    def steps(self):
        """
        number of steps to move forward,
        calculated from thrown sticks.
        if mobility is 0 or -1,
        this value stays unchanged
        """
        return self.__steps
    
    @property
    def additional_move(self):
        """
        1 if next move belongs to the same agent
        0 otherwise
        """
        return self.__am
    
    @property
    def event(self):
        """
        a tuple with info on the previous turn event
        (<agent>, <eventcode>, <start>, <destination>, <victim's destination>)
        codes:
        0 - skip movement
        1 - reverse movement
        2 - moving to empty cell
        3 - swaping
        4 - drowing and reborning
        5 - swaping on Houses with reborning
        6 - escaping 
        """
        return self.__event
    
    @property
    def moves(self):
        """
        if no possible moves for current steps,
        reverse moves will be calculated
        """
        if self.__moves is None:
            self.__set_cached_data()
        return self.__moves
    
    @property
    def mobility(self):
        """
        1 for normal movement
        0 for no possible moves
        -1 for reverse movement
        """
        if self.__mobility is None:
            self.__set_cached_data()
        return self.__mobility
    
    def to_json(self, util=False):
        data = {
            "board": self.board,
            "agent": self.agent,
            "steps": self.steps,
            "moves": self.moves,
            "event": self.event
        }
        return dumps(data)


    def increment(self, cell, newsteps):
        """
        move choosen pawn
        returns a new state on success or None on failure
        if no possible moves, returns the same state with swaped agent
        """
        next_player = (self.enemy, self.__agent)[self.__am]
        # skipping move
        if self.__mobility == 0:
            return state(self.__board, next_player, newsteps, self.__event)
        # incorrect cell index
        if cell not in self.__moves:
            return None
        # movement cases
        steps = self.__steps
        destination = cell + steps #target
        board = list(self.__board) #copy
        agent = self.__agent
        enemy = self.__enemy
        event = self.__event 
        #reverse
        if self.__mobility == -1:
            if cell == 27: #reverse to House of Waters
                board[cell] = 0
                for i in range(14, -1, -1):
                    if board[i] == 0:
                        board[i] = agent
                        event = (agent, 1, cell, i, -1)
                        break
            else:
                board[cell] = 0
                board[cell-1] = agent 
                event = (agent, 1, cell, cell-1, -1)          
        #drowing in House of Water
        elif destination == 26: 
            board[cell] = 0
            for i in range(14, -1, -1):
                if board[i] == 0:
                    board[i] = agent
                    event = (agent, 4, cell, destination, i)
                    break
        #escaping
        elif cell == 29: 
            board[29] = 0
            event = (agent, 6, 29, destination, -1)
        elif destination == 30 and cell == (30 - steps):
            board[cell] = 0
            event = (agent, 6, cell, destination, -1)
        #attacking
        elif board[destination] == enemy:
            if self.cell_defended(destination):
                board[destination] = agent                
                if destination > 26: #attacking Houses rule
                    for i in range(14, -1, -1):
                        if board[i] == 0:
                            board[i] = enemy
                            board[cell] = 0
                            event = (agent, 5, cell, destination, i)
                            break
                else:
                    board[cell] = enemy
                    event = (agent, 3, cell, destination, cell)
        #moving to empty target
        elif board[destination] == 0:
            board[destination] = agent
            board[cell] = 0
            event = (agent, 2, cell, destination, -1)
        else:
            raise ValueError(f"incorrect move logic, start: {cell}")
        return state(board, next_player, newsteps, event)

    def __set_cached_data(self):
        mobility = 1 # 1 for normal movement, -1 for reverse, 0 for skipping
        moves = self.__get_moves(self.__agent, self.__steps)
        if len(moves) == 0: #no direct moves
            mobility = -1
            moves = self.__get_moves(self.__agent, -1)
            if len(moves) == 0: #no reverse moves
                mobility = 0
        self.__moves = moves
        self.__mobility = mobility

    def __get_moves(self, agent, steps):
        """
        returns list of possible moves for a given state
        """
        if steps not in [-1,1,2,3,4,5] or agent not in [1,2]:
            raise ValueError("corrupted data")
        board = self.__board
        moves = []
        enemy = agent % 2 +1
        
        #reverse moves
        if steps == -1:
            for cell in range(28):
                if board[cell] == 0 and board[cell + 1] == agent:
                    moves.append(cell+1)
            return moves
        #else: normal moves, filling array by exclusion:
        for cell in range(30): #cell index
            #cell is not under control
            if board[cell] != agent:
                continue
            destination = cell + steps #destination index
            #escaping cases
            if cell == 29: #always escaping 
                moves.append(cell)
                continue
            if cell in [27,28] and destination == 30: #correct escaping
                moves.append(cell)     
                continue
            if destination > 25 and cell != 25: #incorrect house arriving
                continue
            if destination > 29: #incorrect escaping
                continue
            #target occupation cases
            target = board[destination] #target cell value
            if target == agent: #friendly occupation
                continue
            elif target == enemy and self.cell_defended(destination): #defended target
                continue
            #final default case
            moves.append(cell)
        return tuple(moves)  
    
    def cell_defended(self, pos):
        board = self.board
        if board[pos] == 0:
            return False
        if pos > 0 and board[pos - 1] == board[pos]:
            return True
        if pos < 29 and board[pos + 1] == board[pos]:
            return True
        return False
        
    #utility func for minimax
    @property
    def utility(self):
        """
        returns maximin for current agent
        - approximation of total of estimated moves
        - n of possible moves
        - n of defenced cells
        - 
        """
        # DATA COLLECTING
        board = self.board
        ag_pws, en_pws = 0, 0 # num of pawns
        ag_em, en_em = 0, 0 # estimated moves to escape
        ag_def, en_def = 0, 0 # defended cells
        
        for c in range(30):
            if board[c] == 0:
                continue
            m = (30 - c) // 3 # estimated moves to escape
            #(lambda: enemies.append(c), lambda: friends.append(c))[b[c] == self.agent]()
            if board[c] == self.agent:
                ag_pws += 1
                ag_em += m
                if self.cell_defended(c):
                    ag_def += c
            else:
                en_pws += 1
                en_em += m
                if self.cell_defended(c):
                    en_def += c
        #SUMMANDS DETERMINING
        # EM - estimated moves ratio (agent to enemy)
        # DF - defended cells factor
        # MF - movement freedom ratio (free to total)
        # AM - additional move factor
        # MB - mobility factor
        EM = 1
        if (ag_em + en_em != 0):
            EM = 1 - ag_em / (ag_em + en_em)
        
        DF = 1
        if (ag_def + en_def != 0):
            DF = ag_def / (ag_def + en_def)
        
        MF = len(self.moves) / ag_pws
        AM = int(self.additional_move)
        MB = (0,1,0.5)[self.mobility] #check coef for -1 

        S = (EM, DF, MF, AM, MB)
        C = (10,5,3,3,1)
        RES = sum([S[i] * C[i] for i in range(5)]) / (sum(C) * 5)
        
        return (RES, EM, DF, MF, AM, MB)

    @staticmethod
    def get_bench(board):
        team1, team2 = 5, 5
        for cell in board:
            if cell == 1:
                team1 -= 1
            elif cell == 2:
                team2 -= 1
        return (team1, team2)
    
    @staticmethod
    def board_valid(b):
        if type(b) not in (list, tuple):
            return False
        if len(b) != 30:
            return False
        for c in b:
            if c not in [0, 1, 2]:
                return False
        return True
    
