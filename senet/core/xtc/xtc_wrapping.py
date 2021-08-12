import cppyy
from os import path
PATH = path.dirname(path.realpath(__file__))

__SRC = [
        (["unit_id","event_id", "rules_id"], f"{PATH}/src/xtc.fwd.hpp"),
        (["xState"], f"{PATH}/src/s_state.hpp"),
        (["xMoves"], f"{PATH}/src/s_moves.hpp"),
        (["xEvent"], f"{PATH}/src/s_event.hpp"),
        (["xPly"], f"{PATH}/src/s_ply.hpp"),
        (["evaluate_state"], f"{PATH}/src/f_eval.hpp"),
        (["increment_state","increment_state_meub","increment_state_kendall"], f"{PATH}/src/f_rules.hpp")
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


__load_src()

__all__ += ['Ply']
class Ply(globals()['xPly']):
    """
    python wrapper on x++ Xply class
    """
    def __init__(self, *args, **kwargs):
        super(Ply, self).__init__(*args, **kwargs)