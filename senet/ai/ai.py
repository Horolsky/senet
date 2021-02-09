import threading
import time
import ctypes
from math import inf
from senet.utils.report import report
from time import perf_counter_ns
from senet.settings import SETTINGS
from senet.xtc import emax_timedbrute

class AIplayer():
    def __init__(self, number, depth):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")

        self.__log = SETTINGS.get("dev/ailogs")
        self._agent = number
        self._name = "AI"
        self._tree = []
        self._decision = 0
        self._turn = 0
        self.stopFlag = False
        self._timer = SETTINGS.get("ai/timer")
        self._depth = depth
        if self.__log:
            header = f"seed;t, ns;t, ~sec;leaves searched;agent: {self._agent}, timer: {self._timer}, depth: {self._depth}\n"
            self._report = report(f"ai-{number}", "csv", "logs/ai", header)
        self._leaves = 0

    def __del__(self):
        if self.__log:
            self._report.close()
        
    def choose_movement(self, state):
        #TODO: check state
        if state.agent != self._agent:
            raise ReferenceError("wrong agent call")
        self._state = state

        #some correct value is guaranteed
        self._decision = 0
        self._util = (-inf, inf)[self._agent - 1]
        if len(state.moves) > 0:
            self._decision = state.moves[-1:][0]

        self._leaves = 0
        timeA = perf_counter_ns()
        self.think()
        timeB = perf_counter_ns()
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};{self._leaves};\n")
        return self._decision    
    def think(self):
        """
        tree traversing
        assigning best value to self._decision 
        """
        if len(self._state.moves) < 2:
            return

        time = self._timer // len(self._state.moves)
        if time == 0:
            time == 1
        #get util for guaranteed move first  
        res = emax_timedbrute(self._state.increment(self._decision).seed, self._depth, time)
        
        self._util = res[0]
        self._leaves += res[1]

        for move in self._state.moves[:-1]:
            res = emax_timedbrute(self._state.increment(move).seed, self._depth, time)
            
            self._leaves += res[1]
            if (self._agent == 1 and res[0] > self._util) or (self._agent == 2 and res[0] < self._util):
                self._util = res[0]
                self._decision = move    
