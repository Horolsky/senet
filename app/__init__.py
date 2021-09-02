from app.ui import cli, msg_cli
from app.settings import SETTINGS
"""
senet game
"""    

def launch(mode="cli"):
    if mode == "cli":
        print(msg_cli["init"])
        cli().init()
    else:
        print("senet: only cli mode is avaliable for now")