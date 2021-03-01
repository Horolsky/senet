cdef extern from "src-xtc.h": 
    ctypedef char ui8
    ctypedef unsigned long int ui32
    ctypedef unsigned long long int ui64
    cdef struct _xState: 
        ui64 _seed 
        ui64 _agent
        ui64 _steps
        ui64 _board
    ctypedef _xState xState
    cdef struct _xMoves: 
        ui32 _seed 
        ui32 _dir
        ui32 _len
        ui32 _mvs
    ctypedef _xMoves xMoves

    ctypedef ui64 (*state_increment_func)(ui64 seed, ui8 move)
    ctypedef ui32 (*state_legal_moves_func)(ui64 seed)
    ctypedef float (*state_evaluation_func)(ui64 seed, float* coefs)

    cdef enum _eval_id:
        id_eval_basic,
        if_eval_linear,
        id_eval_meub
    ctypedef _eval_id eval_e
    cdef enum _rules_id:
        id_incr_meub,
        id_incr_kendal
    ctypedef _rules_id rules_e

    cdef state_increment_func get_increment_func(rules_e _id)
    cdef state_legal_moves_func get_legal_moves_func(rules_e _id)
    cdef state_evaluation_func get_evaluation_func(eval_e _id)

    cdef struct _emax_res:
        ui8 strategy
        ui32 searched_nodes
    ctypedef _emax_res emax_res
    cdef emax_res get_strategy_emax_mt(ui64 seed, ui8 depth, ui8 sec, eval_e id_eval, rules_e id_incr, float *coefs)
    