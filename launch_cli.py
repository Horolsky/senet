#!/usr/bin/env python3

from os import system
from os.path import isdir

if __name__ == "__main__":
    try:
        from app import launch
        launch("cli")
    except ModuleNotFoundError:
        confirm = input("core extension module is missing, run installation process [Y/n]?\n")
        if confirm in ['Y', 'y']:
            system("""
            make -C app/core/xtc setup
            """)
            print("extension module installed sucessfully\n\n")
            from app import launch
            launch("cli")
        else:
            print("unable to launch the game")