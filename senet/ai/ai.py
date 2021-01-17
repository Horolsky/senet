import threading
import time
from math import inf
from senet.core import agent, Ply
from senet.utils.report import report
from senet.settings import SETTINGS
from senet.ai.xeval import emm

DEPTH = 6 #settings: ai level
WAIT = 6 #TODO put to settings or derive from level
class AIplayer():
    def __init__(self, number):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")
        self._agent = number
        self._name = "AI"
        self._tree = []
        self._dec = 0
        self._turn = 0
        self.stopFlag = False
        self._timer = SETTINGS.get("ai/timer")
        
    def choose_movement(self, state):
        #TODO: check state
        if state.agent != self._agent:
            raise ReferenceError("wrong agent call")
        self._state = state

        #some correct value is guaranteed
        self._dec = 0
        self._util = (-inf, inf)[self._agent - 1]
        if len(state.moves) > 0:
            self._dec = state.moves[0] 

        #TODO do work in other thread

        #test thread
        self.stopFlag = False
        t = AIthread(self)
        t.start()
        counter = int(self._timer)#check negat
        while counter:
            #stop if ready
            if self.stopFlag:
                break
            #wait
            counter -= 1
            time.sleep(.1)
        #self.stopFlag = True #stopping thread 
        return self._dec

    def think(self):
        """
        recursive tree traversing
        assigning best value to self._dec 
        """
        if len(self._state.moves) < 2:
            return
        for move in self._state.moves:
            res = emm.emm(self._state.increment(move)._xstate["_bitvalue"], 4)
            print(f"iters: {res[1]}")
            if (self._agent == 1 and res[0] > self._util) or (self._agent == 2 and res[0] < self._util):
                self._util = res[0]
                self._dec = move
        #self.stopFlag = True
        #test delay
        #time.sleep(WAIT)
    def EMM(self, node, depth):
        """
        function expectiminimax(node, depth)
        if node is a terminal node or depth = 0
            return the heuristic value of node
        if the adversary is to play at node
            // Return value of minimum-valued child node
            let α := +∞
            foreach child of node
                α := min(α, expectiminimax(child, depth-1))
        else if we are to play at node
            // Return value of maximum-valued child node
            let α := -∞
            foreach child of node
                α := max(α, expectiminimax(child, depth-1))
        else if random event at node
            // Return weighted average of all child nodes' values
            let α := 0
            foreach child of node
                α := α + (Probability[child] × expectiminimax(child, depth-1))
        return α

        P:
        1 .25
        2 .375
        3 .25
        4 .0625
        5 .0625
        """
        A = 0
        if node.utility  == 1 or node.utility == 0 or depth == 0:
                return node.utility
        if (self._agent == 1):
            A = -inf
            

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