#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{
using namespace cnst;

uint64_t
State::seed (Unit _agent, int _steps, int *_board)
{
  bitfield data{ ._seed = 0UL };
  data._agent = static_cast<uint64_t>(_agent);
  data._steps = _steps;
  data._board = bitf::solid::set_bulk<int *, uint64_t> (
      _board, _board + cnst::board_size, 0UL, cnst::board_offset);
  return data._seed;
}

/**
 * loads game state to array buffer of size 30
 * return buffer pointer
 * lookup! raw pointers, size not checked
 */
int *
State::board (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (
      buffer, buffer + cnst::board_size, _data._board,
      cnst::board_offset);
  return buffer;
}

Unit
State::agent () const
{
  return static_cast<Unit>(_data._agent);
}
int
State::steps () const
{
  return (int)_data._steps;
}

State &
State::operator= (const State &other)
{
  _data = other._data;
  return *this;
}
State &
State::operator= (State &&other)
{
  _data = other._data;
  return *this;
}
void
State::set_agent (Unit agent)
{
  _data._agent = static_cast<uint64_t>(agent);
}
void
State::set_steps (int steps)
{
  _data._steps = steps;
}

Moves
moves_kendall (State state)
{
  return Moves ();
}

Moves
moves_meub (State state)
{
  return Moves ();
}

Moves
State::moves (Rules rules) const
{
  if (rules == Rules::KENDALL)
    {
      return Moves (); // moves_kendall (this);
    }
  else if (rules == Rules::MEUB)
    {
      return Moves (); // moves_meub (this);
    }
  else
    throw "invalid rules";
}

float
State::evaluate () const
{
  uint64_t seed = _data._seed;
  uint8_t maxSum = 0;
  uint8_t minSum = 0;
  for (uint8_t i = 4; i < 64; i += 2)
    {
      uint8_t cell = (seed >> i) & 0b11;
      if (cell == 0)
        maxSum += (30 - i);
      else if (cell == 1)
        minSum += (30 - i);
    }
  float res;
  if (minSum == 0)
    res = 0;
  else if (maxSum == 0)
    res = 0;
  else
    res = (float)(minSum) / (maxSum + minSum);
  return res;
}

} // namespace xtc

/*
Moves moves_kendall(State state)
{
    const uint8_t agent = xtc::state::agent<uint8_t>(state);
    const uint8_t enemy = agent ^ 1U;
    uint8_t indici[7];
    uint8_t actions[7];
    uint8_t mobility {0};
    // only SKIP, MOVE and RETREAT used here

    uint8_t dest, trgt;

    uint8_t board[30];
    board = xtc::state::board<uint8_t*>(board, state);

    // HOUSE OF WATER PENALTY
    if (board[WATERS] == agent){
        if (state._steps == 4){
            indici[mobility++] = WATERS; //escaping actions[mobility]
= MOVE;
        }
        else if (board[REBIRTH] == unit_id::NONE::value){
            indici[mobility++] = WATERS;
            actions[mobility] = RETREAT;
            moves[mobility++] = WATERS;
            actions[mobility] = SKIP;
        }
        return moves::seed<uint8_t*,uint8_t>()
    }
    // DIRECT MOVE
    moves._dir = 1;
    for (ui8 i =0; i < 30; i++){
        ui8 cell = BOARD_GET(state, i);
        if (cell != agent) continue;
        dest = i + state._steps;
        trgt = BOARD_GET(state, dest);
        if (i == 29) moves = _moves_add_move(moves, i); // always escaping else
if (i > 24 && dest == 30) moves = _moves_add_move(moves, i);         // correct
escaping else if (i != 25 && dest > 25) continue;                            //
forbidden house arriving else if (dest > 29) continue; // corrupted logic else
if (trgt == 0) moves = _moves_add_move(moves, i);                    // normal
movement else if (trgt == enemy) {                                           //
attack ui8 prev = dest > 0 ? BOARD_GET(state, (dest-1)) : 0; ui8 nxt = dest <
29 ? BOARD_GET(state, (dest+1)) : 0; if (prev != enemy && nxt != enemy) moves =
_moves_add_move(moves, i); //defended trgt else if (dest == 26) moves =
_moves_add_move(moves, i);               //drowed trgt
        }
    }
    // REVERSE MOVE
    if (moves._len == 0) {
        moves._dir = 2;
        ui8 cell;
        trgt = BOARD_GET(state, 0);// state._board % 4;
        for (ui8 i = 1; i < 30; i++) {
            cell = BOARD_GET(state, i);
            if (cell == agent && trgt == 0) moves = _moves_add_move(moves, i);
            else if (cell == agent && trgt == enemy) { // attack backward ui8
prev = i > 1 ? BOARD_GET(state, (i-2)) : 0; if (prev != enemy) moves =
_moves_add_move(moves, i); else if (i == 27) moves = _moves_add_move(moves, i);
//drowed trgt
            }
            trgt = cell;
        }
    }
    / SKIP MOVE
    if (moves._len == 0) moves._dir = 0;
    return moves._seed;
}
*/
