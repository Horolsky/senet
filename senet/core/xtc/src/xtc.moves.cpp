#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{

uint64_t
Moves::build_seed (Unit agent, Action direction, int mobility, int *indici,
                   int *actions)
{
  bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.mobility = mobility;
  data.direction = static_cast<uint64_t> (direction);
  data.indici = bitf::solid::set_bulk<int *, uint64_t> (
      indici, indici + cnst::max_moves, 0UL, cnst::indici_offset);
  data.actions = bitf::solid::set_bulk<int *, uint64_t> (
      actions, actions + cnst::max_moves, 0UL, cnst::actions_offset);
  return data.seed;
}

uint64_t
Moves::build_seed (Unit agent, Action direction, int mobility, int *indici)
{
  bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.mobility = mobility;
  data.direction = static_cast<uint64_t> (direction);
  data.indici = bitf::solid::set_bulk<int *, uint64_t> (
      indici, indici + cnst::max_moves, 0UL, cnst::indici_offset);
  return data.seed;
}

int *
Moves::indici (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.indici, cnst::indici_offset);
  return buffer;
}

int
Moves::indici (int index) const
{
  return bitf::solid::get_scalar<int, uint64_t> (_data.indici, index,
                                                 cnst::indici_offset);
}

int *
Moves::actions (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.actions, cnst::actions_offset);
  return buffer;
}

Action
Moves::actions (int index) const
{
  return static_cast<Action> (bitf::solid::get_scalar<int, uint64_t> (
      _data.actions, index, cnst::actions_offset));
}

Unit
Moves::agent () const
{
  return static_cast<Unit> (_data.agent);
}

int
Moves::mobility () const
{
  return _data.mobility;
}

Action
Moves::direction () const
{
  return static_cast<Action> (_data.direction);
}

Moves &
Moves::operator= (const Moves &other)
{
  _data = other._data;
  return *this;
}

Moves &
Moves::operator= (Moves &&other)
{
  _data = other._data;
  return *this;
}

bool
Moves::contains (int index) const
{
  uint64_t indici = _data.indici;
  for (int i = 0; i < _data.mobility; i++)
    if ((indici >> (i * cnst::indici_offset)) & 0b11111UL == index)
      return true;
  return false;
}

uint64_t
Moves::seed () const
{
  return _data.seed;
}

int
Moves::add_move (int index, Action action)
{
  if (_data.mobility == cnst::max_moves)
    return cnst::max_moves;

  _data.indici = bitf::solid::set_scalar (_data.indici, _data.mobility,
                                          cnst::indici_offset, index);
  _data.actions = bitf::solid::set_scalar (_data.actions, _data.mobility,
                                           cnst::actions_offset,
                                           static_cast<int> (action));
  return ++_data.mobility;
}

}
