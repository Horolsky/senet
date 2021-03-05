from random import choice
from time import perf_counter_ns
from senet.utils.report import Report
from senet.settings import SETTINGS
from senet.xtc import emax, INCREMENT_RULES, EVALUATION_FUNCS

class AIplayer():
    def __init__(self, number, depth, rules="Kendal", eval_func="basic", coefs=(1,0,0,0)):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")
        if depth < 0:
            raise ValueError("negative depth arg")
        if rules not in INCREMENT_RULES:
            raise ValueError("invalid rules arg")
        if eval_func not in EVALUATION_FUNCS:
            raise ValueError("inalid eval arg")
            
        
        self.__log = SETTINGS.get("logs/ai")
        self._agent = number
        self._name = "AI"
        self._timer = SETTINGS.get("game/timer")
        self._depth = depth

        self._eval_func = eval_func
        self._coefs = coefs
        self._rules = rules
        if self.__log:
            header = f"seed;t, ns;t, ~sec;leaves searched;agent: {self._agent}, timer: {self._timer}, depth: {self._depth}, eval: {self._eval_func}, coefs: {self._coefs}\n"
            self._report = Report(f"ai-{number}", None, "csv", "logs/ai", header)

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
        elif self._depth < 1:
            return choice(state.moves)

        timeA = perf_counter_ns()
        res = emax(state=state.seed, depth=self._depth, sec=self._timer, coefs=self._coefs, incr_func=self._rules, eval_func=self._eval_func)
        timeB = perf_counter_ns()
        decision = res[0]
        leaves = res[1]
        
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};{leaves};\n")
            
        return state.moves[decision]
