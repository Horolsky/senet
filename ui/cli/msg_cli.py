messages = {
  "help" : """
Basic controls:
  q - quit game
  s - start new game or restart running one
  g - show game status
  b - render game board
  h - show help info
  r - show rules
  i - detailed info about the game
  o - show options info
  o list - show options state
  o <name> <value> - set option
All commands are case insensitive

How to play:
 Press s to start new game. 
 Pawns are represented as V and X for 1st and 2nd player
 To make a turn enter a T command with coordinates of choosen pawn to move, 
 in form T <row> <cell>, for tabular mode, or
 T <index> for linear mode.
 In both modes indexation starts from 1.

 Note that on the second row pawns are moving from right to left,
 but that does not affect the cell coordinates in tabular mode. 

 to test the game without manual control enter "auto" after the game start
 """,

"info" : """
designed by A. Horolsky as a CS study project
development in progress
""",

"confirm_q" : """
The game is running. Tonfirm quit enter Y,
to continue press ENTER
""",

"confirm_s" : """
The game is running. To confirm restart enter Y,
to continue press ENTER
""",

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
