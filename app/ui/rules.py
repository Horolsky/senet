rules = """

SENET RULES:
These rules are based on the Kendall's Senet rules from 1978,
with slight modification added, read original rules here:
https://www.cs.brandeis.edu/~storer/JimPuzzles/GAMES/Senet/INFO/WalkersRulesKendal.pdf

The board of Senet is a linear track of 30 squares, following a 'Z' shaped path.
On each turn, the game throws the dice automatically, and gives the number of steps. 
An active player has to move any pawn that many steps forward.
The sticks can give from 1 to 5 steps. Getting a 1, 4, or 5 grants an extra turn.

Goal: Win by moving all of your pawns off the board (ESCAPE)

On each turn game automatically shows you all possible actions:

  SKIP          - skip turn and throw sticks again
  MOVE          - move pawn forward
  RETREAT       - return pawn back 1 step
  SWAPBACK      - swap pawn with one behind
  DROW          - release House of Waters penalty and move pawn to House of Rebirth (15),
                  or, if it is occupied, to first empty cell behind it
  ATTACK        - switch places with opponent's pawn
  ATTACK_HOUSE  - take place of opponents pawn, which goes to House of Waters,
                  or, if it is occupied, to first empty cell behind it
  ESCAPE        - escape the board to the NETHERWORLD
  PANIC         - special ability of the veryy last pawn in team:
                  if it is beyond House of Waters, it can move to the House of Beauty.
                  if any pawn is there, they swap places

Special Cells

  15    House of Rebirth      The square where pawns go from the House of Waters.
  25,a  House of Beauty       A mandatory square every pawn must land directly on befor entering other Houses.
  26,b  House of Waters       If a piece falls on the House of Waters, its owner incurs a penalty (SKIP).
                              The current turn is finished, and during subsequent turns, no other piece of 
                              that player maybe moved until the piece is rescued.
                              A pawn may be rescued from the water by moving it to the House of Rebirth (DROW),
                              if that one is not occupied, or by attacking from both directions.
                              Defending is ignored when attacking drowed pawn.
                              A pawn may escape the board on an exact throw of 4.
  27,c  House of Three Truths A pawn may only advance on a roll of 3.
  28,d  House of Re-Atoum     A pawn may only advance on a roll of 2.
  29,e  House of the Scarab   A pawn may advance on any roll.

Sticks Dice 

  The ancient game of Senet was played with four split sticks, each with a mark on one side. 
  Count the number of clear sides facing up to get a roll between zero and four. 
  A zero was counted as a five. This creates uneven probabilities for each roll:
  1: .25
  2: .375
  3: .25
  4: .0625
  5: .0625
"""
