import random as r
class Game():
    is_running = False
    sticks = [0, 0, 0, 0]
    def __init__(self):
        self._running = False
    
    def start_game(self, player=1):
        self._player = player
        self._running = True
        self._turn = 0
        self._table = [x+1 for _ in range(5) for x in range(2)] + [0 for _ in range(20)]
        self._sticks = [r.randrange(2) for _ in range(4)]

        print("game started")
    
    def make_move(self, cell):
        self.curr_team = self.turn % 2 + 1
        target = cell + self.steps
        if self.table[cell] != current_player:
            return False
        if self.table[target] != 0:
            return False
        #next turn
        self._sticks = [r.randrange(2) for _ in range(4)]
        self._turn += 1
        return True

    @property
    def curr_team(self):
        return self.turn % 2 + 1
    @property
    def table(self):
        return self._table
    @property
    def running(self):
        return self._running
    @property
    def player(self):
        return self._player
    @property
    def sticks(self):
        return self._sticks
    @property
    def turn(self):
        return self._turn
    @property
    def steps(self):        
        s = sum(self.sticks)
        if s == 0:
            s = 5
        return s

    @property
    def status(self):
        s = f"""
        Game status:
        current team:\t{self.curr_team},
        turn:\t\t{self._turn},
        steps:\t\t{self.steps}
        """
        return s