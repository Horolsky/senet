#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{
using namespace cnst;

uint64_t
State::build_seed (Unit _agent, int _steps, int *_board)
{
  bitfield data{ ._seed = 0UL };
  data._agent = static_cast<uint64_t> (_agent);
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
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + cnst::board_size,
                                          _data._board, cnst::board_offset);
  return buffer;
}

Unit
State::board (int index) const
{
  return static_cast<Unit> (bitf::solid::get_scalar<int, uint64_t> (
      _data._board, index, cnst::board_offset));
}

Unit
State::agent () const
{
  return static_cast<Unit> (_data._agent);
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
  _data._agent = static_cast<uint64_t> (agent);
}
void
State::set_steps (int steps)
{
  _data._steps = steps;
}

uint64_t
State::seed () const
{
  return _data._seed;
}

float
State::expectation () const
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

Moves
State::moves_kendall () const
{
  Moves moves (0);
  Unit agent = static_cast<Unit> (_data._agent);
  Unit enemy = static_cast<Unit> (_data._agent ^ 1);

  /* HOUSE OF WATER PENALTY */
  if (board (House::WATERS) == agent)
    {
      if (_data._steps == 4)
        {
          moves.add_move (House::WATERS, Action::ESCAPE);
          moves._data._direction = 1;
        }
      else if (board (House::REBIRTH) == Unit::NONE)
        {
          moves.add_move (House::REBIRTH, Action::DROW);   // reborning
          moves.add_move (House::REBIRTH, Action::ESCAPE); // skipping
          moves._data._direction = 2;                      // keep this not 0
        }
      return moves;
    }
  /* DIRECT MOVE */
  moves._data._direction = 1;
  for (int i = 0; i < cnst::board_size; i++)
    {
      Unit cell_occupation = board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + _data._steps;
      Unit trgt = board (dest);
      if (i == House::SCARAB)
        moves.add_move (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        moves.add_move (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
        continue; // corrupted logic
      else if (trgt == Unit::NONE)
        moves.add_move (i, Action::MOVE); // normal movement
      else if (trgt == enemy)
        { // attack
          Unit prev = dest > 0 ? board (dest - 1) : Unit::NONE;
          Unit nxt = dest < House::SCARAB ? board (dest + 1) : Unit::NONE;
          if (prev != enemy && nxt != enemy)
            moves.add_move (i, Action::ATTACK); // not defended trgt
          else if (dest == House::WATERS)
            moves.add_move (i, Action::ATTACK); // drowed trgt
        }
    }
  /* REVERSE MOVE */
  if (moves._data._mobility == 0)
    {
      moves._data._direction = 2;

      Unit trgt = board (0); // state._board % 4;
      for (int i = 1; i < cnst::board_size; i++)
        {
          Unit cell = board (i);

          if (cell == agent && trgt == Unit::NONE)
            moves.add_move (i, Action::RETREAT);
          else if (cell == agent && trgt == enemy)
            { // attack backward
              Unit prev = i > 1 ? board (i - 2) : Unit::NONE;
              if (prev != enemy)
                moves.add_move (i, Action::RETREAT); // retreat with attack
              else if (i == House::TRUTHS)
                moves.add_move (i, Action::RETREAT); // drowed trgt
            }
          trgt = cell;
        }
    }
  /* SKIP MOVE */
  if (moves._data._mobility == 0)
    moves._data._direction = 0;
  return moves;
}

Moves
State::moves_meub () const
{
  Moves moves (0);
  Unit agent = static_cast<Unit> (_data._agent);
  Unit enemy = static_cast<Unit> (_data._agent ^ 1);
  /* DIRECT MOVE */
  moves._data._direction = 1;
  for (int i = 0; i < cnst::board_size; i++)
    {
      Unit cell_occupation = board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + _data._steps;
      Unit trgt = board (dest);
      if (i == House::SCARAB)
        moves.add_move (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        moves.add_move (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
        continue; // forbidden escaping
      else if (trgt == Unit::NONE)
        moves.add_move (i, Action::MOVE); // normal movement
      else if (trgt == enemy)
        { // attack
          Unit prev = dest > 0 ? board (dest - 1) : Unit::NONE;
          Unit nxt = dest < House::SCARAB ? board (dest + 1) : Unit::NONE;
          if (prev != enemy && nxt != enemy)
            moves.add_move (i, Action::ATTACK);
        }
    }
  /* REVERSE MOVE */
  if (moves._data._mobility == 0)
    {
      moves._data._direction = 2;
      Unit trgt = board (0);
      for (int i = 1; i < House::NETHER; i++)
        {
          Unit cell = board (i);
          if (cell == agent && trgt == Unit::NONE)
            moves.add_move (i, Action::RETREAT);
          trgt = cell;
        }
    }
  /* SKIP MOVE */
  if (moves._data._mobility == 0)
    moves._data._direction = 0;
  return moves;
}

State::moves_f
State::get_moves_f (Rules rules)
{
  if (rules == Rules::KENDALL)
    {
      return &State::moves_kendall;
    }
  else if (rules == Rules::MEUB)
    {
      return &State::moves_meub;
    }
  else
    throw "invalid rules";
}

State
State::increment_kendall () const
{
  return State ();
}

State
State::increment_meub () const
{
  return State ();
}

State::increment_f
State::get_increment_f (Rules rules)
{
  if (rules == Rules::KENDALL)
    {
      return &State::increment_kendall;
    }
  else if (rules == Rules::MEUB)
    {
      return &State::increment_meub;
    }
  else
    throw "invalid rules";
}

} // namespace xtc
