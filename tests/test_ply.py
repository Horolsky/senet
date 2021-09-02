from senet.core.enums import Action, Unit
import unittest
from senet.core.xtc import State,StrategyNode
from senet.core import Ply

class TestState(unittest.TestCase):

    def test_init(self):
        State()

class TestPly(unittest.TestCase):

    def test_init(self):
        self.assertRaises(AttributeError, Ply)

    def test_agent(self):
        p = Ply(chance=1)
        self.assertEqual(p.agent, Unit.X)

    def test_board(self):
        p = Ply(chance=1)
        self.assertEqual(p.board[0], Unit.X)
        self.assertEqual(p.board[29], Unit.NONE)

    def test_event(self):
        p = Ply(chance=1)
        self.assertIsInstance(p.event,(Ply.Event,))

    def test_expectation(self):
        p = Ply(chance=1)
        self.assertIsInstance(p.expectation,(float,))

    def test_increment(self):
        p = Ply(chance=1)
        self.assertIsInstance(p.increment(0,1), (Ply,))
    
    def test_seed(self):
        p = Ply(chance=1)
        self.assertIsInstance(p.seed,(int,))

    def test_state(self):
        p = Ply(chance=1)
        self.assertIsInstance(p.state,(StrategyNode,))
    
    def test_steps(self):
        p = Ply(chance=1)
        self.assertEqual(p.steps,1)
        
    def test_strats(self):
        p = Ply(chance=1)    
        self.assertTrue(p.strategies.actions[0] == Action.ATTACK)
