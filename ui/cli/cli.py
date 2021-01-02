from core import game
from agent import agent
from .msg_cli import messages as msg
from ui.rules import rules

msgout = {
    "h": lambda: print(msg["help"]),
    "r": lambda: print(rules),
    "i": lambda: print(msg["info"]),
    "b": lambda: print(render_board()),
    "m": lambda: print(self.game.state.moves),
    "u": lambda: print(self.game.state.utility)
}

dummy_player_f = agent("first")
dummy_player_l = agent("last")
dummy_player_r = agent("random")

class cli:
    def __init__(self):
        self.__game = game()
        self.__options = { "crd": "tbl", "brd": "tbl" }

    @property
    def game(self):
        return self.__game
    @property
    def options(self):
        return self.__options
    
    def init(self):
        print("Welcome to SENET Game")
        msgout['h']()
        while True:
            tks = input("senet: ").lower().split(None, 2)
            if len(tks) < 1:
                continue
            cmd = tks[0]
            # terminating loop
            if cmd == "q": #or cmd == "Q":
                if self.game.running:
                    conf = input(msg["confirm_q"])
                    if conf != "Y" and conf != "y":
                        continue
                break
            # msg request
            if cmd in ['h', 'r', 'i', 'g']:
                msgout[cmd]()
                continue
            if cmd in [ 'b', 'm', 'u' ] and self.game.running:
                msgout[cmd]()
                continue
            # actions
            if cmd == 's':
                if self.game.running:
                    conf = input(msg["confirm_s"]).lower()
                    if conf != "y": #or conf == "y":
                        continue
                self.start(tks)
                continue
            if cmd == 't' and len(tks) > 1 and self.game.running:
                self.make_move(self.get_index(tks[1:]))
                continue
            if cmd == 'o':
                if len(tks) == 1:
                    print(msg["options"])
                    continue
                elif len(tks) > 1:
                    if tks[1] == "list":
                        self.print_options()
                        continue
                    elif len(tks) == 3:
                        self.toggle_option(tks[1:])
                        continue
            if cmd == 'auto':
                if not self.game.running:
                    continue
                while self.game.running:   
                    move = 0
                    dummy = [dummy_player_r, dummy_player_f][self.game.state.agent - 1]
                    if len(self.game.state.moves) > 0:
                        move = dummy.choose_movement(self.game.state)
                    self.make_move(move)
                continue
            # default error msg
            print(msg["warn"])

    def start(self, tokens):
        firstplayer = 1
        if len(tokens) > 1:
            firstplayer = int(tokens[1])
            if firstplayer not in [1,2]:
                firstplayer = 1
        self.game.start_game(firstplayer)
        print("\tGAME STARTED")
        print(self.render_board())

    def make_move(self, cell):
        success = self.game.manage_movement(cell)
        if success:
            print(self.render_board())
            #victory condition
            if 5 in self.game.state.bench:
                print(f"player {('V', 'X')[self.game.state.event[0]-1]} won the game")
                print("press S to start new game")
                self.game.stop_game()
            #skipping move
            elif self.game.state.mobility == 0:
                success = self.game.manage_movement(0)
        else:
            print("choosen movement is impossible")

    def toggle_option(self, tokens):
        option, value = tokens
        if option not in ["brd", "crd"] or value not in ["lin", "tbl"]:
            raise ValueError("invalid option arguments")
        self.options[option] = value
        print(f"option {option} set to {value}")
        
    def print_options(self):
        print("self.game OPTIONS STATE:")
        for option in self.options:
            print(f"{option}: {self.options[option]}")

    def render_board(self):
        b = self.game.state.board
        board = ""
        def symb(n):
            return {
                0: "_",
                1: "V",
                2: "X"
            }.get(n)

        if self.options["brd"] == "tbl":
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
        elif self.options["brd"] == "lin":
            board = " ".join(map(symb, b)) 
        #EVENT MSG
        agent =  ('V', 'X')[self.game.state.agent-1]
        event_msg = ""
        if self.game.turn == 0:
            event_msg = "game starts"
        else:
            code = self.game.state.event[1]
            previous = ('V', 'X')[self.game.state.event[0]-1]
            start, destination, victim_destination = map(self.get_pos, self.game.state.event[2:])
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
            1: f"possible moves: {', '.join(map(self.get_pos, self.game.state.moves))}",
            -1: f"player cannot move forward, possible reverse moves: {', '.join(map(self.get_pos, self.game.state.moves))}",
            0: "no avaliable moves, player skips the turn"
            }.get(self.game.state.mobility)
        
        stats = f"""
        {event_msg}
        bench: V - {self.game.state.bench[0]}, X - {self.game.state.bench[1]}
        turn: {self.game.turn}, player {agent} is moving for {self.game.state.steps} steps
        {moves_msg}
        """
        return f"{board}\n{stats}"

    def get_pos(self, index):#FIXME
        mode = self.options["crd"]
        if mode == "lin":
            return str(index + 1)
        elif mode == "tbl":
            r = index // 10 + 1
            c = index % 10 + 1
            if r == 2:
                c = 11 - c
            return f"({r}, {c})"
        
    def get_index(self, tokens):
        cell, steps, r, c = [None for _ in range(4)]
        mode = self.options["crd"]
        if mode == "lin" and len(tokens) == 1:
            cell = int(tokens[0]) - 1
        if mode == "tbl" and len(tokens) == 2:
            r, c = int(tokens[0])-1, int(tokens[1])-1
            cell = r * 10 + c
            if r == 1:
                cell = 19 - c
        return cell
