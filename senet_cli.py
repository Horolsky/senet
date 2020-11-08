import messages, gamemanager
from player import ArtificialPlayer as AI
GAME = gamemanager.Game()

cli_msg = {
    "h": lambda: print(messages.help),
    "r": lambda: print(messages.rules),
    "i": lambda: print(messages.info),
    "b": lambda: print(render_board()),
    "m": lambda: print(GAME.state.moves),
    "u": lambda: print(GAME.state.utility)
}

cli_options = {
    "crd": "tbl",
    "brd": "tbl"
}
dummy_player_f = AI("first")
dummy_player_l = AI("last")
dummy_player_r = AI("random")



def get_pos(index):
    mode = cli_options["crd"]
    if mode == "lin":
        return str(index + 1)
    elif mode == "tbl":
        r = index // 10 + 1
        c = index % 10 + 1
        if r == 2:
            c = 11 - c
        return f"({r}, {c})"
    
def get_index(tokens):
    cell, steps, r, c = [None for _ in range(4)]
    mode = cli_options["crd"]
    if mode == "lin" and len(tokens) == 1:
        cell = int(tokens[0]) - 1
    if mode == "tbl" and len(tokens) == 2:
        r, c = int(tokens[0])-1, int(tokens[1])-1
        cell = r * 10 + c
        if r == 1:
            cell = 19 - c
    return cell


def init_loop():
    print("""
//////////////////////////////    
    Welcome to SENET Game
//////////////////////////////
    """)
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
                if conf != "Y" and conf != "y":
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
            make_move(get_index(tks[1:]))
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
        if cmd == 'auto':
            if not GAME.running:
                continue
            while GAME.running:   
                move = 0
                dummy = [dummy_player_r, dummy_player_f][GAME.state.agent - 1]
                if len(GAME.state.moves) > 0:
                    move = dummy.choose_movement(GAME.state)
                make_move(move)
            continue
        # default error msg
        print(messages.warn)

def start_game(tokens):
    firstplayer = 1
    if len(tokens) > 1:
        firstplayer = int(tokens[1])
        if firstplayer not in [1,2]:
            firstplayer = 1
    GAME.start_game(firstplayer)
    print("\tGAME STARTED")
    print(render_board())


def make_move(cell):
    success = GAME.manage_movement(cell)
    if success:
        print(render_board())
        #victory condition
        if 5 in GAME.state.bench:
            print(f"player {('V', 'X')[GAME.state.event[0]-1]} won the game")
            print("press S to start new game")
            GAME.stop_game()
        #skipping move
        elif GAME.state.mobility == 0:
            success = GAME.manage_movement(0)
    else:
        print("choosen movement is impossible")
def toggle_option(tokens):
    option, value = tokens
    if option not in ["brd", "crd"] or value not in ["lin", "tbl"]:
        raise ValueError("invalid option arguments")
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
    #EVENT MSG
    agent =  ('V', 'X')[GAME.state.agent-1]
    event_msg = ""
    if GAME.turn == 0:
        event_msg = "game starts"
    else:
        code = GAME.state.event[1]
        previous = ('V', 'X')[GAME.state.event[0]-1]
        start, destination, victim_destination = map(get_pos, GAME.state.event[2:])
        event_msg = {
            0: f"{previous} skipped the turn",
            1: f"{previous} reversed from {start} to {destination}",
            2: f"{previous} moved from {start} to {destination}",
            3: f"{previous} from {start} attacked enemy on {destination}",
            4: f"{previous} drawed in House of Waters and reborned on {destination}",
            5: f"{previous} from {start} attacked enemy on {destination}. His victim rebourned on {victim_destination}",
            6: f"{previous} from {start} has successfully escaped the board",
        }.get(code)
    #MOVES MSG
    moves_msg = {
        1: f"possible moves: {', '.join(map(get_pos, GAME.state.moves))}",
        -1: f"player cannot move forward, possible reverse moves: {', '.join(map(get_pos, GAME.state.moves))}",
        0: "no avaliable moves, player skips the turn"
        }.get(GAME.state.mobility)
    
    stats = f"""
    {event_msg}
    bench: V - {GAME.state.bench[0]}, X - {GAME.state.bench[1]}
    turn: {GAME.turn}, player {agent} is moving for {GAME.state.steps} steps
    {moves_msg}
    """
    return f"{board}\n{stats}"

if __name__ == "__main__":
    init_loop()