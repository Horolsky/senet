"""
wrapper for builtin structures
"""
from .xtc import uint64_t, State, Strategies, Eval, StrategyNode, ChanceNode
from .enums import Action, Unit, House
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
            return "("+ ", ".join([f"{self.indici[i]}: {self.actions[i]}" for i in range(self.mobility) ]) + ")"

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
            return f"({repr(self.agent)} did {repr(self.action)} from {self.start} to {self.destination})"

    def __init__(self, **kwargs):
        state, seed, event = None, None, None

        # fast cinit
        if "seed" in kwargs:
            seed = kwargs["seed"]
            if type(seed) not in (int, uint64_t):
                raise TypeError("invalid seed type")
            state = StrategyNode(uint64_t(seed))
        # manual init
        elif "state" in kwargs:
            state = kwargs["state"]
            if type(state) != StrategyNode:
                raise TypeError("invalid StrategyNode type")
            seed = state.seed()
        elif "chance" in kwargs:
            chance = kwargs["chance"]
            agent = Unit(kwargs.get("agent", Unit.X))
            if type(chance) != int:
                raise TypeError("invalid chance type")
            if type(agent) != Unit:
                raise TypeError("invalid agent type")
            state = StrategyNode(State(), agent.value, chance)
            seed = state.seed()

        if "event" in kwargs:
            event = kwargs["event"]
            if type(event) != Ply.Event:
                raise TypeError("invalid event type")
        else:
            event = Ply.Event()

        if None in (state, seed, event):
            raise AttributeError("not enough data to build Ply")
        self.__state = state
        self.__seed = seed
        self.__event = event

        self.__board = None
        self.__stratview = None

    @property
    def seed(self) -> int:
        return self.__seed
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
        if self.__stratview is None:
            self.__stratview = Ply.StrategiesView(self.__state.strategies())
        return self.__stratview
    def __repr__(self) -> str:
            return f"Ply({self.agent}, s: {self.steps}, {str(self.board)})"

    def increment(self, choice:int, chance:int):
        # if choice not in self.strategies.indici:
        #     raise RuntimeError("invalid strategy")
        strategies = self.strategies._strategies
        state = self.__state.child(choice,strategies).child(chance)
        index = self.strategies.indici.index(choice)
        event = Ply.Event(
            agent=self.agent, 
            action=self.strategies.actions[index], 
            start=choice, 
            destination=choice+self.steps
            )
        return Ply(state=state, event=event)
    
    