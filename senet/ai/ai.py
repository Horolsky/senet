from array import array
from random import choice
from senet.core.agent import Agent
from time import perf_counter_ns
from senet.utils.report import Report
from senet.settings import SETTINGS
from senet.core import Ply
from senet.core.xtc import Emax, Eval
from senet.core.enums import EvalFunc, EmaxAlgo, Unit

EVALUATION_FUNCS = {
    "basic": EvalFunc.BASIC,
    "linear": EvalFunc.LINEAR,
    "polynomial": EvalFunc.POLYNOMIAL
}
EMAX_ALGOS = {
    "recursive": EmaxAlgo.RECURSIVE,
    "iterative": EmaxAlgo.ITERATIVE
}

class AIplayer(Agent):
    def __init__(self, agent: Unit, depth, eval_func="basic", emax_algo="recursive", coefs=(1,0,0,0)):
        
        if depth < 0:
            raise ValueError("negative depth arg")
        if eval_func.lower() not in EVALUATION_FUNCS:
            raise ValueError("inalid eval arg")
        if emax_algo.lower() not in EMAX_ALGOS:
            raise ValueError("inalid eval arg")
        if len(coefs) != 4:
            raise AttributeError("coefs size is exactly 4")
        
        self.__log = SETTINGS.get("logs/ai")
        self._agent = agent
        self._name = "AI"
        self._timer = SETTINGS.get("game/timer")
        self._depth = depth

        self._eval_func = EVALUATION_FUNCS[eval_func]
        self._emax_algo = EMAX_ALGOS[emax_algo]
        self._coefs = array('i',coefs)
        self.__emax = Emax(Eval(self._eval_func.value, self._coefs), self._emax_algo.value, depth, self._timer*1000)
        if self.__log:
            header = f"seed;t, ns;t, ~sec;agent: {agent.name}, timer: {self._timer}, depth: {depth}, eval: {self._eval_func}, coefs: {self._coefs}\n"
            self._report = Report(f"ai-{agent.name}", None, "csv", "logs/ai", header)

    def __del__(self):
        if self.__log:
            self._report.close()
        
    def choose_movement(self, state: Ply) ->int:
        
        if state.agent != self._agent:
            raise ReferenceError("wrong agent call")
        if len(state.strategies.indici) == 0:
            return 0
        elif len(state.strategies.indici) == 1:
            return state.strategies.indici[0]
        elif self._depth < 1:
            return choice(state.strategies.indici)

        timeA = perf_counter_ns()
        res = self.__emax(state.state)
        timeB = perf_counter_ns()
        decision = res
        # leaves = res[1]
        
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};\n")
            
        return state.strategies.indici[decision]

    @property
    def brieflog_msg(self):
        return f"AI;{self._depth};{self._eval_func.name};{list(self._coefs)};"
