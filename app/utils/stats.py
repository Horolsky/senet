import pandas as pd
from tempfile import NamedTemporaryFile
from .singleton import singleton
from os import listdir
from os.path import isfile, join

pd.set_option('display.width', 1000)
pd.set_option('display.max_columns', None)
stats_header = "game cumulative statistics"

class Stats(metaclass=singleton):
    _src = {
        "brief": [],
        # "games": [],
        # "ai": [],
        # "meub": []
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
            try:
                self._src[dr] = [join(f"logs/{dr}", f) for f in listdir(f"logs/{dr}") if isfile(join(f"logs/{dr}", f))]
            except:
                pass

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
        brief = self._df["brief"]
        src = brief[(brief["agent 1"] == "AI") & (brief["agent 2"] == "AI")]
        
        trgattrs=[
            "A",      # "Pos",
            "F",        # "First",
            "T",        # "Timer",
            "D",        # "Depth",
            "E",     # "Eval",
            "C",        # "Coefs",
            "~D",       # "~Depth",
            "~E",        # "~Eval",
            "~C",       # "~Coefs",
            "Plays",        # "Plays",
            "Wins",     # "Wins",
            "Score"     # "Score"
        ]
        summary = pd.DataFrame(columns=trgattrs)
        srcattrs = [
            "first move",
            "timer",
            "depth 1",
            "eval 1",
            "coefs 1",
            "depth 2",
            "eval 2",
            "coefs 2"
            ]
        superset = src.groupby(srcattrs)
        
        for group in superset.groups:
            data = superset.get_group(group)
            if "human" in group:
                continue
            record1 = {"A": "X"}
            record2 = {"A": "Y"}
            for i in range(2):
                record1.update({f"{trgattrs[i+1]}": group[i]})
                record2.update({f"{trgattrs[i+1]}": group[i]})
            for i in range(2,8):
                i2 = (i+3,i-3)[i>4]
                record1.update({f"{trgattrs[i+1]}": group[i]})
                record2.update({f"{trgattrs[i+1]}": group[i2]})

            plays = len(data)
            wins = len(data[data["winner"] == "X"]) / plays
            score = sum(data[data["winner"] == "X"]["score"]) / sum(data["score"])
            record1.update({
                "Plays": plays,
                "Wins": round(wins, 3),
                "Score": round(score,3)
                })
            record2.update({
                "Plays": plays,
                "Wins": round(1-wins, 3),
                "Score": round(1-score,3)
                })
                        
            summary = summary.append(record1, ignore_index=True)
            summary = summary.append(record2, ignore_index=True)
        
        return summary
    
    def show_brief(self):
        self.update_src()
        self.update_brief()
        df = self.parse_brief()
        df = df.sort_values(by='Wins',ascending=False)
        print(df)
STATS = Stats()