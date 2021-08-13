"""
builtin enums wrapping
"""
from .xtc import unit_id, rules_id, action_id
from enum import Enum

class Rules(Enum):
    MEUB = rules_id.meub
    KENDALL = rules_id.kendall
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Unit(Enum):
    X = unit_id.X
    Y = unit_id.Y
    EMPTY = unit_id.empty
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Action(Enum):
    SKIP = action_id.skip
    MOVE = action_id.move
    RETREAT = action_id.retreat
    ATTACK = action_id.attack
    ESCAPE = action_id.escape
    DROW = action_id.drow
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"