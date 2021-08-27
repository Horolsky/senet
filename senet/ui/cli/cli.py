from senet.core.enums import Unit
from senet.core import Game, Agent
from senet.ai import AIplayer
from .msg_cli import msgout#messages as msg
from senet.utils import singleton, STATS
from senet.settings import SETTINGS
from senet.ui.rules import rules


class cli(metaclass=singleton):
    def __init__(self):
        self.__game = Game(self._on_move, self._on_victory)
        
        self.__autogame = Game(None,None,"senet_autogames")
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
        seed=0
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
        depth1 = SETTINGS.get("agent-x/depth")
        depth2 = SETTINGS.get("agent-y/depth")
        
        eval1 = SETTINGS.get("agent-x/eval")
        eval2 = SETTINGS.get("agent-y/eval")
        emax1 = SETTINGS.get("agent-x/emax")
        emax2 = SETTINGS.get("agent-y/emax")
        coefs1 = SETTINGS.get("agent-x/coefs")
        coefs2 = SETTINGS.get("agent-y/coefs")

        autofirst = SETTINGS.get("game/first")

        agentX = AIplayer(agent=Unit.X, depth=depth1, eval_func=eval1, emax_algo=emax1, coefs=coefs1)
        agentY = AIplayer(agent=Unit.Y, depth=depth2, eval_func=eval2, emax_algo=emax2, coefs=coefs2)

        self.msgout(f"launching {repeats} autogames")        
        for i in range(repeats):    
            self.__autogame.start(agentX, agentY, autofirst, seed)
            if (i+1) % 10 == 0:
                self.msgout(f"{i+1} games over")  
        self.msgout(f"{repeats} autogames completed")        
        STATS.show_brief()  
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
        first = Unit.X
        seed = 0 #def seed for game start
        if len(tokens) == 4:
            try:
                option = int(tokens[3])
                if option in [1,2]:
                    first = Unit(option-1)
                elif Game.check_seed(option):
                    seed = option 
                else:
                    return False
            except:
                return False

        agents = {
            "human": lambda an: Agent(agent=an, dfunc=self.ask_human, name="human"),
            "ai": lambda an: AIplayer(
                agent=an,
                depth=SETTINGS.get(f"agent-{repr(an)}/depth"),
                eval_func=SETTINGS.get(f"agent-{repr(an)}/eval"),
                emax_algo=SETTINGS.get(f"agent-{repr(an)}/emax")
                )
            }
        if tokens[1] not in agents or tokens[2] not in agents:
            return False
        agentX = agents.get(tokens[1])(Unit.X)
        agentY = agents.get(tokens[2])(Unit.Y)
        self.msgout("\tGAME STARTED")
        self.game.start(agentX, agentY, first, seed)
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
                msgout("GAME STATS")
                msgout("A: Agent, F: first move, T: timer, D: depth, E: eval func, C: coefs, ~: opponents params\n")
                STATS.show_brief()
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
                if m in self.game.ply.strategies.indici:
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
        self.msgout(f"VICTORY! Player {repr(agent)} won the game")
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
            "m": lambda: msgout(self.game.ply.strategies.indici),
            "u": lambda: msgout("utility: " + str(self.game.ply.utility))
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
        b = self.game.ply.board
        board = ""

        brd = SETTINGS.get("cli/brd")
        if brd == "tbl":
            r1 = [0 for _ in range(10)]
            r2, r3 = r1.copy(), r1.copy()
            for i in range(10):
                c1, c2, c3 = b[i], b[19-i], b[i+20]
                r1[i], r2[i], r3[i] = repr(c1), repr(c2), repr(c3)
            r1, r2, r3 = " ".join(r1), " ".join(r2), " ".join(r3)
            top = " ".join(str(i+1) for i in range(10))
            bottom = " ".join(" " for _ in range(5)) + " a b c d e"
            board = f"\t{top}\n\t{r1} 1\n\t{r2} 2\n\t{r3} 3\n\t{bottom}\n"
        elif brd == "lin":
            board = " ".join(map(repr, b)) 
        #EVENT MSG
        agent =  self.game.ply.agent
        event_msg = ""
        if self.game.turn == 0:
            event_msg = "game starts"
        else:
            event_msg = repr(self.game.ply.event)
        #STATS MSG
        stats = f"\tdice rollout gives for {repr(agent)} {self.game.ply.steps} steps\n"
        strats = self.game.ply.strategies
        stats += "\tpossible strategies:\n\t" + ", ".join([f"{self.get_pos(strats.indici[i])}: {repr(strats.actions[i])}" for i in range(strats.mobility) ])
        
        # stats += f"\tbench: V - {self.game.ply.bench[0]}, X - {self.game.ply.bench[1]}"
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
