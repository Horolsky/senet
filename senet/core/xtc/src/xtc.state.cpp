#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{
using namespace cnst;

uint64_t
State::build_seed (Unit agent, int steps, int *board)
{
  bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.steps = steps;
  data.board = bitf::solid::set_bulk<int *, uint64_t> (
      board, board + cnst::board_size, 0UL, cnst::board_offset);
  return data.seed;
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
                                          _data.board, cnst::board_offset);
  return buffer;
}

Unit
State::board (int index) const
{
  return static_cast<Unit> (bitf::solid::get_scalar<int, uint64_t> (
      _data.board, index, cnst::board_offset));
}

Unit
State::agent () const
{
  return static_cast<Unit> (_data.agent);
}
int
State::steps () const
{
  return (int)_data.steps;
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
  _data.agent = static_cast<uint64_t> (agent);
}
void
State::set_steps (int steps)
{
  _data.steps = steps;
}
void
State::update_board (int index, Unit unit)
{
  _data.board = bitf::solid::set_scalar (
      _data.board, index, cnst::board_offset, static_cast<int> (unit));
}
uint64_t
State::seed () const
{
  return _data.seed;
}

float
State::expectation () const
{
  uint64_t seed = _data.seed;
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
  Unit agent = static_cast<Unit> (_data.agent);
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);

  /* HOUSE OF WATER PENALTY */
  if (board (House::WATERS) == agent)
    {
      if (_data.steps == 4)
        {
          moves.add_move (House::WATERS, Action::ESCAPE);
          moves._data.direction = 1;
        }
      else if (board (House::REBIRTH) == Unit::NONE)
        {
          moves.add_move (House::REBIRTH, Action::DROW); // reborning
          moves.add_move (House::SKIPTURN, Action::SKIP);
        }
      return moves;
    }
  /* DIRECT MOVE */
  for (int i = 0; i < cnst::board_size; i++)
    {
      Unit cell_occupation = board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + _data.steps;
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
        moves.add_move (i, Action::MOVE);
      else if (trgt == enemy)
        { // attack
          if (dest == House::WATERS)
            moves.add_move (i, Action::ATTACK); // drowed trgt
          else
            {
              Unit prev = dest > 0 ? board (dest - 1) : Unit::NONE;
              Unit nxt = dest < House::SCARAB ? board (dest + 1) : Unit::NONE;
              if (prev != enemy && nxt != enemy)
                moves.add_move (i, dest > House::WATERS ? Action::ATTACK_HOUSE
                                                        : Action::ATTACK);
            }
        }
    }
  /* REVERSE MOVE */
  if (moves._data.mobility == 0)
    {
      Unit trgt = board (0); // state.board % 4;
      for (int i = 1; i < cnst::board_size; i++)
        {
          Unit cell = board (i);

          if (cell == agent && trgt == Unit::NONE)
            moves.add_move (i, Action::RETREAT);
          else if (cell == agent && trgt == enemy)
            {
              if (i == House::TRUTHS)
                moves.add_move (i, Action::SWAPBACK); // drowed trgt
              else
                {
                  Unit prev = i > 1 ? board (i - 2) : Unit::NONE;
                  if (prev != enemy)
                    moves.add_move (i, Action::SWAPBACK);
                }
            }
          trgt = cell;
        }
    }

  if (moves._data.mobility == 0)
    {
      moves.add_move (House::SKIPTURN, Action::SKIP);
    }
  return moves;
}

Moves
State::moves_meub () const
{
  Moves moves (0);
  Unit agent = static_cast<Unit> (_data.agent);
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);
  /* DIRECT MOVE */

  for (int i = 0; i < cnst::board_size; i++)
    {
      Unit cell_occupation = board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + _data.steps;
      Unit trgt = board (dest);
      if (i == House::SCARAB)
        moves.add_move (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        moves.add_move (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
        continue; // forbidden escaping
      else if (dest == House::WATERS)
        moves.add_move (i, Action::DROW); //
      else if (trgt == Unit::NONE)
        moves.add_move (i, Action::MOVE); // normal movement
      else if (trgt == enemy)
        { // attack
          Unit prev = dest > 0 ? board (dest - 1) : Unit::NONE;
          Unit nxt = dest < House::SCARAB ? board (dest + 1) : Unit::NONE;
          if (prev != enemy && nxt != enemy)
            moves.add_move (i, dest > House::WATERS ? Action::ATTACK_HOUSE
                                                    : Action::ATTACK);
        }
    }
  /* REVERSE MOVE */
  if (moves._data.mobility == 0)
    {
      Unit trgt = board (0);
      for (int i = 1; i < House::NETHER; i++)
        {
          Unit cell = board (i);
          if (cell == agent && trgt == Unit::NONE)
            moves.add_move (i, Action::RETREAT);
          trgt = cell;
        }
    }
  if (moves._data.mobility == 0)
    {
      moves.add_move (House::SKIPTURN, Action::SKIP);
    }
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
State::increment_kendall (int index, Moves moves = Moves (0UL)) const
{
  if (moves._data.mobility > 0 && !moves.contains (index))
    throw "illegal move";
  if (moves._data.seed == 0UL)
    moves = this->moves_kendall ();

  Unit agent = static_cast<Unit> (_data.agent);
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);
  int steps = _data.steps;

  /* next step preparation */
  State new_state (_data.seed);
  new_state._data.steps = 0;
  // next agent rule
  if (steps == 1 || steps == 4 || steps == 5)
    new_state._data.agent = _data.agent; // bonus move
  else
    new_state._data.agent = (_data.agent ^ 1); // normal move

  Action action = moves.actions (index);

  /* STATE UPD */
  switch (action)
    {
    case Action::SKIP:
      break;
    case Action::MOVE:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, Unit::NONE);
      break;
    case Action::ESCAPE:
      new_state.update_board (index, Unit::NONE);
      break;
    case Action::DROW:
      new_state.update_board (index, Unit::NONE);
      new_state.update_board (House::REBIRTH,
                              agent); // space is guaranteed by moves func
      break;
    case Action::SWAPBACK:
      new_state.update_board (index - 1, agent);
      new_state.update_board (index, enemy);
      break;
    case Action::ATTACK:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, enemy);
      break;
    case Action::ATTACK_HOUSE:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, Unit::NONE);
      for (int i = House::WATERS; i >= 0; i--)
        {
          if (board (i) == Unit::NONE)
            {
              new_state.update_board (House::WATERS, enemy);
              break;
            }
        }
      break;
    default:
      new_state._data.seed = 0UL; // corrupted logic
      break;
    }

  return new_state;
}

State
State::increment_meub (int index, Moves moves = Moves (0UL)) const
{
  if (moves._data.mobility > 0 && !moves.contains (index))
    throw "illegal move";
  if (moves._data.seed == 0UL)
    moves = this->moves_meub ();

  Unit agent = static_cast<Unit> (_data.agent);
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);
  int steps = _data.steps;

  /* next step preparation */
  State new_state (_data.seed);
  new_state._data.steps = 0;
  // next agent rule
  if (steps == 1 || steps == 4 || steps == 5)
    new_state._data.agent = _data.agent; // bonus move
  else
    new_state._data.agent = (_data.agent ^ 1); // normal move

  Action action = moves.actions (index);

  /* STATE UPD */
  switch (action)
    {
    case Action::SKIP:
      break;
    case Action::MOVE:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, Unit::NONE);
      break;
    case Action::ESCAPE:
      new_state.update_board (index, Unit::NONE);
      break;
    case Action::DROW:
      new_state.update_board (index, Unit::NONE);
      for (int i = House::REBIRTH; i >= 0; i--)
        {
          if (board (i) == Unit::NONE)
            {
              new_state.update_board (i, agent);
              break;
            }
        }
      break;
    case Action::ATTACK:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, enemy);
      break;
    case Action::ATTACK_HOUSE:
      new_state.update_board (index + steps, agent);
      new_state.update_board (index, Unit::NONE);
      for (int i = House::REBIRTH; i >= 0; i--)
        {
          if (board (i) == Unit::NONE)
            {
              new_state.update_board (i, enemy);
              break;
            }
        }
      break;
    default:
      new_state._data.seed = 0UL; // corrupted logic
      break;
    }

  return new_state;
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
