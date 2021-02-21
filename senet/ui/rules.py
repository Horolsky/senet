rules = """

ABOUT THE GAME:

Goal
  Win by moving all of your Pawns off the board

Rules  
  The board of Senet is a linear track of 30 squares, following a 'Z' shaped path.
  On each turn, the game throws the stick automatically, and gives the number of steps. 
  An active player has to move any Pawn that many spaces forward.
  If a Pawn lands on an opposing pawn, it 'attacks' it and the two Pawns switch places.
  Pawns which are next to a friendly pawn are defended and cannot be attacked.
  The sticks can give from 1 to 5 steps. Getting a 1, 4, or 5 grants an extra turn.
  If player can advance a piece, he must do so. Otherwise, he must move a piece backwards 1 square.
  If player cannot do either, his turn is forfeited.

  This program provides two versions of game rules, one by T. Kendal, and the other by C. Meub.
  Both differs only in mechanics of House of Waters, which is described below.

Special Cells (Meub rules)
  (15)  House of Rebirth      The square where pawns go from the House of Waters.
  (*a)  House of Beauty       A mandatory square every pawn must land directly on.
  (*b)  House of Waters       * Meub version:
                                Any pawn landing on this square is moved to the House of Rebirth immediately,
                                or behind it if the square is occupied. 
                                Any pawn which is attacked on one of the squares after this
                                gets taken to the House of Waters instead of switching places.
                              * Kendal version:
                                If a piece falls on the House of Humiliation, its owner incurs a penalty.
                                The current turn is finished, and during subsequent turns, no other piece of 
                                that player maybe moved until the piece is rescued.
                                A pawn may be rescued from the water by moving it to the House of Rebirth,
                                if that one is not occupied, or by attacking from both directions.
                                Defending is ignored when attacking drowed pawn.
                                A pawn may escape the board on an exact throw of 4.

  (*c)  House of Three Truths A pawn may only advance on a roll of 3.
  (*d)  House of Re-Atoum     A pawn may only advance on a roll of 2.
  (*e)  House of the Scarab   A pawn may advance on any roll.

Throwing Sticks 
  The ancient game of Senet was played with four split sticks, each with a mark on one side. 
  Count the number of clear sides facing up to get a roll between zero and four. 
  A zero was counted as a five. This creates a "normal distribution." 
  Unlike a six-sided die where every number is equally likely, in Senet you have a much higher chance of rolling a two, for example. 
  In this game sticks are represented as an array of randomly choosen 1 and 0, and the steps are calculated from their sum
_________
this is slightly revised version of Senet rules by Chris Meub (2019),
with addition of 
see more at http://chrismeub.com/projects/senet.html
"""
