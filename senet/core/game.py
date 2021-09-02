import random as r
from time import perf_counter
from senet.ai.ai import AIplayer
from typing import Tuple, Union
from senet.core.enums import Unit
from senet.core.xtc import State as _State
from senet.core import Ply, Dice
from .agent import Agent
from senet.utils import Report
from senet.settings import SETTINGS
from json import dumps
from datetime import datetime

BRIEFLOG_HEADERS = [
    "duration",
    "timer",
    "first move",
    "agent 1",
    "depth 1",
    "eval 1",
    "coefs 1",
    "agent 2",
    "depth 2",
    "eval 2",
    "coefs 2",
    "winner",
    "score"
    ]

class Game():
    
    @staticmethod
    def check_seed(seed):
        if type(seed) is not int:
            return False
        try:
            test = Ply(seed=seed)
            res = True
            if test.expectation in (0, 1):
                res = False
            if set(test.board) != {Unit.X, Unit.Y, Unit.NONE}:
                res = False
            if sum([x for x in test.board if x == Unit.NONE]) < 20:
                res = False
            return res
        except:
            return False
        
    def __init__(self, onmove, onvictory, prefix="senet_games"):
        """
        game logic manager
        """
        self.__running = False
        self.__ply = None       
        self.__sticks = None
        self.__turn = None
        if not callable(onmove):
            onmove = lambda: None
        if not callable(onvictory):
            onvictory = lambda v: None
        self.__onmove = onmove
        self.__onvictory = onvictory
        self.__logging_brief = SETTINGS.get("logs/brief")
        self.__game_starttime = None
        self.__dice = Dice()
        if self.__logging_brief:
            self._brieflog = Report(prefix, "csv", "logs/brief", ';'.join(BRIEFLOG_HEADERS)+"\n", False)
        else:
            self._brieflog = None
    
    def __stop(self): #after loop game closing
        self.__running = False
        if self.__logging_game:
            self._gamelog.close("\ngame over")
        self.__agentX = None
        self.__agentY = None
        
    def stop(self): #in loop game stop
        self.__running = False
        if self.__ply:
            return self.__ply.seed
    
    def start(self, agentX: Agent, agentY: Agent, first: Unit, seed=0): #start_game
        """
        start new or restart current game
        @param first: Unit
        @param agentX: Agent
        @param agentY: Agent
        """
        self.__logging_game = False #SETTINGS.get("logs/game")
        self.__logging_brief = SETTINGS.get("logs/brief")
        #TODO: fix logging

        self.__agentX = agentX
        self.__agentY = agentY
        self.__running = True
        self.__turn = 0
        
        
        if seed == 0:
            chance = self.__dice.roll()
            self.__sticks = tuple(self.__dice.sticks)    
            self.__ply = Ply(chance=chance, agent=first)
        else:
            s = _State(seed)
            chance = s.steps()
            if chance == 0:
                chance = self.__dice.roll()
                self.__sticks = tuple(self.__dice.sticks)  
                self.__ply = Ply(state=s,chance=chance, agent=first)
            else:
                self.__ply = Ply(seed=seed)
            
        if self.__logging_game:
            headers = f"N;{agentX._name} vs {agentY._name}: agent;steps;utility;seed\n"
            self._gamelog = Report("game", "csv", "logs/games", headers)
        self.__game_starttime = perf_counter()
        self.__onmove()
        self.__run()
        self.__stop()
    
    def __run(self):
        if self.__logging_game:
            self._gamelog.write(f"{self.__turn};{self.ply.to_csv()}")
            
        while self.__running:
            self.__running = self.__move()
            if not self.__running:
                break
            self.__onmove()
            if self.__logging_game:
                self._gamelog.write(f"\n{self.__turn};{self.ply.to_csv()}")
            #END GAME CONDITION
            if self.ply.state.is_terminal_node():
                self.__onvictory(self.ply.event.agent) #sending agent n to callback
                if self.__logging_brief:
                    self.__record_to_brieflog()
                break
                
    def __record_to_brieflog(self):
        if self._brieflog == None:
            self._brieflog = Report("senet_log", "csv", "logs/brief", ';'.join(BRIEFLOG_HEADERS)+"\n", False)

        winner = self.ply.event.agent
        
        
        score = 0 
        for cell in self.ply.board:
            if cell != Unit.NONE:
                score += 1
        timer = SETTINGS.get("game/timer")
        first = SETTINGS.get("game/first")
        msg = f"{(perf_counter() - self.__game_starttime)};{timer};{first};"
        msg += self.__agentX.brieflog_msg
        msg += self.__agentY.brieflog_msg
        msg += f"{winner.name};{score}\n"

        self._brieflog.write(msg)

    def __move(self):  #manage_movement
        """
        a new move is requested from the current agent,
        on which the game state is incremented
        returns True on success,
        False on failure
        """ 
        choice = self.agent.choose_movement(self.ply)
        if self.__running == False:
            return False
        if choice not in self.ply.strategies.indici:
            return False
        chance = self.__dice.roll()
        self.__sticks = tuple(self.__dice.sticks)
        newply = self.ply.increment(choice, chance)
        self.__ply = newply
        self.__turn += 1
        return True    
    
    @property
    def agent(self):
        """
        an instance of the core.agent class, holding a callback for a movement request
        represents the current agent (player)
        """
        return (self.__agentX, self.__agentY)[int(self.ply.agent.value)]
    @property
    def enemy(self):
        """
        an instance of the core.agent class, holding a callback for a movement request
        represents an enemy of the current agent (player)
        """
        return (self.__agentX, self.__agentY)[int(self.ply.enemy.value)]
    
    @property
    def ply(self):
        """
        an instance of the core.state class
        """
        if self.running:
            return self.__ply
    
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
    def get_steps(sticks):
        """
        @param sticks: tuple
        """
        s = sum(sticks)
        if s == 0:
            s = 5
        return s