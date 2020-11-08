help = """
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
 """

rules = """
Goal
  Win by moving all of your Pawns off the board and into the afterlife.

Rules
  The board of Senet is a linear track of 30 squares, following a 'Z' shaped path.
  On each turn, the game throws the stick automatically, and gives the number of steps. 
  An active player has to move any Pawn that many spaces forward.
  If a Pawn lands on an opposing pawn, it 'attacks' it and the two Pawns switch places.
  Pawns which are next to a friendly pawn are defended and cannot be attacked.
  The sticks can give from 1 to 5 steps. Getting a 1, 4, or 5 grants an extra turn.
  If player can advance a piece, he must do so.
      Otherwise, he must move a piece backwards 1 square.
      If player cannot do either, his turn is forfeited.

Special Cells
  (15) House of Rebirth The square where pawns go from the House of Waters.
  (*a)  House of Beauty A mandatory square every pawn must land directly on.
  (*b)  House of Waters Any pawn landing on this square is moved to the House of Rebirth, or behind it if the square is occupied. Any pawn which is attacked on one of the squares after this gets taken to the House of Waters instead of switching places.
  (*c)  House of Three Truths A pawn may only advance on a roll of 3.
  (*d)  House of Re-Atoum A pawn may only advance on a roll of 2.
  (*e)  House of the Scarab A pawn may advance on any roll.

Throwing Sticks 
  The ancient game of Senet was played with four split sticks, each with a mark on one side. 
  Count the number of clear sides facing up to get a roll between zero and four. 
  A zero was counted as a five. This creates a "normal distribution." 
  Unlike a six-sided die where every number is equally likely, in Senet you have a much higher chance of rolling a two, for example. 
  In this game sticks are represented as an array of randomly choosen 1 and 0, and the steps are calculated from their sum
_________
this description of game rules is based on the rules of the web-based Senet by Chris Meub (2019),
see more at http://chrismeub.com/projects/senet.html
"""
info = """
designed by A. Horolsky as a CS study project
development in progress
"""
confirm_q = """
The game is running. Tonfirm quit enter Y,
to continue press ENTER
"""
confirm_s = """
The game is running. To confirm restart enter Y,
to continue press ENTER
"""

warn = "invalid input, try again or press h for help"

options = """
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