"""
senet c++ extension builtins
"""
import cppyy
from os import path
PATH = path.dirname(path.realpath(__file__))

__HDR = [
    f"{PATH}/src/xtc.fwd.hpp",
    f"{PATH}/src/xtc.dice.hpp",
    f"{PATH}/src/xtc.state.hpp",
    f"{PATH}/src/xtc.strategies.hpp",
    f"{PATH}/src/xtc.eval.hpp",
    f"{PATH}/src/xtc.emax.hpp"
    ]
__SMBS = ["Dice", "House", "Unit", "Action", "Strategies", "State",  "StrategyNode", "ChanceNode", "Eval", "Emax"]

__STD_T = [
    (["uint8_t","uint16_t","uint32_t","uint64_t",], "cstdint"),
    ]

def __load_src(): 
    for header  in __HDR:
        if not cppyy.include(header):
            raise ImportError(f"unable to load {header}")
        
    cppyy.load_library(f"{PATH}/build/senet_xtc-0.0.1-x86_64-Linux.so")
    
    __internal = {}
    for smb in __SMBS:
        __internal[smb] = eval(f"cppyy.gbl.xtc.{smb}") 
    
    globals().update(__internal)#cppyy.gbl.xtc
    

def __load_std():
    _std = {}
    for symbols, header in __STD_T:
        if not cppyy.include(header):
            raise ImportError(f"unable to load {header}")
        for smb in symbols:
            _std[smb] = eval(f"cppyy.gbl.std.{smb}")

    globals().update(_std)
        
__load_src()
__load_std()

# __all__ = ['__internal']