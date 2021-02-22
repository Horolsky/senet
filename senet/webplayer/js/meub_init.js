_get_index = function(move){
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
/* on move */
_state_before_move = {
    state_index: game_obj.state_index, 
    move: 0, 
    steps: game_obj.dice_value, 
    agent: game_obj.state_index + 1, 
    bb: JSON.stringify(game_obj.bb)
}
_game_log_buffer = [];
game_obj._orig_move = game_obj.generic_move;
game_obj._move_trap = function(move, callback){
    let record = {
        state_index: game_obj.state_index,
        move: _get_index(move),
        steps: game_obj.dice_value,
        agent: move[3],
        bb: JSON.stringify(game_obj.bb),
    }
    _game_log_buffer.push(record);
    return game_obj._orig_move(move, callback);
}
game_obj.generic_move = game_obj._move_trap;

