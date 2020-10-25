import messages, game
GAME = game.Game()
def init_loop():
    print("Welcome to SENET Game")
    actions('h')()
    while True:
        tok = input()
        if tok == "q" or tok == "Q":
            if GAME.is_running:
                conf = input(messages.confirm_q)
                if conf != "Y" or conf != "y":
                    print('continue')
                else:
                    break
            else:
                break
        else:
            tok = tok.split()
            if tok[0].isdigit() and tok[1].isdigit():
                if int(tok[0]) < 4 and int(tok[1]) < 11:
                    actions('turn')(tok)
            elif tok[0] == 's':
                actions('s')(tok[1:])
            else:
                actions(tok[0])()
def actions(t):
    if t == 's' and GAME.is_running:
        conf = input(messages.confirm_s)
        if conf != "Y" or conf != "y":
            return lambda: print('continue')
         
    a = {
        "h": lambda: print(messages.help),
        "r": lambda: print(messages.rules),  
        "i": lambda: print(messages.info),
        "s": lambda player: GAME.start_game(player),
        "g": lambda: print(GAME.status),
        "turn": lambda tokens: make_move(tokens),
        "t": lambda: print(render_table())
    }
    if t in a:
        return a.get(t)
    else:
        return lambda: print("invalid token")

def make_move(tokens):
    r, c = int(tokens[0])-1, int(tokens[1])-1
    cell = r * 10 + c
    if r == 1:
        cell = 19 - c
    success = GAME.make_move(cell)
    print(f"movement successful: {success}")
def render_table():
    t = GAME.table
    r1 = [0 for _ in range(10)]
    r2, r3 = r1.copy(), r1.copy()
    def symb(s):
        return {
            0: "_",
            1: "V",
            2: "X"
        }.get(s)
    for i in range(10):
        c1, c2, c3 = t[i], t[19-i], t[i+20]
        r1[i], r2[i], r3[i] = symb(c1), symb(c2), symb(c3)

    r1, r2, r3 = " ".join(r1), " ".join(r2), " ".join(r3)
    top = " ".join(str(i+1) for i in range(10))
    bottom = " ".join(" " for _ in range(5)) + " a b c d e"
    return f"""    
    {top}

    {r1}  1
    {r2}  2
    {r3}  3
    {bottom}
    """