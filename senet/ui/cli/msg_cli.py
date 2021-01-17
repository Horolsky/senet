messages = {
  "init" : """
///////////////////////
      SENET GAME
///////////////////////
  """,
  
  "help" : """

    Basic controls:
      q - quit game
      s - start a new game or stop the running one
      a - start autoplay
      b - render the game board
      u - show current state utility
      h - show help info
      r - show rules
      i - detailed info about the game
      o - show options state
      o <key> <value> - set option

    How to play:
    Pawns of two players are represented on the board as V and X
    To start new game enter next command with desired options:
      s <player 1> <player 2> <first> 
    options for this command:
      <player #> - choose "human", "ai" or "dummy"
      <first> - choose pawns troupe (1 or 2) for the first player
      if omitted, first player will start with Y by default
      
    To make a turn enter a coordinates of choosen pawn to move, 
    in form <row> <column> for tabular mode, 
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

  "warn": "invalid input, try again or press h for help"
}

def msgout(msg):
  if msg in messages:
    print("\nsenet: " + messages[msg])
  else:
    print("\nsenet: " + str(msg))