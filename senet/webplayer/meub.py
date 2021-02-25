from selenium import webdriver
from senet.utils import singleton, Report
from senet.xtc import emax, Ply
from time import sleep

BROWSERS = {
    "Chrome": webdriver.Chrome,
    "Firefox": webdriver.Firefox
}
launch_browser = lambda b: BROWSERS.get(b)()
MEUB_URL = "http://chrismeub.com/projects/senet.html"
MEUB_URL_LOCAL = "/home/alexander/projects/chrismeub/senet.html"
CELL_SELECTOR_T = "body > div > div.content_wrapper > div > div.game_wrapper > div > div.board.unselectable > a:nth-child"
RESTART_BTN_SELECTOR = "body > div > div.content_wrapper > div > div.game_wrapper > div > div.controls > a"
MEUB_SCRIPT_PATH = "senet/webplayer/js/meub_init.js"

SCRIPT_WAIT_FOR_CTRL = """
  var callback = arguments[arguments.length - 1];
  (function wait_for_ctrl(){
    if(!isNaN(game_obj.state_index) && game_obj.state_index != 1) return game_obj.state_index;
    setTimeout(wait_for_ctrl, 60);
  })();
"""
SCRIPT_GET_MATCHES = " return [game_obj.wins, game_obj.losses]; "

class MeubPlayer(metaclass=singleton):
    __cells = None
    __browser_id = None
    _browser = None
    _game = None
    _report = None
    gamedata = []
    matches = [0,0] #wins/losses
    @staticmethod
    def get_cell_selector(id_):
        """
        returns css celector of clickable DOM object,
        representing the board cell.
        @param id: 0-based index of cell in senet board 
        """
        cell_id = id_ + 1
        if 9 < id_ < 20:
            cell_id = 31 - id_
        elif id_ > 19:
            cell_id = id_ + 3 
        return f"{CELL_SELECTOR_T}({cell_id})"
    @staticmethod
    def convert_bb_to_board(bb):
        """
        @param bb: 2d array, representing the  Meub's board 
        return 1d list of 0-indexed board
        """
        bb[1].reverse()
        bb.reverse()
        return [cell for row in bb for cell in row]

    def __init__(self, report=False):
        pass
        #if report:
        #    headers = "seed;move"
        #    self._report = Report("webplayer-meub", None, "csv", "logs/meub", headers, False)
    def launch(self, browser="Chrome", local=False):
        if self._browser != None:
            print("browser is running")
            return
        self.__browser_id = browser
        try:
            self._browser = launch_browser(self.__browser_id)
            url = MEUB_URL
            if local:
                url = MEUB_URL_LOCAL
            self._browser.get(url)
            self._browser.set_script_timeout(30)

            jscript = open(MEUB_SCRIPT_PATH, "r")
            _s = jscript.read()
            self._browser.execute_script(_s)
            jscript.close()

            self.matches = self._browser.execute_script(SCRIPT_GET_MATCHES)
        except:
            print("unable to launch browser")
    
    def quit(self):
        if self._browser != None:
            self._browser.quit()
            self._browser = None
    
    def __del__(self):
        #self._report.close()
        self.quit()

    def play(self):
        if self._browser == None:
            return
        
        while self.game_state['state_index'] < 2:
            #sleep(1)
            seed = self.state.seed
            moves = self.state.moves
            strategy = emax(seed, 6, 4, "Meub")[0]
            if strategy >= len(moves):
                continue
            move = moves[strategy]
            print(f"ai plays {move}")
            self.cells[move].click()
            while self.game_state['state_index'] == 1:
                sleep(0.2)
            
            
            
        if self.game_state['state_index'] == 2:
            print("Victory is ours!")
        elif self.game_state['state_index'] == 3:
            print("You loose!")
    
    def restart(self):
        if self._browser != None:
            self.__cells = None
            self._browser.find_element_by_css_selector(RESTART_BTN_SELECTOR).click()

    @property
    def cells(self):
        """
        DOM elements of game board cells
        """
        if self.__cells is None and self._browser != None:
            selector = lambda i: MeubPlayer.get_cell_selector(i)
            element = lambda i: self._browser.find_element_by_css_selector(selector(i))
            self.__cells = tuple([element(i) for i in range(30)]) 
        return self.__cells

    @property
    def bb(self):
        if self._browser != None:
            return self._browser.execute_script("return game_obj.bb;")
    @property
    def board(self):
        if self._browser != None:
            return MeubPlayer.convert_bb_to_board(self.bb)
    @property
    def steps(self):
        if self._browser != None:
            return self._browser.execute_script("return game_obj.dice_value;")
    @property
    def agent(self):
        if self._browser != None:
            return (int(self._browser.execute_script("return game_obj.is_player_turn;")) ^ 1) + 1

    @property
    def state(self):
        if self._browser != None:
            state = Ply(0, "Meub")
            state.board = self.board
            state.agent = self.agent
            state.steps = self.steps
            return state

    @property
    def game_state(self):
        if self._browser != None:
            return self._browser.execute_script("return _get_state();")

    def play_best(self):
        strategy = emax(self.state.seed, 6, 4, "Meub")[0]
        move = self.state.moves[strategy]
        print(f"ai plays {move}")
        self.cells[move].click()


meubplayer = MeubPlayer() # MeubPlayer singletone