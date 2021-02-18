import threading
import time
import ctypes
from math import inf
from senet.utils.report import report
from time import perf_counter_ns
from senet.settings import SETTINGS
from senet.xtc import emax, INCREMENT_RULES, EVALUATION_FUNCS

class AIplayer():
    def __init__(self, number, depth, rules="Kendal", eval_func="basic"):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")
        if depth < 0:
            raise ValueError("negative depth arg")
        if rules not in INCREMENT_RULES:
            raise ValueError("invalid rules arg")
        if eval_func not in EVALUATION_FUNCS:
            raise ValueError("inalid eval arg")
            

        self.__log = SETTINGS.get("dev/ailogs")
        self._agent = number
        self._name = "AI"
        self._timer = SETTINGS.get("ai/timer")
        self._depth = depth

        self._eval_func = eval_func
        self._rules = rules
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
        if len(state.moves) == 0:
            return 0
        elif len(state.moves) == 1:
            return state.moves[0]

        timeA = perf_counter_ns()
        res = emax(state.seed, self._depth, self._timer, self._rules, self._eval_func)
        timeB = perf_counter_ns()
        decision = res[0]
        leaves = res[1]
        
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};{leaves};\n")
            
        return state.moves[decision]
