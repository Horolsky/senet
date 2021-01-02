import random as r
from .state import state


class game():
    def __init__(self):
        """
        game logic manager
        """
        self.__running = False
        self.__state = None       
        self.__sticks = None
        self.__turn = None
    
    def start_game(self, firstplayer=1):
        self.__running = True
        self.__turn = 0
        self.__sticks = game.throw_sticks()
        board = [x+1 for _ in range(5) for x in range(2)] + [0 for _ in range(20)]
        self.__state = state(board, firstplayer, self.steps)
    def stop_game(self):
        self.__running = False
    def manage_movement(self, cell):  
        """
        move pawn on a given index 
        """ 
        newsticks = game.throw_sticks()
        newstate = self.state.increment(cell, game.__get_steps(newsticks))   
        if newstate is None:
            return False  
        else:
            self.__sticks = newsticks
            self.__state = newstate
            self.__turn += 1
            return True
    @property
    def state(self):
        if self.running:
            return self.__state
    @property
    def running(self):
        return self.__running
    @property
    def turn(self):
        return self.__turn
    @property
    def sticks(self):
        return self.__sticks
    @property
    def steps(self):        
        return game.__get_steps(self.sticks)
    @staticmethod
    def throw_sticks():
        return [r.randrange(2) for _ in range(4)] 
    @staticmethod
    def __get_steps(sticks):
        s = sum(sticks)
        if s == 0:
            s = 5
        return s