import threading
import time
from senet.core import state, agent
from senet.utils.report import report

DEPTH = 6 #settings: ai level
WAIT = 2 #TODO put to settings or derive from level
class AIplayer():
    def __init__(self, number):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")
        self._number = number
        self._tree = []
        self._dec = 0
        self._turn = 0
        self.stopFlag = False

        
    def choose_movement(self, state):
        #TODO: check state
        if state.agent != self._number:
            raise ReferenceError("wrong agent call")
        self._currentstate = state

        #some correct value is guaranteed
        self._dec = None
        if len(state.moves) > 0:
            self._dec = state.moves[0] 

        #TODO do work in other thread

        #test thread
        self.stopFlag = False
        t = AIthread(self)
        t.start()
        counter = int(WAIT)#check negat
        while counter:
            #stop if ready
            if self.stopFlag:
                break
            #wait
            counter -= 1
            time.sleep(1)
        return self._dec

    def think(self):
        """
        recursive tree traversing
        assigning best value to self._dec 
        """
        #test delay
        time.sleep(WAIT)

        

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