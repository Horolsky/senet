import random as r
from gamestate import GameState
throw_sticks = lambda: [r.randrange(2) for _ in range(4)] 

class Game():
    def __init__(self):
        self._running = False
    
    def start_game(self, player=1):
        self._player = player
        self._running = True
        self._turn = 0
        board = [x+1 for _ in range(5) for x in range(2)] + [0 for _ in range(20)]
        
        self._sticks = throw_sticks()
        print(board)
        self._state = GameState(board, 0)
        self._state.steps = self.steps

        print("game started")
    
    def make_move(self, cell):        
        newState = self.state.move(cell)
        if newState is None:
            if len(self.state.moves) == 0:
                newState = self.state.skip()
            else:
                return False
        
        self._sticks = throw_sticks()
        newState.steps = self.steps
        self._state = newState
        return True
    @property
    def state(self):
        if self.running:
            return self._state

    @property
    def curr_team(self):
        return self.state.agent
    @property
    def running(self):
        return self._running
    @property
    def player(self):
        return self._player
    @property
    def turn(self):
        return self._turn
    @property
    def sticks(self):
        return self._sticks
    @property
    def steps(self):        
        s = sum(self.sticks)
        if s == 0:
            s = 5
        return s
    @property
    def status(self):
        s = {
            "agent": self.state.agent,
            "turn": self.state.turn,
            "steps": self.state.steps,
            "bench": self.state.bench,
            "event": self.state.event
        }
        return s