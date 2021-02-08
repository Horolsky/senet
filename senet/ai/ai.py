import threading
import time
import ctypes
from math import inf
from senet.utils.report import report
from time import perf_counter_ns
from senet.settings import SETTINGS
from senet.xtc import emax_brute

class AIplayer():
    def __init__(self, number, depth):
        if number not in [1, 2]:
            raise ValueError("invalid agent number value")

        self.__log = SETTINGS.get("dev/ailogs")
        self._agent = number
        self._name = "AI"
        self._tree = []
        self._dec = 0
        self._turn = 0
        self.stopFlag = False
        self._timer = SETTINGS.get("ai/timer")
        self._depth = depth#SETTINGS.get("ai/depth")
        if self.__log:
            header = f"seed;t, ns;t, ~sec;leaves searched;agent: {self._agent}, timer: {self._timer}, depth: {self._depth}\n"
            self._report = report(f"ai-{number}", "csv", "logs/ai", header)
        self._leaves = 0

    def __del__(self):
        if self.__log:
            self._report.close()
        
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
        self._leaves = 0
        timeA = perf_counter_ns()
        t.start()
        counter = self._timer * 20 
        while counter > 0: #TODO check negat
            #stop if ready
            if self.stopFlag:
                break
            #wait
            counter -= 1
            time.sleep(.05)
        timeB = perf_counter_ns()
        if self.__log:
            nano = timeB - timeA
            self._report.write(f"{state.seed};{nano};{nano / 10**9};{self._leaves};\n")
        t.join()
        t.ai = None
        return self._dec     

class AIthread (threading.Thread):
    def __init__(self, ai):
        threading.Thread.__init__(self)
        self.ai = ai
        #self.threadID = threadID
        #self.name = name
        
    def run(self):
        self.think()
        self.ai.stopFlag = True
        self.raise_exception()
        #do work
    def think(self):
        """
        tree traversing
        assigning best value to self._dec 
        """
        if len(self.ai._state.moves) < 2:
            return
        #get util for guaranteed move first    
        
        res = emax_brute(self.ai._state.increment(self.ai._dec).seed, self.ai._depth)
        
        self.ai._util = res[0]
        self.ai._leaves += res[1]

        for move in self.ai._state.moves[:-1]:
            res = emax_brute(self.ai._state.increment(move).seed, self.ai._depth)
            
            self.ai._leaves += res[1]
            if (self.ai._agent == 1 and res[0] > self.ai._util) or (self.ai._agent == 2 and res[0] < self.ai._util):
                self.ai._util = res[0]
                self.ai._dec = move   
    def get_id(self): 
  
        # returns id of the respective thread 
        if hasattr(self, '_thread_id'): 
            return self._thread_id 
        for id, thread in threading._active.items(): 
            if thread is self: 
                return id
    def raise_exception(self): 
        thread_id = self.get_id() 
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id, 
              ctypes.py_object(SystemExit)) 
        if res > 1: 
            ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id, 0) 
            print('Exception raise failure') 

    #def join(self, timeout=None):
    #    """ Stop the thread. """
    #    self._stopevent.set(  )
    #    threading.Thread.join(self, timeout)