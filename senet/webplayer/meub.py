from selenium import webdriver
from senet.utils import singleton, Report
from senet.xtc import emax, Ply

BROWSERS = {
    "Chrome": webdriver.Chrome,
    "Firefox": webdriver.Firefox
}
launch_browser = lambda b: BROWSERS.get(b)()
MEUB_URL = "http://chrismeub.com/projects/senet.html"
MEUB_URL_LOCAL = "/home/alexander/projects/chrismeub/senet.html"
CELL_SELECTOR_T = "body > div > div.content_wrapper > div > div.game_wrapper > div > div.board.unselectable > a:nth-child"

#PROXY_SCRIPT = """
#const handler = {
#  get: function(target, prop, receiver) {
#    if (prop === "proxied") {
#      return "replaced value";
#    }
#    return Reflect.get(...arguments);
#  }
#};
#"""

class MeubPlayer(metaclass=singleton):
    __cells = None
    __browser_id = None
    _browser = None
    _game = None
    _report = None
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
        if report:
            headers = "timestamp;ai;winner;score"
            self._report = Report("webplayer-meub", None, "csv", "logs/meub", headers, False)
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
        except:
            print("unable to launch browser")
    def quit(self):
        if self._browser != None:
            self._browser.quit()
            self._browser = None
    def __del__(self):
        self.quit()
    
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

    def play_best(self):
        t = Ply()
        t.board = self.board
        t.steps = self.steps
        t.agent = 1
        strategy = emax(t.seed, 6, 4, "Meub")[0]
        move = t.moves[strategy]
        print(f"ai plays {move}")
        self.cells[move].click()


meubplayer = MeubPlayer() # MeubPlayer singletone