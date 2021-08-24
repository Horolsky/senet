"""
wrapper for builtin structures
"""
from .xtc import State, Strategies, FuncStrategies, FuncIncrement, Eval
from .enums import Action, Rules, Unit, House
from array import array
from typing import Union, Tuple

class Ply:
    """
    senet game ply
    """
    class StrategiesView:
        def __init__(self, strategies: Strategies):
            self._strategies = strategies
            self._indici = None
            self._actions = None
        
        @property
        def mobility(self) -> int:
            return self._strategies.mobility()
        @property
        def indici(self) -> Tuple[int]:
            if self._indici is None:
                if self.mobility == 0:
                    self._indici = ()
                else:
                    cont = array('i', [0 for _ in range(self.mobility)])
                    self._strategies.indici(cont)
                    self._indici = tuple(cont)
                    del cont
            return self._indici
        @property
        def actions(self) -> Tuple[Action]:
            if self._actions is None:
                if self.mobility == 0:
                    self._actions = ()
                else:
                    cont = array('i', [0 for _ in range(self.mobility)])
                    self._strategies.actions(cont)
                    self._actions = tuple(map(lambda x: Action(x), cont))
                    del cont
            return self._actions

        def __repr__(self) -> str:
            return f"Strategies[{self.mobility}]"

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
        def __repr__(self) -> str:
            return f"Event({self.agent}, {self.action}, {self.start}, {self.destination})"

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
        self.__getstrat = FuncStrategies(rules.value)
        self.__increment = FuncIncrement(rules.value)
        self.__stratview = Ply.StrategiesView(self.__getstrat(state))

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
        return self.__state.steps()
    @property
    def expectation(self) -> float:
        return Unit(self.__state.expectation())
    @property
    def board(self) -> Tuple[Unit]:
        if self.__board is None:
            cont = array('i', [0 for _ in range(State.board_size)])
            self.__state.board(cont)
            self.__board = tuple(map(lambda x: Unit(x), cont))
            del cont
        return self.__board
    @property
    def event(self) -> Event:
        return self.__event
    @property
    def strategies(self) -> StrategiesView:
        return self.__stratview
    def __repr__(self) -> str:
            return f"Ply({self.agent}, s: {self.steps}, {str(self.board)})"

    def increment(self, choice:int):
        if choice not in self.strategies.indici:
            raise RuntimeError("invalid strategy")
        state = self.__increment(
            self.__state,
            choice,
            self.__stratview._strategies
            )
        index = self.strategies.indici.index(choice)
        event = Ply.Event(
            agent=self.agent, 
            action=self.strategies.actions[index], 
            start=choice, 
            destination=choice+self.steps
            )
        return Ply(state, self.rules, event)
    
    