def is__board(b):
    if type(b) is not list:
        return False
    if len(b) != 30:
        return False
    for c in b:
        if c not in [0, 1, 2]:
            return False
    return True

class GameState():
    def __init__(self, board, agent, steps, event = None):
        if not is__board(board):
            raise ValueError("invalid board data")
        self.__board = board
        if agent not in [1,2]:
            raise ValueError("invalid agent number")
        self.__agent = agent
        self.__enemy = agent % 2 + 1
        
        if event is None:
            event = (agent, 0,0,0,0)
        else:
            if type(event) is not tuple:
                raise TypeError("invalid event data")
            if len(event) != 5:
                raise ValueError("invalid event data")
        self.__event = event
        if steps not in [1,2,3,4,5]:
            raise ValueError("invalid steps value")
        self.__steps = steps
        self.__am = steps in [1,4,5] #additional move
        self.__bench = GameState.get_bench(board)
        #cached data
        self.__moves = None
        self.__mobility = None
        self.__utility = None
    #main data getters
    @property
    def board(self):
        return self.__board.copy()
    @property
    def agent(self): #current player number
        return self.__agent
    @property
    def enemy(self):
        return self.__enemy
    @property
    def bench(self):
        return self.__bench
    @property
    def steps(self):
        return self.__steps
    @property
    def additional_move(self):
        return self.__am
    @property
    def event(self):
        """
        tuple with info on game event, caused by last move
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
        if self.__moves is None:
            self.__set_cached_data()
        return self.__moves.copy()
    @property
    def mobility(self):
        if self.__mobility is None:
            self.__set_cached_data()
        return self.__mobility
    def increment(self, cell, newsteps):
        """
        move choosen pawn
        returns a new state on success or None on failure
        if no possible moves, returns the same state with swaped agent
        """
        next_player = (self.enemy, self.__agent)[self.__am]
        # skipping move
        if self.__mobility == 0:
            return GameState(self.__board, next_player, newsteps, self.__event)
        # incorrect cell index
        if cell not in self.__moves:
            return None
        # movement cases
        steps = self.__steps
        destination = cell + steps #target
        board = self.__board
        agent = self.__agent
        enemy = self.enemy
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
            if board[destination-1] != enemy and board[destination+1] != enemy:
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
                    event = (agent, 2, cell, destination, cell)
        #moving to empty target
        elif board[destination] == 0:
            board[destination] = agent
            board[cell] = 0
            event = (agent, 2, cell, destination, -1)
        else:
            raise ValueError(f"incorrect move logic, start: {cell}")
        return GameState(board, next_player, newsteps, event)

    def __set_cached_data(self):
        mobility = 1 # 1 for normal movement, -1 for reverse, 0 for skipping
        no_direct_moves, no_reverse_moves = False, False
        moves = GameState.get_moves(self.__board, self.__agent, self.__steps)
        no_direct_moves = len(moves) == 0
        if no_direct_moves:
            mobility = -1
            moves = GameState.get_moves(self.__board, self.__agent, -1)
            no_reverse_moves = len(moves) == 0
            if no_reverse_moves:
                mobility = 0
        self.__moves = moves
        self.__mobility = mobility

    #utility func for minimax
    @property
    def utility(self):
        """
        returns maximin for current agent
        - approximation of total of estimated moves
        - n of possible moves
        - n of defenced groups
        - positions 22-23 occupation
        """
        #friends, enemies = [], []
        f_m, e_m = 0, 0 # total estimated moves to escape
        b = self.board
        for c in range(30):
            if b[c] == 0:
                continue
            m = (30 - c) // 3 # estimated moves to escape
            #(lambda: enemies.append(c), lambda: friends.append(c))[b[c] == self.agent]()
            if b[c] == self.agent:
                f_m += m
            else:
                e_m += m
            #(incr_e, incr_f)[b[c] == self.agent](m)

        #avg_f = sum(friends) / len(friends)
        #avg_e = sum(enemies) / len(enemies)
        return (f_m, e_m)

    @staticmethod
    def get_moves(board, agent, steps):
        """
        returns list of possible moves 
        """
        if steps not in [-1,1,2,3,4,5] or agent not in [1,2] or not is__board(board):
            raise ValueError("corrupted data")
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
            elif target == enemy: #defended target
                if board[destination - 1] == enemy:
                    continue
                if destination < 29:
                    if board[destination + 1] == enemy:
                        continue
            #final default case
            moves.append(cell)
        return moves  
    
    @staticmethod
    def get_bench(board):
        team1, team2 = 5, 5
        for cell in board:
            if cell == 1:
                team1 -= 1
            elif cell == 2:
                team2 -= 1
        return (team1, team2)