def is_board(b):
    if type(b) is not list:
        return False
    if len(b) != 30:
        return False
    for c in b:
        if c not in [0, 1, 2]:
            return False
    return True

class GameState():
    def __init__(self, board, turn, event = (0,0,0,0)):

        if not is_board(board):
            raise ValueError("invalid board data")
        if type(turn) is not int:
            raise TypeError("invalid turn number type")

        self._board = board
        self._turn = turn
        self._steps = None
        self._moves = None
        self._event = event 
    #
    @property
    def event(self):
        """
        returns tuple with info on game event, caused by last move
        (<eventcode>, <start>, <destination>)
        codes:
        0 - no movement
        1 - moving to empty cell
        2 - swaping
        3 - drowing and reborning
        4 - swaping on Houses with reborning
        5 - escaping 
        """
        return self._event
    @property
    def steps(self):
        return self._steps
    @steps.setter
    def steps(self, s):
        if type(s) is not int:
            raise TypeError("steps value must be int")
        elif s not in range (1, 6):
            raise ValueError("steps not in range")
        else:
            self._steps = s
            self._moves = None

    #main data getters
    @property
    def board(self):
        return self._board
    @property
    def agent(self): #current player number
        return self.turn % 2 + 1 
    @property
    def turn(self):
        return self._turn

    #dependent cached data getters
    @property
    def bench(self):
        t1, t2 = 5, 5
        for c in self.board:
            if c == 1:
                t1 -= 1
            elif c == 2:
                t2 -= 1
        return (t1, t2)
    @property
    def moves(self):
        """
        returns possible moves of the current state
        if the steps value is set
        """
        if self.steps is None:
            return None
        moves = self._moves
        if moves is not None:
            return moves
        else:
            moves = []
        b = self.board
        s = self.steps
        agent = self.agent
        for c in range(30): #c is cell index
            if b[c] != agent:
                continue
            
            d = c + s #destination index
            #cell is not under control
            if b[c] != agent: 
                continue
            #escaping cases
            if c == 29: #always escaping 
                moves.append(c)
                continue
            if c in [27,28] and d == 30: #correct escaping
                moves.append(c)     
                continue
            if d > 25 and c != 25: #incorrect house arriving
                continue
            if d > 29: #incorrect escaping
                continue
            #target occupation cases
            target = b[d] #target cell value
            enemy = agent % 2 + 1
            if target == agent: #friendly occupation
                continue
            elif target == enemy:
                if b[d + 1] == enemy or b[d - 1] == enemy: #target is defended
                    continue
            #final default case
            moves.append(c)
        self._moves = moves
        return moves               

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
    def skip(self):
        """
        return the same state with incremented turn number
        """
        return GameState(self.board, self.turn + 1)
    def move(self, cell):
        """
        move choosen paw
        returns a new state on success
        or None on failure
        """
        if self.steps is None:
            return None
        elif cell not in self.moves:
            return None
        steps = self.steps
        destination = cell + steps #target
        board = self.board.copy()
        agent = self.agent
        enemy = agent % 2 + 1 # enemy
        event = (0,0,0,0)
        #drowing in House of Water
        if destination == 26: 
            board[cell] = 0
            for i in range(14, -1, -1):
                if board[i] == 0:
                    board[i] = agent
                    event = (3, cell, destination, i)
                    break
        #escaping
        elif cell == 29: 
            board[29] = 0
            event = (6, 29, destination, -1)
        elif destination == 30 and cell == (30 - steps):
            board[cell] = 0
            event = (6, cell, destination, -1)
        #attacking
        elif board[destination] == enemy:
            if board[destination-1] != enemy and board[destination+1] != enemy:
                board[destination] = agent                
                if destination > 26: #attacking Houses rule
                    for i in range(14, -1, -1):
                        if board[i] == 0:
                            board[i] = enemy
                            board[cell] = 0
                            event = (4, cell, destination, i)
                            break
                else:
                    board[cell] = enemy
                    event = (2, cell, destination, cell)
        #moving to empty target
        elif board[destination] == 0:
            board[destination] = agent
            board[cell] = 0
            event = (1, cell, destination, -1)
        else:
            raise ValueError(f"incorrect move logic, start: {cell}")
        return GameState(board, self.turn + 1, event)
        
         