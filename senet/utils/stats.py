import pandas as pd
from .singleton import singleton
from .report import Report
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
    _report = None
    _df = {
        "brief": None,
        "games": None,
        "ai": None,
        "meub": None
    }
    def __init__(self):
        self._report = Report("stats", None, "txt", "logs/stats", stats_header, False)
        self.update_src()
        
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
        trg = pd.DataFrame(columns=["params", "position", "plays", "wins", "efficiency", "score"])
        groups1 = src.groupby(["agent 1", "depth 1", "eval 1", "coefs 1"])
        groups2 = src.groupby(["agent 2", "depth 2", "eval 2", "coefs 2"])
        for g in groups1.groups:
            subdf = groups1.get_group(g)
            plays= len(subdf)
            wins = len(subdf[subdf["winner"] == 1])
            score = sum(subdf[subdf["winner"] == 1]["score"]) / sum(subdf["score"])
            trg = trg.append({
                "params": g,
                "position": 1,
                "plays": plays,
                "wins": wins,
                "efficiency": round(wins/plays, 2),
                "score": score
            }, ignore_index=True)
        for g in groups2.groups:
            subdf = groups2.get_group(g)
            plays= len(subdf)
            wins = len(subdf[subdf["winner"] == 2])
            score = sum(subdf[subdf["winner"] == 2]["score"]) / sum(subdf["score"])
            trg = trg.append({
                "params": g,
                "position": 2,
                "plays": plays,
                "wins": wins,
                "efficiency": round(wins/plays, 2),
                "score": score
            }, ignore_index=True)

        trg.sort_values("score")
        return trg