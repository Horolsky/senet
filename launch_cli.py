#!/usr/bin/env python3

from os import system
from os.path import isdir

if __name__ == "__main__":
    try:
        from senet import launch
        launch("cli")
    except ModuleNotFoundError:
        confirm = input("core extension module is missing, run installation process [Y/n]?\n")
        if confirm in ['Y', 'y']:
            system("""
            make -C senet/core/xtc setup
            """)
            print("extension module installed sucessfully\n\n")
            from senet import launch
            launch("cli")
        else:
            print("unable to launch the game")