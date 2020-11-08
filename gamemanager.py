import random as r
from gamestate import GameState
throw_sticks = lambda: [r.randrange(2) for _ in range(4)] 

class Game():
    def __init__(self):
        self._running = False
        self._state = None       
         
    
    def start_game(self, firstplayer=1):
        self._running = True
        self._turn = 0
        board = [x+1 for _ in range(5) for x in range(2)] + [0 for _ in range(20)]
        
        self._sticks = throw_sticks()
        print(board)
        self._state = GameState(board, firstplayer)
        self._state.steps = self.steps
        print("game started")
    
    def manage_move(self, cell):  
        """
        move pawe on a given index and 
        """ 
        newState = self.state.move(cell)   
        if newState is None:
            return False  
        self._sticks = throw_sticks()
        newState.steps = self.steps
        self._turn += 1
        self._state = newState
        return True
    @property
    def state(self):
        if self.running:
            return self._state
    @property
    def running(self):
        return self._running
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