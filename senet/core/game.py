import random as r
from .state import state
from .agent import agent
from senet.report import report

class game():
    def __init__(self, onmove, onvictory):
        """
        game logic manager
        """
        self.__running = False
        self.__state = None       
        self.__sticks = None
        self.__turn = None
        self.__onmove = onmove
        self.__onvictory = onvictory
    
    def stop(self): #stop_game
        self.__running = False
    
    def start(self, agent1, agent2,  first=1, log=False): #start_game
        """
        start new or restart current game
        @param first: int
        @param agent1: agent
        @param agent2: agent
        """
        self.__log = log
        
        self.__agent1 = agent1
        self.__agent2 = agent2
        self.__running = True
        self.__turn = 0
        self.__sticks = game.throw_sticks()
        board = [x+1 for _ in range(5) for x in range(2)] + [0 for _ in range(20)]
        self.__state = state(board, first, self.steps)
        if log:
            self._report = report("game", "json", "logs/games")
        self.__onmove()
        self.__run()
    
    def __run(self):
        if self.__log:
            self._report.write(f'\n"{self.__turn}":' + self.state.to_json())
            
        while self.__running:
            if self.__log:
                self._report.write(f',\n"{self.__turn}":' + self.state.to_json())
            self.__running = self.__move()
            self.__onmove()
            #END GAME CONDITION
            if 5 in self.state.bench:
                self.__onvictory(self.state.event[0]) #sending agent n to callback
                self.stop()
                if self.__log:
                    self._report.write("\n}")
                    self._report.close()

    def __move(self):  #manage_movement
        """
        a new move is requested from the current agent,
        on which the game state is incremented
        returns True on success,
        False on failure
        """ 
        cell = 0
        if self.state.mobility != 0:
            cell = self.agent.choose_movement(self.state)
        newsticks = game.throw_sticks()
        newstate = self.state.increment(cell, game.get_steps(newsticks)) 
        if newstate is None:
            return False  
        else:
            self.__sticks = newsticks
            self.__state = newstate
            self.__turn += 1
            return True
    
    @property
    def agent(self):
        """
        an instance of the core.agent class, holding a callback for a movement request
        represents the current agent (player)
        """
        return (self.__agent1, self.__agent2)[self.state.agent-1]
    
    def enemy(self):
        """
        an instance of the core.agent class, holding a callback for a movement request
        represents an enemy of the current agent (player)
        """
        return (self.__agent1, self.__agent2)[self.state.enemy-1]
    
    @property
    def state(self):
        """
        an instance of the core.state class
        """
        if self.running:
            return self.__state
    
    @property
    def running(self):
        return self.__running

    @property
    def turn(self):
        """
        number of game iteration
        """
        return self.__turn

    @property
    def sticks(self):
        return self.__sticks

    @property
    def steps(self):        
        return game.get_steps(self.sticks)

    @staticmethod
    def throw_sticks():
        """
        return a 4x binary tuple, representing random senet sticks 
        """
        return tuple([r.randrange(2) for _ in range(4)])
    
    @staticmethod
    def get_steps(sticks):
        """
        @param sticks: tuple
        """
        s = sum(sticks)
        if s == 0:
            s = 5
        return s