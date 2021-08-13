"""
senet c++ extension builtins
"""
import cppyy
from os import path
PATH = path.dirname(path.realpath(__file__))

__SRC = [
        (["action_id", "rules_id", "unit_id"], f"{PATH}/src/xtc.fwd.hpp"),
        (["xState"], f"{PATH}/src/s_state.hpp"),
        (["xMoves"], f"{PATH}/src/s_moves.hpp"),
        (["xEvent"], f"{PATH}/src/s_event.hpp"),
        (["xPly"], f"{PATH}/src/s_ply.hpp"),
        (["evaluate_state"], f"{PATH}/src/f_eval.hpp"),
        (["increment_state","increment_state_meub","increment_state_kendall"], f"{PATH}/src/f_rules.hpp")
    ]

__STD_T = [
    (["uint8_t","uint16_t","uint32_t","uint64_t",], "cstdint"),
    ]

__all__ = []
def __load_src(): 
    for _, header  in __SRC:
        if not cppyy.include(header):
            raise ImportError(f"unable to load {header}")
        
    cppyy.load_library(f"{PATH}/build/senet_xtc-0.0.1-x86_64-Linux.so")

    global __all__
    for symbols, _  in __SRC:
        for smb in symbols:
            globals()[smb] = eval(f"cppyy.gbl.xtc.{smb}")
        __all__ += symbols
    globals()['__xtc_namespace'] = cppyy.gbl.xtc

def __load_std():
    for symbols, header in __STD_T:
        if not cppyy.include(header):
            raise ImportError(f"unable to load {header}")
        global __all__
        for smb in symbols:
            globals()[smb] = eval(f"cppyy.gbl.std.{smb}")
        __all__ += symbols



__load_src()
__load_std()