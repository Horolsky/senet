import random

class Player:
    def __init__(self, decision_function):
        self._df = decision_function
    
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