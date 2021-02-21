_get_index = function(move){
    let row = 2 - move[0][1];
    let _m = row == 1 ? 9 - move[0][0] : move[0][0];
    _m += (row*10);
    return _m;
}

document.addEventListener("generic_move", function(e) { 
    console.log("move intercept:"); 
    console.log(e.detail) 
});

var _gen_mv_event = new CustomEvent("generic_move", {
    detail: { move: 0, steps: 0, agent: 0, bb: null } 
});

var orig_move = game_obj.generic_move;

_wrapper = function(move, callback){
    _gen_mv_event.detail.agent = move[3];
    _gen_mv_event.detail.move = _get_index(move);
    _gen_mv_event.detail.steps = game_obj.dice_value;
    _gen_mv_event.detail.bb = JSON.stringify(game_obj.bb);

    document.dispatchEvent(_gen_mv_event);
    return orig_move(move, callback);
}

game_obj.generic_move = _wrapper;