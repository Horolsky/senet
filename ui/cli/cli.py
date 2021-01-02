from core import game, state, agent
from .msg_cli import msgout#messages as msg
from ui.rules import rules

dummy_player_f = agent("first")
dummy_player_l = agent("last")
dummy_player_r = agent("random")


class cli:
    def __init__(self):
        self.__game = game(self._on_move, self._on_victory)
        self.__options = { "crd": "tbl", "brd": "tbl" }

    @property
    def game(self):
        return self.__game
   
    @property
    def options(self):
        return self.__options
    
    def init(self):
        print("Welcome to SENET Game")
        self.msgout('h')
        self.ask_human()

    def start(self, tokens):
        """
        @param tokens: list
        0: <cmd>    "s"
        1: <agent1> "human", "ai", "dummy"
        2: <agent2> "human", "ai", "dummy"
        3: <first>  1, 2
        """
        if type(tokens) is not list or len(tokens) < 3:
            return
        #agents
        agents = {
            "human": agent(self.ask_human),
            "ai": dummy_player_r, #TODO
            "dummy": dummy_player_r 
            }
        agent1, agent2 = tokens[1:3]
        if type(agent1) is str:
            agent1 = agents.get(agent1)
        if type(agent2) is str:
            agent2 = agents.get(agent2)
        if not (type(agent1) is type(agent2) is agent):
            return
        #default first player pawns
        first = 1
        if len(tokens) == 4:
            first = int(tokens[3])
            if first not in [1,2]:
                first = 1
        self.msgout("\tGAME STARTED")
        #self.msgout(self.stringify_board())
        self.game.start(agent1, agent2, first)
    
    def ask_human(self, state=0):
        """
        @return: int
        manage cli io-loop
        if cmd is turn and game is running,
        return cell number
        """
        movement = -1 #default invalid move
        while True:
            tks = input("human: ").lower().split(None, 2)
            if len(tks) < 1:
                continue
            cmd = tks[0]
            # terminating loop
            if cmd == "q": #or cmd == "Q":
                if self.game.running:
                    self.msgout("confirm_q")
                    conf = input("human: ")
                    if conf != "Y" and conf != "y":
                        continue
                break
            # msg request
            if cmd in ['h', 'r', 'i', 'g']:
                self.msgout(cmd)
                continue
            if cmd in [ 'b', 'm', 'u' ] and self.game.running:
                self.msgout(cmd)
                continue
            if cmd == 'o':
                if len(tks) == 1:
                    self.msgout("options")
                    continue
                elif len(tks) > 1:
                    if tks[1] == "list":
                        self.print_options()
                        continue
                    elif len(tks) == 3:
                        self.toggle_option(tks[1:])
                        continue
            
            if cmd == 'auto':
                if self.game.running:
                    self.msgout("to start autoplay terminate current game")
                else:
                    self.start(["s", "dummy", "dummy", 1])
                    #break
                continue
            # actions
            if cmd == 's':
                if self.game.running:
                    self.msgout("confirm_s")
                    conf = input("human: ").lower()
                    if conf == "y": 
                        self.game.stop()
                else:
                    self.start(tks)
                #break
            if cmd[0] in "0123456789" and self.game.running:
                m = self.get_index(tks)
                if m in self.game.state.moves:
                    movement = m
                    break
                else:
                    self.msgout("choosen movement is impossible")
                    continue
        return movement

    def _on_victory(self, agent):
        """
        @param agent: int (1, 2)
        """
        self.msgout(f"player {('V', 'X')[agent-1]} won the game")
        self.msgout("press S to start new game")
    
    def _on_move(self):
        """
        @param agent: int (1, 2)
        """
        self.msgout("b")

    def msgout(self, msg):
        reqmsg = {
            "h": lambda: msgout("help"),
            "r": lambda: msgout(rules),
            "i": lambda: msgout("info"),
            "b": lambda: msgout(self.stringify_board()),
            "m": lambda: msgout(self.game.state.moves),
            "u": lambda: msgout(self.game.state.utility)
            }
        if msg in reqmsg:
            reqmsg[msg]()
        else:
            msgout(msg)
    
    def toggle_option(self, tokens):
        option, value = tokens
        if option not in ["brd", "crd"] or value not in ["lin", "tbl"]:
            raise ValueError("invalid option arguments")
        self.options[option] = value
        self.msgout(f"option {option} set to {value}")
        
    def print_options(self):
        self.msgout("OPTIONS STATE:")
        for option in self.options:
            self.msgout(f"{option}: {self.options[option]}")

    def stringify_board(self):
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
        player {agent} is moving for {self.game.state.steps} steps
        {moves_msg}
        bench: V - {self.game.state.bench[0]}, X - {self.game.state.bench[1]}
        """
        return f"turn: {self.game.turn}, {event_msg}\n{board}\n{stats}"

    def get_pos(self, index):
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
