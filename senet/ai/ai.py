import threading
import time
import ctypes
from math import inf
from senet.utils.report import report
from time import perf_counter_ns
from senet.settings import SETTINGS
from senet.xtc import emax

class AIplayer():
    def __init__(self, number, depth):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")

        self.__log = SETTINGS.get("dev/ailogs")
        self._agent = number
        self._name = "AI"
        self._timer = SETTINGS.get("ai/timer")
        self._depth = depth
        if self.__log:
            header = f"seed;t, ns;t, ~sec;leaves searched;agent: {self._agent}, timer: {self._timer}, depth: {self._depth}\n"
            self._report = report(f"ai-{number}", "csv", "logs/ai", header)

    def __del__(self):
        if self.__log:
            self._report.close()
        
    def choose_movement(self, state):
        #TODO: check state
        if state.agent != self._agent:
            raise ReferenceError("wrong agent call")

        timeA = perf_counter_ns()
        res = emax(state.seed, self._depth, self._timer)
        timeB = perf_counter_ns()
        decision = res[0]
        leaves = res[1]
        
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};{leaves};\n")

        if len(state.moves) == 0:
            return 0
        else:
            return state.moves[decision]    
        #NB: ai works also on x0 and x1 strategies just for lulz
        #it could be useful for memo optimisation, if such would be imlemented

