"""
wrapper for builtin structures
"""
from .xtc import xState, xMoves, xEvent, uint8_t, uint16_t, uint32_t, uint64_t
from .constants import DEF_SEED
from .enums import Action, Rules, Unit
# from ctypes import c_uint8, c_uint16, c_uint32, c_uint64
from typing import Union, Tuple

class Ply:
    """
    senet game ply
    """
    class EventView:
        def __init__(self, event: xEvent):
            self.__event = event
        
        @property
        def seed(self) -> int:
            return Unit(self.__event.seed())
        @property
        def agent(self) -> Unit:
            return Unit(self.__event.agent())
        @property
        def action(self) -> Action:
            return Action(self.__event.action())
        @property
        def start(self) -> int:
            return ord(self.__event.start())
        @property
        def destination(self) -> int:
            return ord(self.__event.destination())

    def __init__(self, state: Union[int, xState] = xState(), rules: Rules = Rules.MEUB, event: xEvent = xEvent()):
        if type(state) not in (int, uint64_t, xState):
            raise TypeError("invalid state type") 
        if type(rules) != Rules:
            raise TypeError("invalid rules type") 
        if type(event) != xEvent:
            raise TypeError("invalid event type") 
        
        
        if type(state) in (int, uint64_t):
            state = xState(uint64_t(state))
        self.__state = state
        self.__seed = state.seed()
        
        self.__rules = rules.value
        self.__event = Ply.EventView(event)

    @property
    def seed(self) -> int:
        return self.__seed.value
    @property
    def rules(self) -> Rules:
        return Rules(self.__rules)
    @property
    def agent(self) -> Unit:
        return Unit(self.__state.agent())
    @property
    def board(self) -> Tuple[Unit]:
        return tuple(map(lambda x: Unit(ord(x)), self.__state.board()))
    @property
    def event(self) -> EventView:
        return self.__event
    