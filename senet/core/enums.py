"""
builtin enums wrapping
"""
from .xtc import Unit as _Unit, House as _House, Action as _Action, Eval as _Eval, Emax as _Emax
from enum import Enum

class House(Enum):
    REBIRTH = _House.REBIRTH
    BEAUTY = _House.BEAUTY
    WATERS = _House.WATERS
    TRUTHS = _House.TRUTHS
    ATOUM = _House.ATOUM
    SCARAB = _House.SCARAB
    NETHER = _House.NETHER
    SKIPTURN = _House.SKIPTURN
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}.{self.name}"

class Unit(Enum):
    X = _Unit.X
    Y = _Unit.Y
    NONE = _Unit.NONE
    def __repr__(self) -> str:
        if self.value == _Unit.NONE:
            return "_"
        else : return self.name
        # return f"{self.__class__.__name__}.{self.name}"

class Action(Enum):
    
    SKIP = _Action.SKIP
    MOVE = _Action.MOVE
    RETREAT = _Action.RETREAT
    SWAPBACK = _Action.SWAPBACK
    DROW = _Action.DROW
    ATTACK = _Action.ATTACK
    ATTACK_HOUSE = _Action.ATTACK_HOUSE
    ESCAPE = _Action.ESCAPE
    PANIC = _Action.PANIC
    NONE = _Action.NONE
    ERROR = _Action.ERROR

    def __repr__(self) -> str:
        return self.name
        # return f"{self.__class__.__name__}.{self.name}"

class EvalFunc(Enum):
    BASIC = _Eval.FuncID.BASIC
    LINEAR = _Eval.FuncID.LINEAR
    POLYNOMIAL = _Eval.FuncID.POLYNOMIAL
    def __repr__(self) -> str:
        return self.name

class EmaxAlgo(Enum):
    RECURSIVE = _Emax.AlgoID.RECURSIVE
    ITERATIVE = _Emax.AlgoID.ITERATIVE
    def __repr__(self) -> str:
        return self.name