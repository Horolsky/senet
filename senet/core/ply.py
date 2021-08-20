"""
wrapper for builtin structures
"""
from .xtc import State, Moves
from .constants import BOARD_SIZE
from .enums import Action, Rules, Unit, House
from array import array
from typing import Union, Tuple

class Ply:
    """
    senet game ply
    """
    class MovesView:
        def __init__(self, moves: Moves):
            self.__moves = moves
            self.__indici = None
            self.__actions = None
        
        @property
        def mobility(self) -> int:
            return self.__moves.mobility()
        @property
        def indici(self) -> Tuple[int]:
            if self.__indici is None:
                if self.mobility == 0:
                    self.__indici = ()
                else:
                    cont = array('i', [0 for _ in range(self.mobility)])
                    self.__moves.indici(cont)
                    self.__indici = tuple(cont)
                    del cont
            return self.__indici
        @property
        def actions(self) -> Tuple[Action]:
            if self.__actions is None:
                if self.mobility == 0:
                    self.__actions = ()
                else:
                    cont = array('i', [0 for _ in range(self.mobility)])
                    self.__moves.actions(cont)
                    self.__actions = tuple(map(lambda x: Action(x), cont))
                    del cont
            return self.__actions

    class Event:
        def __init__(self, **kwargs):
            self.__agent = kwargs.get("agent", Unit.NONE)
            self.__action = kwargs.get("action", Action.NONE)
            self.__start = kwargs.get("start", 0)
            self.__destination = kwargs.get("destination", 0)
        
        @property
        def agent(self) -> Unit:
            return self.__agent
        @property
        def action(self) -> Action:
            return self.__action
        @property
        def start(self) -> int:
            return self.__start
        @property
        def destination(self) -> int:
            return self.__destination

    def __init__(self, state: Union[int, State] = State(), rules: Rules = Rules.MEUB, event: Event = Event()):
        if type(state) not in (int, State.seed_type, State):
            raise TypeError("invalid state type") 
        if type(rules) != Rules:
            raise TypeError("invalid rules type") 
        if type(event) != Ply.Event:
            raise TypeError("invalid event type") 
        
        if type(state) in (int, State.seed_type):
            state = State(State.seed_type(state))
        self.__state = state
        self.__seed = state.seed()
        self.__board = None
        
        self.__rules = rules.value
        self.__event = event
        self.__moves = Ply.MovesView(Moves(state, rules.value))

    @property
    def seed(self) -> int:
        return self.__seed
    @property
    def rules(self) -> Rules:
        return Rules(self.__rules)
    @property
    def agent(self) -> Unit:
        return Unit(self.__state.agent())
    @property
    def steps(self) -> Unit:
        return Unit(self.__state.steps())
    @property
    def expectation(self) -> float:
        return Unit(self.__state.expectation())
    @property
    def board(self) -> Tuple[Unit]:
        if self.__board is None:
            cont = array('i', [0 for _ in range(BOARD_SIZE)])
            self.__state.board(cont)
            self.__board = tuple(map(lambda x: Unit(x), cont))
            del cont
        return self.__board
    @property
    def event(self) -> Event:
        return self.__event
    @property
    def moves(self) -> MovesView:
        return self.__moves
    