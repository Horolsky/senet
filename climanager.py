import messages, game
GAME = game.Game()

cli_msg = {
    "h": lambda: print(messages.help),
    "r": lambda: print(messages.rules),  
    "i": lambda: print(messages.info),
    "b": lambda: print(render_board()),
    "m": lambda: print(GAME.state.moves),
    "u": lambda: print(GAME.state.utility),
}

coordinate_modes = ("linear", "tabular")

cli_options = {
    "crd": "tbl",
    "brd": "tbl"
}

def init_loop():
    print("Welcome to SENET Game")
    cli_msg['h']()

    while True:
        tks = input(">>> ").lower().split(None, 2)
        if len(tks) < 1:
            continue
        cmd = tks[0]
        # terminating loop
        if cmd == "q": #or cmd == "Q":
            if GAME.running:
                conf = input(messages.confirm_q)
                if conf != "Y": #and conf != "y":
                    continue
            break
        # messages request
        if cmd in ['h', 'r', 'i', 'g']:
            cli_msg[cmd]()
            continue
        if cmd in [ 'b', 'm', 'u' ] and GAME.running:
            cli_msg[cmd]()
            continue
        # actions
        if cmd == 's':
            if GAME.running:
                conf = input(messages.confirm_s)
                if conf != "y": #or conf == "y":
                    continue
            start_game(tks)
            continue
        if cmd == 't' and len(tks) > 1 and GAME.running:
            make_move(tks[1:])
            continue
        if cmd == 'o':
            if len(tks) == 1:
                print(messages.options)
                continue
            elif len(tks) > 1:
                if tks[1] == "list":
                    list_options()
                    continue
                elif len(tks) == 3:
                    toggle_option(tks[1:])
                    continue
            
        # default error msg
        print(messages.warn)

def start_game(tokens):
    player = 1
    if len(tokens) > 1:
        player = int(tokens[1])
        if player not in [1,2]:
            player = 1
    GAME.start_game(player)


def make_move(tokens):
    mode = cli_options["crd"]
    cell, steps, r, c = [None for _ in range(4)]
    if mode == "lin" and len(tokens) == 1:
        cell = tokens[0] - 1
    if mode == "tbl" and len(tokens) == 2:
        r, c = int(tokens[0])-1, int(tokens[1])-1
        cell = r * 10 + c
        if r == 1:
            cell = 19 - c
    
    if cell is not None:
        success = GAME.make_move(cell)
        target = cell + GAME.steps
    if success and mode == "lin":
        print(f"player {GAME.player} moved from {cell + 1} to {target + 1}")
    elif success and mode == "tbl":
        tr = target // 10 + 1
        tc = target % 10 + 1
        if tr == 2:
            tc = 11 - tc
        print(f"player {GAME.player} moved from {r+1} {c+1} to {tr} {tc}")
    if success:
        print(render_board())
    else:
        print("choosen movement is impossible")
def toggle_option(tokens):
    option, value = tokens
    if option not in ["brd", "crd"] or value not in ["lin", "tbl"]:
        #raise ValueError("invalid option arguments")
        return
    cli_options[option] = value
    print(f"option {option} set to {value}")
    
def list_options():
    print("GAME OPTIONS STATE:")
    for option in cli_options:
        print(f"{option}: {cli_options[option]}")

def render_board():
    b = GAME.state.board
    board = ""
    def symb(n):
        return {
            0: "_",
            1: "V",
            2: "X"
        }.get(n)

    if cli_options["brd"] == "tbl":
        r1 = [0 for _ in range(10)]
        r2, r3 = r1.copy(), r1.copy()

        for i in range(10):
            c1, c2, c3 = b[i], b[19-i], b[i+20]
            r1[i], r2[i], r3[i] = symb(c1), symb(c2), symb(c3)

        r1, r2, r3 = " ".join(r1), " ".join(r2), " ".join(r3)
        top = " ".join(str(i+1) for i in range(10))
        bottom = " ".join(" " for _ in range(5)) + " a b c d e"
        board = f"""    
        {top}
        {r1} 1
        {r2} 2
        {r3} 3
        {bottom}
        """
    elif cli_options["brd"] == "lin":
        board = " ".join(map(symb, b))
        #line2 = " ".join([str(x + 1) for x in range(30)])
        #line3 = " ".join(" " for _ in range(25)) + " a b c d e"
        #board = board + "\n" + line2 + "\n" + line3 
    s = GAME.status
    team = s['team'] - 1
    stats = f"turn: {s['turn']}, team {['V', 'X'][team]} is moving for {s['steps']} steps"
    return f"""
    {board}
    {stats}
    """

            
