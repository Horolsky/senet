from app.core.enums import *
import unittest
from app.core.xtc import State, Eval, Emax
from app.core import Ply

class TestEmax(unittest.TestCase):

    def test_init(self):
        Emax(Eval(), Emax.AlgoID.RECURSIVE, 2, 3000)
    def test_call(self):
        p = Ply(chance=1)
        e = Emax(Eval(), Emax.AlgoID.RECURSIVE, 2, 3000)
        c = e(p.state, )
        self.assertIsInstance(c, (int,))