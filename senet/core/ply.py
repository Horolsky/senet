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
            return "("+ ", ".join([f"{self.indici[i]}: {repr(self.actions[i])}" for i in range(self.mobility) ]) + ")"

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
            return f"({repr(self.agent)} made {repr(self.action)} from {self.start} to {self.destination})"

    def __init__(self, **kwargs):
        state, seed, event = None, None, None

        # fast cinit
        if "seed" in kwargs:
            seed = kwargs["seed"]
            if type(seed) not in (int, uint64_t):
                raise TypeError("invalid seed type")
            seed = uint64_t(seed)
            state = StrategyNode(seed)
        # manual init
        elif "state" in kwargs:
            state = StrategyNode(kwargs["state"])
            seed = state.seed()
        elif "chance" in kwargs:
            chance = kwargs["chance"]
            agent = Unit(kwargs.get("agent", Unit.X))
            if type(chance) != int:
                raise TypeError("invalid chance type")
            if type(agent) != Unit:
                raise TypeError("invalid agent type")
            # s = State()
            a = agent.value
            seed = uint64_t(State.build_seed(a, chance))
            state = StrategyNode(State(seed))
            # seed = state.seed()

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
        self.__expectation = None

    @property
    def state(self) -> StrategyNode:
        return self.__state
    @property
    def seed(self) -> uint64_t:
        return self.__seed
    @property
    def agent(self) -> Unit:
        return Unit(self.__state.agent())
    @property
    def enemy(self) -> Unit:
        return Unit(self.__state.enemy())
    @property
    def steps(self) -> Unit:
        return self.__state.steps()
    @property
    def expectation(self) -> float:
        if self.__expectation is None:
            self.__expectation = Eval()(self.__state)
        return self.__expectation
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
            return f"Ply({repr(self.agent)}, s: {self.steps}, {str(self.board)})"

    def increment(self, choice:int, chance:int):
        # if choice not in self.strategies.indici:
        #     raise RuntimeError("invalid strategy")
        strategies = self.strategies._strategies
        state = self.__state.choice(choice,strategies).chance(chance)
        index = self.strategies.indici.index(choice)
        event = Ply.Event(
            agent=self.agent, 
            action=self.strategies.actions[index], 
            start=choice, 
            destination=choice+self.steps
            )
        return Ply(state=state, event=event)
    
    