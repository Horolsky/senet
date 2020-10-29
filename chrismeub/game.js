var gs = {
  orthogonal_dirs: [
    [1, 0],
    [0, -1],
    [-1, 0],
    [0, 1],
  ],
  diagonal_dirs: [
    [1, 1],
    [1, -1],
    [-1, -1],
    [-1, 1],
  ],
  adjacent_dirs: [
    [1, 0],
    [0, -1],
    [-1, 0],
    [0, 1],
    [1, 1],
    [1, -1],
    [-1, -1],
    [-1, 1],
  ],
  add: function (a, b, c) {
    var d = [a[0] + b[0], a[1] + b[1]];
    if (c) {
      d[0] += c[0];
      d[1] += c[1];
    }
    return d;
  },
  rotate_cw: function (a) {
    return [a[1], -a[0]];
  },
  rotate_ccw: function (a) {
    return [-a[1], a[0]];
  },
  equal: function (a, b) {
    return a[0] == b[0] && a[1] == b[1];
  },
  distance: function (a, b) {
    var c = [a[0] - b[0], a[1] - b[1]];
    var d = Math.pow(c[0], 2) + Math.pow(c[1], 2);
    return d > 0 ? Math.sqrt(d) : 0;
  },
  swap: function (a, b, c) {
    var d = a[b];
    a[b] = a[c];
    a[c] = d;
  },
  reverse: function (a) {
    var b = Math.floor(a.length / 2);
    for (var c = 0; c < b; ++c) gs.swap(a, c, a.length - 1 - c);
  },
  is_adjacent: function (a, b) {
    return (
      !gs.equal(a, b) &&
      Math.abs(b[0] - a[0]) <= 1 &&
      Math.abs(b[1] - a[1]) <= 1
    );
  },
  invalid_move: function (a, b) {
    var c = a.at(b);
    gs.pulse_bg(c, a.clr_invalid, 120);
  },
  pulse: function (a, b, c, d) {
    d = d || 150;
    var e = {};
    e[b] = c;
    var f = {};
    f[b] = null;
    a.animate(e, d / 2, null, function () {
      a.animate(f, d);
    });
  },
  pulse_bg: function (a, b, c) {
    gs.pulse(a, "background", b, c);
  },
  highlight: function (a, b, c) {
    if (b)
      b.animate(
        {
          background: c || a.clr_highlight,
        },
        100
      );
  },
  unhighlight: function (a, b) {
    if (b)
      b.animate(
        {
          background: null,
        },
        100
      );
  },
  select: function (a, b) {
    if (b) b.addClass("selected");
  },
  unselect: function (a, b) {
    if (b) b.removeClass("selected");
  },
  animate_move: function (a, b, c, d, e) {
    var f = a.offset();
    a.appendTo(b);
    var g = a.offset();
    if (e) {
      g.left += e[0];
      g.top -= e[1];
    }
    var h = a.clone().appendTo("body");
    h.css({ position: "absolute", left: f.left, top: f.top, "z-index": 1000 });
    a.hide();
    h.animate(
      {
        top: g.top,
        left: g.left,
      },
      100 + c * 100,
      "ease-out",
      function () {
        a.show();
        h.remove();
        d && d(a);
      }
    );
  },
  current_game: null,
  initialize_game: function (a, b, c, d, e) {
    gs.reverse(d);
    var f = (gs.current_game = {
      size: [d[0].length, d.length],
      game_size_class: b,
      initial_config: d,
      cell_backgrounds: e,
      uses_piece_picking: c,
      is_loss_if_stuck: false,
      ai_depth: 6,
      ai_mistake_chance: 0.03,
      infinity: 10000000,
      win_score: 10000,
      wins: 0,
      losses: 0,
      ties: 0,
      player_moves_first: false,
      root: a,
      board: a.find(".board"),
      state: a.find(".state"),
      score: a.find(".score"),
      roll: a.find(".roll"),
      dice: a.find(".dice"),
      btn_new_game: a.find(".btn_new_game"),
      piece_map: {
        0: "",
      },
      piece_value_map: {},
      piece_count_map: {},
      piece_move_map: {},
      total_piece_value: 0,
      cells: [],
      rows: [],
      bb: [],
      player_piece: null,
      player_piece_coord: null,
      player_piece_moves: null,
      player_can_move: 0,
      opponent_piece: null,
      opponent_piece_coord: null,
      is_player_turn: true,
      state_index: 0,
      state_map: {
        0: "It's your move.",
        1: "Opponent is moving...",
        2: "You have won!",
        3: "You have lost.",
        4: "Game ended in stalemate.",
      },
      dice_value: 1,
      roll_dice: null,
      pick_restriction: null,
      put_restriction: null,
      minimax_increment_depth: function (a) {
        return a + 1;
      },
      ai_wait_ms: function () {
        return 200;
      },
      is_stalemate: function () {
        return false;
      },
      cell_select: function () {
        return false;
      },
      clr_invalid: "#707171",
      clr_highlight: "#55AEEE",
      clr_lowlight: "#83C6F7",
    });
    f.has = function (a) {
      return a[0] >= 0 && a[1] >= 0 && a[0] < f.size[0] && a[1] < f.size[1];
    };
    f.is_on_edge = function (a) {
      return (
        a[0] == 0 || a[1] == 0 || a[0] == f.size[0] - 1 || a[1] == f.size[1] - 1
      );
    };
    f.wrap = function (a) {
      return [(a[0] + f.size[0]) % f.size[0], (a[1] + f.size[1]) % f.size[1]];
    };
    f.get = function (a) {
      return f.bb[a[1]][a[0]];
    };
    f.set = function (a, b) {
      if (!f.has(a)) {
        console.log("Cannot set bad coord: " + a);
        return b;
      }
      var c = f.bb[a[1]][a[0]];
      f.total_piece_value -= f.piece_value_map[c];
      f.piece_count_map[c]--;
      f.total_piece_value += f.piece_value_map[b];
      f.piece_count_map[b]++;
      f.bb[a[1]][a[0]] = b;
      return c;
    };
    f.at = function (a) {
      return f.rows[a[1]][a[0]];
    };
    f.piece_at = function (a) {
      return f.at(a).children(".piece");
    };
    f.estimation = function (a, b) {
      return f.total_piece_value + Math.random();
    };
    f.piece_has_moves = function (a) {
      var b = false;
      var c = f.get(a);
      if (c)
        f.piece_move_map[c](a, c, function (c) {
          if (!gs.equal(a, c[2])) b = true;
        });

      return b;
    };
    f.change_state = function (a) {
      f.state_index = a;
      if (f.state_index in f.state_map)
        f.state.text(f.state_map[f.state_index]).fadeIn();
      else f.state.text("");

      if (f.roll_dice && f.state_index <= 1) {
        f.dice_value = f.roll_dice();
        f.dice.html(["⚀", "⚁", "⚂", "⚃", "⚄", "⚅"][f.dice_value - 1]);
        f.dice.attr("title", f.dice_value);
        f.roll.html(f.state_index ? "Opponent roll:" : "Your roll:");
        f.dice.fadeIn();
      }
      switch (f.state_index) {
        case 0:
          f.is_player_turn = true;
          f.on_player_turn && f.on_player_turn();
          return;
        case 1:
          f.is_player_turn = false;
          return;
        case 2:
          f.wins++;
          break;
        case 3:
          f.losses++;
          break;
        case 4:
          f.ties++;
          break;
        default:
          return;
      }
      f.score.text(
        f.wins + " Wins, " + f.losses + " Losses, " + f.ties + " Ties"
      );
    };
    f.new_game = function () {
      f.player_moves_first = !f.player_moves_first;
      f.create_board();
      f.change_state(f.player_moves_first ? 0 : 1);
      if (f.state_index == 1) f.opponent_move();
    };
    f.create_board = function () {
      for (var a in f.piece_map) f.piece_count_map[a] = 0;

      f.total_piece_value = 0;
      pick_restriction = null;
      put_restriction = null;
      f.rows = [];
      f.cells = [];
      f.bb = [];
      f.board.empty();
      if (f.game_size_class) $(document.body).addClass(f.game_size_class);

      if (f.roll_dice) {
        f.roll.html("Your roll: ");
        f.dice.html("⚄");
      }
      for (var b = 0; b < f.size[1]; ++b) {
        f.bb.push([]);
        f.rows.push([]);
      }
      for (var b = 0; b < f.size[1]; ++b)
        for (var c = 0; c < f.size[0]; ++c) {
          f.bb[b].push(0);
          f.set([c, b], f.initial_config[b][c]);
        }

      for (var b = f.size[1] - 1; b >= 0; --b) {
        for (var c = 0; c < f.size[0]; ++c) {
          var d = $("<a></a>");
          d.addClass("cell");
          d.click(f.cell_select);
          d = d.appendTo(f.board);
          if (f.cell_backgrounds) {
            var e = f.cell_backgrounds[f.size[1] - b - 1][c];
            if (e) d.append(e);
          }
          d[0].data = [c, b];
          f.cells.push(d);
          f.rows[b].push(d);
        }
        if (b > 0) f.board.append($("<br>"));
      }
      f.update_board();
      f.root.fadeIn();
    };
    f.is_mistake = function () {
      var a = Math.random();
      return a > 0.5 && a < 0.5 + f.ai_mistake_chance;
    };
    f.update_board = function () {
      for (var a = 0; a < f.size[1]; ++a)
        for (var b = 0; b < f.size[0]; ++b) {
          var c = [b, a];
          var d = f.get(c);
          var e = f.at(c);
          var g = e.children(".piece");
          if (d && g.length == 0)
            g = $('<div class="piece"></div>').appendTo(e);
          else if (!d && g.length > 0) {
            g.remove();
            continue;
          }
          g.html(f.piece_map[d]);
        }
    };
    f.btn_new_game.click(function () {
      f.new_game();
      return false;
    });
    f.check_finished = function () {
      var a = {
        coords: null,
      };
      if (f.is_player_win(a)) f.change_state(2);
      else if (f.is_opponent_win(a)) f.change_state(3);
      else if (f.is_stalemate()) f.change_state(4);
      else return false;

      if (a.coords) {
        var b = $();
        for (var c = 0; c < a.coords.length; ++c) {
          var d = a.coords[c];
          b = b.add(f.at(d));
        }
        gs.highlight(f, b);
      }
      return true;
    };
    f.player_pick = function (a, b, c) {
      gs.select(f, f.at(a));
      f.player_piece = b;
      f.player_piece_coord = a;
      f.player_piece_moves = [];
      var d = $();
      (c ? c : f.piece_move_map[b])(a, b, function (a) {
        f.player_piece_moves.push(a);
        if (f.has(a[2])) d = d.add(f.at(a[2]));
      });
      gs.highlight(f, d, f.clr_lowlight);
    };
    f.player_unpick = function () {
      if (f.player_piece_coord) gs.unselect(f, f.at(f.player_piece_coord));

      if (f.player_piece_moves)
        for (var a = 0; a < f.player_piece_moves.length; ++a) {
          var b = f.player_piece_moves[a][2];
          if (f.has(b)) gs.unhighlight(f, f.at(b));
        }

      f.player_piece = null;
      f.player_piece_coord = null;
      f.player_piece_moves = null;
    };
    f.is_valid_player_piece_move = function (a) {
      if (f.player_piece_moves)
        for (var b = 0; b < f.player_piece_moves.length; ++b)
          if (gs.equal(a, f.player_piece_moves[b][2]))
            return f.player_piece_moves[b];

      return null;
    };
    f.ai_choose_move = function () {
      moves_checked = 0;
      var a = gs.minimax_move(f, 0, 0);
      if (!a || a.length != 2 || !a[1])
        console.log("Error, no best move found: " + a);

      var b = a[0];
      var c = a[1];
      return c;
    };
    return f;
  },
  moves_checked: 0,
  minimax: function (a, b, c) {
    moves_checked++;
    var d = !a.is_player_turn;
    var e = a.win_score - b * 2;
    if (a.is_player_win()) return -e;

    if (a.is_opponent_win()) return e;

    if (b >= a.ai_depth || a.is_stalemate() || a.is_mistake())
      return a.estimation(b, c);

    var f = d ? -a.infinity : a.infinity;
    var g = false;
    var h = a.minimax_increment_depth(b);
    var i = c + 1;
    a.make_all_moves(function () {
      g = true;
      var b = gs.minimax(a, h, i);
      f = d ? Math.max(f, b) : Math.min(f, b);
    });
    if (!g)
      f = a.is_loss_if_stuck ? (a.is_player_turn ? e : -e) : a.estimation(b, c);

    return f;
  },
  minimax_move: function (a, b, c) {
    var d = !a.is_player_turn;
    var e = d ? -a.infinity : a.infinity;
    var f = null;
    var g = false;
    var h = a.minimax_increment_depth(b);
    var i = c + 1;
    a.make_all_moves(function (b) {
      g = true;
      var c = gs.minimax(a, h, i);
      if ((d && c > e) || (!d && c < e)) {
        e = c;
        f = b;
      }
    });
    if (!g)
      e = a.is_loss_if_stuck
        ? a.is_player_turn
          ? a.win_score
          : -a.win_score
        : a.estimation(b, c);

    return [e, f];
  },
  on_body_click: function () {
    if (
      gs.current_game &&
      gs.current_game.uses_piece_picking &&
      !gs.current_game.pick_restriction
    )
      gs.current_game.player_unpick();

    return true;
  },
};
