import pandas as pd
import webbrowser
from tempfile import NamedTemporaryFile
from .singleton import singleton
from os import listdir
from os.path import isfile, join


stats_header = "game cumulative statistics"

class Stats(metaclass=singleton):
    _src = {
        "brief": [],
        "games": [],
        "ai": [],
        "meub": []
    }
    _df = {
        "brief": None,
        "games": None,
        "ai": None,
        "meub": None
    }
    def __init__(self):
        pass
        
    def update_src(self):
        for dr in self._src:
            self._src[dr] = [join(f"logs/{dr}", f) for f in listdir(f"logs/{dr}") if isfile(join(f"logs/{dr}", f))]

    def update_brief(self):
        if type(self._src["brief"]) not in [list, tuple]:
            return False
        frames = []
        for filepath in self._src["brief"]:
            frames.append(pd.read_csv(filepath, sep=";", index_col=None))
        self._df["brief"] = pd.concat(frames)
        self._df["brief"].reset_index(drop=True, inplace=True)
        return True
    
    def parse_brief(self):
        if self._df["brief"] is None:
            return False
        src = self._df["brief"]
        trg = pd.DataFrame(columns=["V\X"])#columns=["params", "position", "plays", "wins", "efficiency", "score"]
        L_super = src.groupby(["agent 1", "depth 1", "eval 1", "coefs 1"])
        def grp_to_id(g):
            if g[0] in ["dummy", "human"]:
                return g[0]
            _coefs = g[3][1:-1].split(', ')
            coefs = []
            for c in _coefs:
                coefs.append((" "* (3-len(c))) + c)
            return f"{g[0]}-{g[1]}-{g[2][0].upper()}[{','.join(coefs)}]"
        for g in L_super.groups:
            L_to_R = L_super.get_group(g).groupby(["agent 2", "depth 2", "eval 2", "coefs 2"])
            record = {
                "V\X": grp_to_id(g)
            }
            for sub in L_to_R.groups:
                subset = L_to_R.get_group(sub)
                plays= len(subset)
                wins = len(subset[subset["winner"] == 1])
                efficiency = round(wins/plays, 3)
                score = sum(subset[subset["winner"] == 1]["score"]) / sum(subset["score"])
                score = round(score, 3)
                record.update({
                    f"{grp_to_id(sub)}": f"[ {plays} , {efficiency:.3f} , {score:.3f} ]"
                })
                #trg = trg.append({
                #    "params": g,
                #    "position": 1,
                #    "plays": plays,
                #    "wins": wins,
                #    "efficiency": round(wins/plays, 2),
                #    "score": score
                #}, ignore_index=True)
            trg = trg.append(record, ignore_index=True)
        
        return trg
    
    def show_brief_in_browser(self):
        self.update_src()
        self.update_brief()
        df = self.parse_brief()
        f = NamedTemporaryFile(delete=True, suffix='.html')
        df.to_html(f.name)
        webbrowser.open(f.name)
STATS = Stats()