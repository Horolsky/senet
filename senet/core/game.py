import random as r
from senet.xtc import Ply
from .agent import Agent
from senet.utils import report
from senet.settings import SETTINGS
from json import dumps
from datetime import datetime

class Game():
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
        self.__logging_brief = SETTINGS.get("dev/brieflog")
        if self.__logging_brief:
            headers = f"end time;agent 1;agent 2;winner;score;\n"
            self._brieflog = report("senet_log", "csv", "logs/brief", headers, False)
        else:
            self._brieflog = None
    
    def __stop(self): #after loop game closing
        self.__running = False
        if self.__logging_game:
            self._gamelog.close("\ngame over")
        self.__agent1 = None
        self.__agent2 = None
        
    def stop(self): #in loop game stop
        self.__running = False
        return self.__state.seed
    
    def start(self, agent1, agent2,  first=1, seed=10066320): #start_game
        """
        start new or restart current game
        @param first: int
        @param agent1: Agent
        @param agent2: Agent
        """
        self.__logging_game = SETTINGS.get("dev/gamelogs")
        self.__logging_brief = SETTINGS.get("dev/brieflog")


        #agents duck typing
        if not Game.check_agent(agent1) or not Game.check_agent(agent2):
            raise TypeError("invalid agent objects") 
        self.__agent1 = agent1
        self.__agent2 = agent2
        self.__running = True
        self.__turn = 0
        
        self.__state = Ply(seed)
        if seed == 10066320:
            self.__sticks = Game.throw_sticks()
            self.__state.steps = Game.get_steps(self.__sticks)
            self.__state.agent = first
        
        if self.__logging_game:
            headers = f"N;{agent1._name} vs {agent2._name};agent;steps;utility;seed\n"
            self._gamelog = report("game", "csv", "logs/games", headers)
        self.__onmove()
        self.__run()
        self.__stop()
    
    def __run(self):
        if self.__logging_game:
            self._gamelog.write(f"{self.__turn};{self.state.to_csv()}")
            
        while self.__running:
            self.__running = self.__move()
            if not self.__running:
                break
            self.__onmove()
            if self.__logging_game:
                self._gamelog.write(f"\n{self.__turn};{self.state.to_csv()}")
            #END GAME CONDITION
            if 5 in self.state.bench:
                self.__onvictory(self.state.event[1]) #sending agent n to callback
                if self.__logging_brief:
                    self.__record_to_brieflog()
                break
                
    def __record_to_brieflog(self):
        if self._brieflog == None:
            headers = f"end time;agent 1;agent 2;winner;score;\n"
            self._brieflog = report("senet_log", "csv", "logs/brief", headers, False)

        winner = self.state.event[1]
        looser = winner % 2 + 1
        score = sum(self.state.board) / looser
        agents = [self.__agent1._name, self.__agent2._name]

        if agents[0].lower() == "ai":
            agents[0] += f"-{str(self.__agent1._depth)}"
        elif agents[1].lower() == "ai":
            agents[1] += f"-{str(self.__agent2._depth)}"
        self._brieflog.write(f"{str(datetime.now())};{agents[0]};{agents[1]};{winner};{score}\n")

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
        newsticks = Game.throw_sticks()
        newstate = self.state.increment(cell)
        if 3 in newstate.board:
            print("erreo: " + str(newstate.seed))
        newstate.steps = Game.get_steps(newsticks) 
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