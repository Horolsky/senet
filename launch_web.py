from senet.xtc import Ply, emax
from senet.webplayer import meubplayer as mp

if __name__ == "__main__":
    mp.launch()
    n = input("input a number of autoplays:")
    n = int(n)
    while n > 0:
        mp.play()
        mp.restart()
    print("shutting down webplayer")
    mp.quit()
"""
from senet.webplayer import meubplayer as mp
mp.launch()
"""