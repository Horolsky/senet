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
    def __init__(self, board, agent, event = (0,0,0,0)):

        if not is_board(board):
            raise ValueError("invalid board data")
        if agent not in [1,2]:
            raise ValueError("invalid agent number")
        if type(event) is not tuple:
            raise TypeError("invalid event data")
        if len(event) != 4:
            raise ValueError("invalid event data")
        self._agent = agent
        self._board = board
        self._event = event
        self._steps = None
        self._moves = None
        self._reverse_moves = None
        
    @property
    def event(self):
        """
        returns tuple with info on game event, caused by last move
        (<eventcode>, <start>, <destination>)
        codes:
        0 - skip movement
        1 - reverse movement
        2 - moving to empty cell
        3 - swaping
        4 - drowing and reborning
        5 - swaping on Houses with reborning
        6 - escaping 
        """
        return self._event
    @property
    def steps(self):
        return self._steps
    @steps.setter
    def steps(self, s):
        if type(s) is not int:
            raise TypeError("steps value must be int")
        elif s not in [-1,1,2,3,4,5]:
            raise ValueError("steps not in range")
        else:
            self._steps = s

    #main data getters
    @property
    def board(self):
        return self._board
    @property
    def agent(self): #current player number
        return self._agent
    @agent.setter
    def sgent(self, newAg):
        if newAg not in [1,2]:
            raise ValueError("invalid agent number")
        self._agent = newAg
    @property
    def enemy(self):
        return self.agent % 2 + 1

    #dependent cached data getters
    @property
    def bench(self):
        team1, team2 = 5, 5
        for cell in self.board:
            if cell == 1:
                team1 -= 1
            elif cell == 2:
                team2 -= 1
        return (team1, team2)
    @property
    def moves(self):
        """
        returns possible moves of the current state
        if the steps value is set
        """
        if self.steps is None:
            return None
        #cache
        moves = self._moves
        if moves is not None:
            return moves
        else:
            moves = []
        #calculating
        board = self.board
        steps = self.steps
        agent = self.agent
        enemy = self.enemy
        for cell in range(30): #c is cell index
            #filling array by exclusion:
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
            elif target == enemy:
                if board[destination + 1] == enemy or board[destination - 1] == enemy: #target is defended
                    continue
            #final default case
            moves.append(cell)
        
        #reverse moves
        if len(moves) == 0:
            self.steps = -1
            for cell in range(28):
                if board[cell] == 0 and board[cell + 1] == agent:
                    moves.append(cell+1)
        self._moves = moves
        return moves               
    def move(self, cell):
        """
        move choosen paw
        returns a new state on success or None on failure
        if no possible moves, returns the same state with swaped agent
        """
        if self.steps is None:
            return None
        elif self.steps > 0 and cell not in self.moves:
            return None
        steps = self.steps
        destination = cell + steps #target
        board = self.board.copy()
        agent = self.agent
        enemy = self.enemy
        event = (0,0,0,0) #default event - skipping move
        #reverse
        if steps == -1:
            if cell in self.moves:
                if cell == 27: #reverse to House of Waters
                    board[cell] = 0
                    for i in range(14, -1, -1):
                        if board[i] == 0:
                            board[i] = agent
                            event = (1, cell, i, -1)
                            break
                else:
                    board[cell] = 0
                    board[cell-1] = agent 
                    event = (1, cell, cell-1, -1)          
        #drowing in House of Water
        elif destination == 26: 
            board[cell] = 0
            for i in range(14, -1, -1):
                if board[i] == 0:
                    board[i] = agent
                    event = (4, cell, destination, i)
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
                            event = (5, cell, destination, i)
                            break
                else:
                    board[cell] = enemy
                    event = (2, cell, destination, cell)
        #moving to empty target
        elif board[destination] == 0:
            board[destination] = agent
            board[cell] = 0
            event = (2, cell, destination, -1)
        else:
            raise ValueError(f"incorrect move logic, start: {cell}")
        return GameState(board, self.enemy, event)
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
    