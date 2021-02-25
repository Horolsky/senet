
__bb_to_board = function(bb){
    board = []
    bb[1].reverse()
    bb.reverse()
    for (let row = 0; row < 3; row++){
        for (let col = 0; col < 10; col++){
            board.push(bb[row][col]);
        }
    }
    return board;
}

const bin_steps = ["000","001","010","011","100","101","111"];
const bin_agent = ["0", "1"];
const bin_pawn = ["00", "01", "10", "11"];
__state_to_seed = function(agent, steps, board){
    seed = bin_agent[agent - 1];
    seed = bin_steps[steps] + seed;
    for (let i = 0; i < 30; i++) seed = bin_pawn[board[i]] + seed;
    l = parseInt(seed.slice(0,32), 2);
    r = parseInt(seed.slice(32), 2);
    return `${l == "0" ? "" : l}${r}`
}

_get_index = function(move){
    if (!move.length) return 30;//skippingi
    let row = 2 - move[0][1];
    let _m = row == 1 ? 9 - move[0][0] : move[0][0];
    _m += (row*10);
    return _m;
}

_get_state = function() {
    return { 
        state_index: game_obj.state_index, 
        steps: game_obj.dice_value, 
        agent: game_obj.state_index + 1, 
        bb: JSON.stringify(game_obj.bb)
    }
}

