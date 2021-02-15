#ifndef SENET_XPLY
#define SENET_XPLY
typedef unsigned char ui8;
typedef unsigned int ui16;
typedef unsigned long int ui32; 
typedef unsigned long long int ui64;
typedef struct _xState {
    union {
        /*
         * 1st bit for agent, 2d-4d for steps, other for board
         * right to left indexation
         */
        ui64 _seed; 
        struct {
            ui64 _agent:1; // current ply active agent 
            ui64 _steps:3; // 1-5, 0 for unset
            ui64 _board:60;// 30x array, 
        };
    };
} xState; 
typedef struct _xMoves{
    union {
        /*
         * 1st bit for direction, 2d for length, other for 5 movement indices 
         * right to left indexation
         */
        ui32 _seed; 
        struct {
            ui32 _dir:2; // 0: no movement, 1: normal, 2: backward
            ui32 _len:3; // length of avaliable movements
            ui32 _mvs:27;
        };
    };
} xMoves;
#define BOARD_GET(state, cell) ((state._board >> (cell << 1)) % 4)
#define BOARD_SET(state, cell, val) \
state._board &= ~((ui64) 3 << (cell << 1));    \
state._board ^= ((ui64) val << (cell << 1));   \

#define MOVES_GET(moves, index) ((moves._mvs >> (index * 5)) % 32)
#define MOVES_SET(moves, index, val) \
    moves._mvs &= ~((ui32) 31 << (index * 5));    \
    moves._mvs ^= ((ui32) val << (index * 5));   \


static const float P[5] = {.25, .375, .25, .0625, .0625};//chance probabilities

typedef ui64 (*state_increment_func)(ui64 seed, ui8 move);
typedef ui32 (*state_legal_moves_func)(ui64 seed);
typedef float (*state_evaluation_func)(ui64 seed);

typedef enum _evaluation_function_id {
    id_eval_basic,
    id_eval_basic_zero
} eval_id_e;
typedef enum _state_increment_id {
    id_incr_meub,
    id_incr_kendal
} incr_id_e;

state_increment_func get_increment_func(incr_id_e id);
state_legal_moves_func get_legal_moves_func(incr_id_e id);
state_evaluation_func get_evaluation_func(eval_id_e id);

ui32 get_moves_meub(ui64 seed);
ui64 increment_meub(ui64 seed, ui8 m);

float eval_basic(ui64 seed);
float eval_basic_zero(ui64 seed);


typedef struct _emax_res {
    ui8 strategy;
    ui32 searched_nodes;
} emax_res;

emax_res get_strategy_emax_mt(ui64 seed, ui8 depth, ui8 sec, eval_id_e id_eval, incr_id_e id_incr);
#endif