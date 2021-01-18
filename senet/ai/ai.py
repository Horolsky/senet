import threading
import time
from math import inf
from senet.core import Ply
from senet.utils.report import report
from senet.settings import SETTINGS
from senet.ai.xeval import emm

class AIplayer():
    def __init__(self, number, depth):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")
        self._agent = number
        self._name = "AI"
        self._tree = []
        self._dec = 0
        self._turn = 0
        self.stopFlag = False
        self._timer = SETTINGS.get("ai/timer")
        self._depth = depth#SETTINGS.get("ai/depth")
        
    def choose_movement(self, state):
        #TODO: check state
        if state.agent != self._agent:
            raise ReferenceError("wrong agent call")
        self._state = state

        #some correct value is guaranteed
        self._dec = 0
        self._util = (-inf, inf)[self._agent - 1]
        if len(state.moves) > 0:
            self._dec = state.moves[-1:][0]

        
        self.stopFlag = False
        t = AIthread(self)
        t.start()
        counter = float(self._timer)#check negat
        while counter > 0:
            #stop if ready
            if self.stopFlag:
                break
            #wait
            counter -= 1
            time.sleep(.05)
        #self.stopFlag = True #stopping thread 
        return self._dec

    def think(self):
        """
        tree traversing
        assigning best value to self._dec 
        """
        if len(self._state.moves) < 2:
            return
        #get util for guaranteed move first    
        res = emm.emm(self._state.increment(self._dec).seed, self._depth)
        self._util = res[0]
        print(f"leaves: {res[1]}")

        for move in self._state.moves[:-1]:
            res = emm.emm(self._state.increment(move).seed, self._depth)
            print(f"leaves: {res[1]}")
            if (self._agent == 1 and res[0] > self._util) or (self._agent == 2 and res[0] < self._util):
                self._util = res[0]
                self._dec = move        

class AIthread (threading.Thread):
    def __init__(self, ai):
        threading.Thread.__init__(self)
        self.ai = ai
        #self.threadID = threadID
        #self.name = name
        
    def run(self):
        self.ai.think()
        self.ai.stopFlag = True
        #do work