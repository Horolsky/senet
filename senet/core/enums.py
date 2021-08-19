"""
builtin enums wrapping
"""
from .xtc import Rules as _Rules, Unit as _Unit, House as _House, Action as _Action
from enum import Enum

class House(Enum):
    REBIRTH = _House.REBIRTH
    BEAUTY = _House.BEAUTY
    WATERS = _House.WATERS
    TRUTHS = _House.TRUTHS
    ATOUM = _House.ATOUM
    SCARAB = _House.SCARAB
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Rules(Enum):
    MEUB = _Rules.MEUB
    KENDALL = _Rules.KENDALL
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Unit(Enum):
    X = _Unit.X
    Y = _Unit.Y
    NONE = _Unit.NONE
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Action(Enum):
    SKIP = _Action.SKIP
    MOVE = _Action.MOVE
    RETREAT = _Action.RETREAT
    ATTACK = _Action.ATTACK
    ESCAPE = _Action.ESCAPE
    DROW = _Action.DROW
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"