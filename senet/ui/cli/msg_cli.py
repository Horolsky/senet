messages = {
  "init" : """
///////////////////////
      SENET GAME
///////////////////////
  """,
  
  "help" : """

    Basic controls:
      q - quit game
      s - start new game or stop the running one
      a - start autoplay
      b - render the game board
      u - show current state utility
      h - show help info
      r - show rules
      i - detailed info about the game
      o - show options info
      o list - show options state
      o <name> <value> - set option
    All commands are case insensitive

    How to play:
    Pawns of two players are represented on the board as V and X
    To start new game enter next command with desired options:
      s <player 1> <player 2> <first> 
    options for this command:
      <player #> - choose "human", "ai" or "dummy"
      <first> - choose pawns troupe (1 or 2) for the first player
      if omitted, first player will start with Y by default
      
    To make a turn enter a coordinates of choosen pawn to move, 
    in form <row> <cell> for tabular mode, 
    or <index> for linear mode.
    In both modes indexation starts from 1.

    Note that on the second row pawns are moving from right to left,
    but that does not affect the cell coordinates in tabular mode. 
    """,

  "info" : """

    designed by A. Horolsky as a CS study project
    development in progress
    """,

  "confirm_q" : "The game is running. Ton confirm quit enter Y, to continue press any key",

  "confirm_s" : "The game is running. To confirm restart enter Y, to continue press any key",

  "warn": "invalid input, try again or press h for help",

  "options" : """

    List of avaliable options:

      COORDINATES MODE. There are two options:
      - linear: to make move enter a number of cell with controlled pawn (1-30)
      - tabular: to make move enter a row and column of cell with controlled pawn. This is default option.
      Note that the movement direction in the second row does not affect the column numbering, which tarts from left. 
      Commands:
        o crd tbl - toggle tabular mode (t <row> <column>)
        o crd lin - toggle linear mode (t <cellnum>)

      BOARD MODE. Same as above, the game board can be rendered as single line or as table (default)
      this does not affect the coordinates mode
      Commands:
        o brd tbl - toggle tabular mode 
        o brd lin - toggle linear mode
    """  
}

def msgout(msg):
  if msg in messages:
    print("\nsenet: " + messages[msg])
  else:
    print("\nsenet: " + str(msg))