from array import ArrayType, array
from app.core.xtc import Dice as _Dice

class Dice:
        def __init__(self) -> None:
            self.__dice = _Dice()
            self.__sticks = array('i', [0 for _ in range(_Dice.stick_number)])
            self.__steps = 0
            
        def roll(self) -> int:
            self.__steps = self.__dice.roll()
            self.__dice.sticks(self.__sticks)
            return self.__steps
        @property
        def sticks(self) -> array:
            return self.__sticks
        @property
        def steps(self) -> int:
            return self.__steps