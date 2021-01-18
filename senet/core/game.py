import random as r
from .xply import Ply
from .agent import agent
from senet.utils import report
from senet.settings import SETTINGS
from json import dumps


class game():
    @staticmethod
    def check_agent(agent):
        for attr in ("choose_movement", "_agent", "_name"):
            if not hasattr(agent, attr):
                return False  
        return True
    @staticmethod
    def check_seed(seed):
        if type(seed) is not int:
            return False
        test = Ply(seed)
        res = True
        if test.agent not in (1,2):
            res = False
        if test.steps not in [1,2,3,4,5]:
            res = False
        if test.utility in (0, 1):
            res = False
        return res
        
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
    
    def __stop(self): #after loop game closing
        self.__running = False
        if self.__log:
            self._report.write("\ngame over")
            self._report.close()
        
    def stop(self): #in loop game stop
        self.__running = False
        return self.__state.seed
    
    def start(self, agent1, agent2,  first=1, seed=10066320): #start_game
        """
        start new or restart current game
        @param first: int
        @param agent1: agent
        @param agent2: agent
        """
        self.__log = SETTINGS.get("dev/gamelogs")


        #agents duck typing
        if not game.check_agent(agent1) or not game.check_agent(agent2):
            raise TypeError("invalid agent objects") 
        self.__agent1 = agent1
        self.__agent2 = agent2
        self.__running = True
        self.__turn = 0
        
        self.__state = Ply(seed)
        if seed == 10066320:
            self.__sticks = game.throw_sticks()
            self.__state.steps = game.get_steps(self.__sticks)
            self.__state.agent = first
        
        if self.__log:
            headers = f"N;{agent1._name} vs {agent2._name};agent;steps;utility;seed\n"
            self._report = report("game", "csv", "logs/games", headers)
        self.__onmove()
        self.__run()
        self.__stop()
    
    def __run(self):
        if self.__log:
            self._report.write(f"{self.__turn};{self.state.to_csv()}")
            
        while self.__running:
            self.__running = self.__move()
            if not self.__running:
                break
            self.__onmove()
            if self.__log:
                self._report.write(f"\n{self.__turn};{self.state.to_csv()}")
            #END GAME CONDITION
            if 5 in self.state.bench:
                self.__onvictory(self.state.event[1]) #sending agent n to callback
                break
                


    def __move(self):  #manage_movement
        """
        a new move is requested from the current agent,
        on which the game state is incremented
        returns True on success,
        False on failure
        """ 
        cell = self.agent.choose_movement(self.state)
        if self.__running == False:
            return False
        newsticks = game.throw_sticks()
        newstate = self.state.increment(cell)
        newstate.steps = game.get_steps(newsticks) 
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