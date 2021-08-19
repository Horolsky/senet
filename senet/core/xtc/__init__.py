"""
senet c++ extension builtins
"""
import cppyy
from os import path
PATH = path.dirname(path.realpath(__file__))

__HDR = [f"{PATH}/src/xtc.common.hpp", f"{PATH}/src/xtc.internal.hpp"]
__SMBS = ["cnst", "House", "Unit", "Action", "Rules", "Moves", "Event", "Emax", "State"]

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
    
    globals()['__internal'] = __internal#cppyy.gbl.xtc
    

def __load_std():
    for symbols, header in __STD_T:
        if not cppyy.include(header):
            raise ImportError(f"unable to load {header}")
        for smb in symbols:
            globals()['__internal'][smb] = eval(f"cppyy.gbl.std.{smb}")
        
__load_src()
__load_std()

__all__ = ['__internal']