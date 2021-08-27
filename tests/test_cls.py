from senet.core.enums import *
import unittest
from senet.core.xtc import State, Eval, Emax
from senet.core import Ply

class TestState(unittest.TestCase):

    def test_init(self):
        Emax()
    def test_call(self):
        p = Ply(chance=1)
        e = Emax()
        c = e(p.state, 2, 3000)
        self.assertIsInstance(c, (int,))