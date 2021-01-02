import random

class agent:
    def __init__(self, df): 
        """
        df: decision function
        or dummy code str ("random", "first", "last")
        """
        if type(df) is str:
            self._df = {
                "random": choose_random,
                "first": choose_first,
                "last": choose_last
            }.get(df)
        elif callable(df):
            self._df = df
    
    def choose_movement(self, state):
        """
        accepts a GameState object
        return an index of a choosen movement
        decision is based on initial decision_function
        """
        return self._df(state)

def choose_first(state):
    if len(state.moves) > 0:
        return state.moves[0]
    else:
        return None

def choose_random(state):
    if len(state.moves) > 0:
        return random.choice(state.moves)
    else:
        return None

def choose_last(state):
    if len(state.moves) > 0:
        return state.moves[-1]
    else:
        return None