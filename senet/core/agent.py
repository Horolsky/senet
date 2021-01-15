import random

class agent:
    """
    wrapper class for human and dummy agent controls
    """
    def __init__(self, number, dfunc=None, name="dummy"): 
        """
        dfunc: decision function
        or dummy code str ("random", "first", "last")
        """
        if number not in [1,2]:
            raise ValueError("invalid number arg")
        self._number = number
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
    
    def choose_movement(self, state):
        """
        accepts a core.state object
        return an index of a choosen movement
        decision is based on initial decision_function
        """
        return self._dfunc(state)

def choose_first(state):
    if len(state.moves) > 0:
        return state.moves[0]
    else:
        return 0

def choose_random(state):
    if len(state.moves) > 0:
        return random.choice(state.moves)
    else:
        return 0

def choose_last(state):
    if len(state.moves) > 0:
        return state.moves[-1]
    else:
        return 0