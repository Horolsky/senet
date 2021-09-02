from senet.ui import cli
import unittest

class TestAutoplay(unittest.TestCase):

    def test_call(self):
        cli().autoplay(["a", "0", "1"])
    # def test_100(self):
        # self.assertTrue(cli().autoplay(["a", "0", "100"]))