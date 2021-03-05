from senet.core import Game, Agent
from senet.ai import AIplayer
from .msg_cli import msgout#messages as msg
from senet.ui.rules import rules
from senet.utils import singleton, STATS
from senet.settings import SETTINGS


class cli(metaclass=singleton):
    def __init__(self):
        self.__game = Game(self._on_move, self._on_victory)
        
        self.__autogame = Game(None,None)
        self.__running = True

    @property
    def game(self):
        """
        instance of a core.game class
        """
        return self.__game
    
    def init(self):
        self.msgout('h')
        self.__running = True
        self.ask_human()
        
    def autoplay(self, tokens):
        """
        ai vs ai autoplayed game
        ai depth are taken from the settings 
        """
        seed=10066320
        repeats = 1
        if len(tokens) > 1:
            try:
                newseed = int(tokens[1])
                if Game.check_seed(newseed):
                    seed = newseed 
                if len(tokens) == 3:
                    repeats = int(tokens[2])
            except:
                return False
        depth1 = SETTINGS.get("agent-V/depth")
        depth2 = SETTINGS.get("agent-X/depth")
        
        eval1 = SETTINGS.get("agent-V/eval")
        eval2 = SETTINGS.get("agent-X/eval")
        coefs1 = SETTINGS.get("agent-V/coefs")
        coefs2 = SETTINGS.get("agent-X/coefs")

        rules = SETTINGS.get("game/rules")
        autofirst = SETTINGS.get("game/first")

        agent1 = AIplayer(number=1, depth=depth1, rules=rules, eval_func=eval1, coefs=coefs1)
        agent2 = AIplayer(number=2, depth=depth2, rules=rules, eval_func=eval2, coefs=coefs2)

        self.msgout(f"\tLAUNCHING {repeats} AUTOGAMES")        
        for i in range(repeats):    
            self.__autogame.start(agent1, agent2, rules, autofirst, seed)
            if (i+1) % 10 == 0:
                self.msgout(f"{i+1} games over")        
        return True

    def start(self, tokens):
        """
        @param tokens: list
        0: <cmd>        "s"
        1: <player 1>   "human", "ai"
        2: <player 2>   "human", "ai"
        3: <option>      starting player (1, 2) or game seed
        """
        if type(tokens) is not list or len(tokens) < 3:
            return False
        #default first player pawns
        first, seed = 1, 10066320 #def seed for game start
        if len(tokens) == 4:
            option = int(tokens[3])
            if option in [1,2]:
                first = 1
            elif Game.check_seed(option):
                seed = option 
            else:
                return False

        rules = SETTINGS.get("game/rules")
        symb = ('V','X')
        agents = {
            "human": lambda an: Agent(number=an+1, dfunc=self.ask_human, name="human"),
            "ai": lambda an: AIplayer(number=an+1, depth=SETTINGS.get(f"agent-{symb[an]}/depth"), rules=rules, eval_func=SETTINGS.get(f"agent-{symb[an]}/eval"))
            }
        if tokens[1] not in agents or tokens[2] not in agents:
            return False
        agent1 = agents.get(tokens[1])(0)
        agent2 = agents.get(tokens[2])(1)
        self.msgout("\tGAME STARTED")
        self.game.start(agent1, agent2, rules, first, seed)
        return True
    
    def ask_human(self, state=0):
        """
        manage cli io-loop
        also serves as dfunc for human controlled core.agent instance
        if cmd is turn and game is running,
        return integer cell number
        """
        movement = -1 #default invalid move

        while self.__running:
            tks = input("human: ").lower().split()
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
                seed = self.game.stop()
                self.msgout(f"game stopped, seed: {seed}")
                self.msgout("shut down")
                self.__running = False
            # msg request
            if cmd in ['h', 'r', 'i']:
                self.msgout(cmd)
                continue
            if cmd in [ 'b', 'm', 'u' ] and self.game.running:
                self.msgout(cmd)
                continue
            if cmd == 'o':
                if len(tks) == 1:
                    self.print_options()
                elif len(tks) > 1:
                    self.toggle_option(tks[1:])
                continue
            if cmd == "stats":
                msgout("rendering report in the web browser...\n")
                STATS.show_brief_in_browser()
            #game actions
            if cmd == 'a':
                if self.game.running:
                    self.msgout("to start autoplay break the current game")
                else:
                    success = self.autoplay(tks)
                    if not success:
                        self.msgout("warn")
                    #break
                continue
            if cmd == 's':
                if self.game.running:
                    self.msgout("confirm_s")
                    conf = input("\nhuman: ").lower()
                    if conf == "y": 
                        seed = self.game.stop()
                        self.msgout(f"game stopped, seed: {seed}")
                else:
                    success = self.start(tks)
                    if not success:
                        self.msgout("warn")
                #break
            if cmd[0] in "0123456789" and self.game.running:
                m = self.get_index(tks)
                if m in self.game.state.moves:
                    movement = m
                    break # return movement for in-game loop
                else:
                    self.msgout("choosen movement is impossible")
                    continue
        return movement

    def _on_victory(self, agent):
        """
        callback for core.game victory event
        @param agent: int (1, 2)
        """
        self.msgout(f"VICTORY! Player {('V', 'X')[agent-1]} won the game")
        self.msgout("press S to start new game")
    
    def _on_move(self):
        """
        callback for core.game turn event
        @param agent: int (1, 2)
        """
        self.msgout("b")

    def msgout(self, msg):
        """
        alias for ui.cli.msg 
        """
        reqmsg = {
            "h": lambda: msgout("help"),
            "r": lambda: msgout(rules),
            "i": lambda: msgout("info"),
            "b": lambda: msgout(self.stringify_board()),
            "m": lambda: msgout(self.game.state.moves),
            "u": lambda: msgout("utility: " + str(self.game.state.utility))
            }
        if msg in reqmsg:
            reqmsg[msg]()
        else:
            msgout(msg)
    
    def toggle_option(self, tokens):
        option = tokens[0]
        value = tokens[1:]
        success = SETTINGS.set(option, value)
        if success:
            self.msgout(f"{option} has been set to {SETTINGS.get(option)}")
        else:
            self.msgout("warn")
        
    def print_options(self):
        settings = SETTINGS.get("all")
        msg = f"\n  {'='*55} SETTINGS {'='*55}\n"
        msg += f"\n  KEY{' '*13}VALUE{' '*10}OPTIONS{' '*30}DESCRIPTION\n" 
        msg += f"  {'='*120}\n\n"
        for group in settings:
            msg += f"  {group.upper()}:\n  {'`'*120}\n" #group separator

            for option in settings[group]:
                stype = settings[group][option]['type']
                value = settings[group][option]['value']
                if stype == "list":
                    value = ", ".join([str(k) for k in value])
                options = settings[group][option]['options']
                options = ", ".join([str(o) for o in options])
                descr = settings[group][option]['descr']
                dlines = descr[0] + "\n"
                for line in descr[1:]:
                    dlines += " " * 70 + line + "\n"
                ws1 = " " * (15 - len(option))#whitespase
                ws2 = " " * (15 - len(str(value)))#whitespase
                ws3 = " " * (37 - len(str(options)))#whitespase
                msg += f"  {option}:{ws1}{value}{ws2}{options}{ws3}{dlines}\n"
        msg += f"  {'='*120}\n"
        self.msgout(msg)

    def stringify_board(self):
        """
        return stringified representation of a current game board, event info and stats
        """
        b = self.game.state.board
        board = ""
        def symb(n):
            return {
                0: "_",
                1: "V",
                2: "X"
            }.get(n)

        brd = SETTINGS.get("cli/brd")
        if brd == "tbl":
            r1 = [0 for _ in range(10)]
            r2, r3 = r1.copy(), r1.copy()
            for i in range(10):
                c1, c2, c3 = b[i], b[19-i], b[i+20]
                r1[i], r2[i], r3[i] = symb(c1), symb(c2), symb(c3)
            r1, r2, r3 = " ".join(r1), " ".join(r2), " ".join(r3)
            top = " ".join(str(i+1) for i in range(10))
            bottom = " ".join(" " for _ in range(5)) + " a b c d e"
            board = f"\t{top}\n\t{r1} 1\n\t{r2} 2\n\t{r3} 3\n\t{bottom}\n"
        elif brd == "lin":
            board = " ".join(map(symb, b)) 
        #EVENT MSG
        agent =  ('V', 'X')[self.game.state.agent-1]
        event_msg = ""
        if self.game.turn == 0:
            event_msg = "game starts"
        else:
            code = self.game.state.event[2]
            previous = ('V', 'X')[self.game.state.event[1]-1]
            start, destination = map(self.get_pos, self.game.state.event[3:])
            event_msg = {
                0: f"{previous} drawed in House of Waters",
                1: f"{previous} reversed from {start}",
                2: f"{previous} skipped the turn",
                3: f"{previous} moved from {start} to {destination}",
                4: f"{previous} from {start} attacked enemy on {destination}",
                5: f"{previous} from {start} has successfully escaped the board",
            }.get(code)
        #STATS MSG
        stats = f"\tplayer {agent} is moving for {self.game.state.steps} steps\n"
        stats += {
            1: f"\tpossible moves: {', '.join(map(self.get_pos, self.game.state.moves))}\n",
            -1: f"\tplayer cannot move forward, possible reverse moves: {', '.join(map(self.get_pos, self.game.state.moves))}\n",
            0: "\tno avaliable moves, player skips the turn\n"
            }.get(self.game.state.mobility)
        
        stats += f"\tbench: V - {self.game.state.bench[0]}, X - {self.game.state.bench[1]}"
        head = f"turn {self.game.turn}, {event_msg}"
        emb = "=" * (80 - len(head))
        return f"{head} {emb}\n\n{board}\n{stats}"

    def get_pos(self, index):
        """
        convert given input index to 0-based index
        depending on coordinate options
        """
        mode = SETTINGS.get("cli/crd")
        result = ""
        if mode == "lin":
            result = str(index + 1)
            if index == 30:
                result = "0"
        elif mode == "tbl":
            r = index // 10 + 1
            c = index % 10 + 1
            if r == 2:
                c = 11 - c
            result = f"({r}, {c})"
            if index == 30:
                result = "(0)"
        return result
        
    def get_index(self, tokens):
        """
        convert given 0-based index to output index
        depending on coordinate options
        """
        cell, steps, r, c = [None for _ in range(4)]
        mode = SETTINGS.get("cli/crd")
        if tokens[0] == "0":
            cell = 30
        elif mode == "lin" and len(tokens) == 1:
            cell = int(tokens[0]) - 1
        elif mode == "tbl" and len(tokens) == 2:
            r, c = int(tokens[0])-1, int(tokens[1])-1
            cell = r * 10 + c
            if r == 1:
                cell = 19 - c
        return cell
