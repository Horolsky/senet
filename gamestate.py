def is_board(b):
    if type(b) is not list:
        return False
    if len(b) != 30:
        return False
    for c in b:
        if c not in [0, 1, 2]:
            return False
    return True
def throw_sticks():
    return [r.randrange(2) for _ in range(4)]
class GameState():
    def __init__(self, board, turn):

        if not is_board(board):
            raise ValueError("invalid board data")
        #if type(steps) is not int:
        #    raise ValueError("invalid steps number")
        if type(turn) is not int:
            raise ValueError("invalid turn number")

        self._board = board
        #self._steps = steps
        self._steps = None
        self._turn = turn
        self._moves = None
        self._team = None
    #
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
    def team(self):
        """
        returns indices of the current team
        """
        team = self._team
        if team is not None:
            return team
        else: 
            team = []
        for c in range(30):
            if self.board[c] == self.agent:
                team.append(c)
        self._team = team
        return team
    @property
    def enemies(self):
        """
        returns indices of the current team
        """
        enemies = self._enemies
        if enemies is not None:
            return enemies
        for c in range(30):
            if self.board[c] == self.agent % 2 + 1:
                enemies.append[c]
        self._enemies = enemies
        return enemies
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
        for c in self.team: #c is cell index
            b = self.board
            s = self.steps
            agent = self.agent
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
            if d > 25 and c != 24: #incorrect house arriving
                continue
            if d > 30: #incorrect escaping
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

    def move(self, c):
        """
        move choosen paw
        returns new state on success
        or None on failure
        """
        if self.steps is None:
            return None
        elif c not in self.moves:
            return None
        s = self.steps
        t = c + s #target
        b = self.board.copy()
        a = self.agent
        e = a % 2 + 1 # enemy
        #drowing in House of Water
        if t == 26: 
            b[c] = 0
            b[14] = a
        #escaping
        elif c == 29: 
            b[29] = 0
        elif t == 30 and c == (30 - s):
            b[c] = 0
        #attacking
        elif b[t] == e:
            if b[t-1] != e and b[t+1] != e:
                b[t] = a
                b[c] = e
        #moving to empty target
        elif b[t] == 0:
            b[t] = a
            b[c] = 0
        else:
            raise ValueError(f"incorrect move logic, start: {c}")
        return GameState(b, self.turn + 1)
        
         