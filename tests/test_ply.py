import unittest
from senet.core.xtc import State

class TestState(unittest.TestCase):

    def test_init(self):
        success = True
        try:
            State()
            print("running state test")
        except:
            success = False
        self.assertEqual(success, True)
