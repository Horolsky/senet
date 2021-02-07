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
            ui32 _mv0:5;
            ui32 _mv1:5;
            ui32 _mv2:5;
            ui32 _mv3:5;
            ui32 _mv4:5;
        };
    };
} xMoves;

xMoves get_moves(xState s);
xState increment1(xState s, ui8 m);
float eval_basic(xState s);