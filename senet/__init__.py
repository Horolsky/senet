from senet.ui import cli, msg_cli
from senet.settings import SETTINGS
"""
senet game
"""    

def start(mode="cli"):
    if mode == "cli":
        print(msg_cli["init"])
        cli().init()
    else:
        print("senet: only cli mode is avaliable for now")