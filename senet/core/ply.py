"""
wrapper for builtin structures
"""
from .xtc import State, Moves, Event, cnst, uint64_t #, Unit, RUles, Action, House
# from .cnst import DEF_SEED
from .enums import Action, Rules, Unit, House
from ctypes import c_int, c_uint64, byref
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

    class EventView:
        def __init__(self, event: Event):
            self.__event = event
        
        @property
        def seed(self) -> int:
            return self.__event.seed()
        @property
        def agent(self) -> Unit:
            return Unit(self.__event.agent())
        @property
        def action(self) -> Action:
            return Action(self.__event.action())
        @property
        def start(self) -> int:
            return self.__event.start()
        @property
        def destination(self) -> int:
            return self.__event.destination()

    def __init__(self, state: Union[int, State] = State(), rules: Rules = Rules.MEUB, event: Event = Event()):
        if type(state) not in (int, uint64_t, State):
            raise TypeError("invalid state type") 
        if type(rules) != Rules:
            raise TypeError("invalid rules type") 
        if type(event) != Event:
            raise TypeError("invalid event type") 
        
        
        if type(state) in (int, uint64_t):
            state = State(uint64_t(state))
        self.__state = state
        self.__seed = state.seed()
        self.__board = None
        
        self.__rules = rules.value
        self.__event = Ply.EventView(event)
        self.__moves = Ply.MovesView(state.moves(rules.value))

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
    def expectation(self) -> float:
        return Unit(self.__state.expectation())
    @property
    def board(self) -> Tuple[Unit]:
        if self.__board is None:
            cont = array('i', [0 for _ in range(30)])
            self.__state.board(cont)
            self.__board = tuple(map(lambda x: Unit(x), cont))
            del cont
        return self.__board
    @property
    def event(self) -> EventView:
        return self.__event
    @property
    def moves(self) -> MovesView:
        return self.__moves
    