import random
from senet.core.enums import Unit
from senet.core.ply import Ply

class Agent:
    """
    wrapper class for human and dummy agent controls
    """
    def __init__(self, agent: Unit, dfunc=None, name="dummy"): 
        """
        dfunc: decision function
        or dummy code str ("random", "first", "last")
        """
        
        self._agent = agent
        if dfunc in ("random", "first", "last"):
            self._dfunc = {
                "random": choose_random,
                "first": choose_first,
                "last": choose_last
            }.get(dfunc)
        elif callable(dfunc):
            self._dfunc = dfunc
        else:
            self._dfunc = choose_random

        self._name = name
        #else:
        #    raise TypeError("invalid decision function")
    
    def choose_movement(self, state) -> int:
        """
        accepts a core.state object
        return an index of a choosen movement
        decision is based on initial decision_function
        """
        return self._dfunc(state)
    @property
    def brieflog_msg(self):
        return f"{self._name};;;;"
        
def choose_first(state: Ply):
    if len(state.strategies.indici) > 0:
        return state.strategies.indici[0]
    else:
        return 0

def choose_random(state):
    if len(state.strategies.indici) > 0:
        return random.choice(state.strategies.indici)
    else:
        return 0

def choose_last(state):
    if len(state.strategies.indici) > 0:
        return state.strategies.indici[-1]
    else:
        return 0