#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{

uint64_t
Strategies::build_seed (Unit agent, Action direction, int mobility, int *indici,
                   int *actions)
{
  bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.mobility = mobility;
  data.direction = static_cast<uint64_t> (direction);
  data.indici = bitf::solid::set_bulk<int *, uint64_t> (
      indici, indici + cnst::max_strategies, 0UL, cnst::indici_offset);
  data.actions = bitf::solid::set_bulk<int *, uint64_t> (
      actions, actions + cnst::max_strategies, 0UL, cnst::actions_offset);
  return data.seed;
}

uint64_t
Strategies::build_seed (Unit agent, Action direction, int mobility, int *indici)
{
  bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.mobility = mobility;
  data.direction = static_cast<uint64_t> (direction);
  data.indici = bitf::solid::set_bulk<int *, uint64_t> (
      indici, indici + cnst::max_strategies, 0UL, cnst::indici_offset);
  return data.seed;
}

int *
Strategies::indici (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.indici, cnst::indici_offset);
  return buffer;
}

int
Strategies::indici (int index) const
{
  return bitf::solid::get_scalar<int, uint64_t> (_data.indici, index,
                                                 cnst::indici_offset);
}

int *
Strategies::actions (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.actions, cnst::actions_offset);
  return buffer;
}

Action
Strategies::actions (int index) const
{
  return static_cast<Action> (bitf::solid::get_scalar<int, uint64_t> (
      _data.actions, index, cnst::actions_offset));
}

Unit
Strategies::agent () const
{
  return static_cast<Unit> (_data.agent);
}

int
Strategies::mobility () const
{
  return _data.mobility;
}

Action
Strategies::direction () const
{
  return static_cast<Action> (_data.direction);
}

Strategies &
Strategies::operator= (const Strategies &other)
{
  _data = other._data;
  return *this;
}

Strategies &
Strategies::operator= (Strategies &&other)
{
  _data = other._data;
  return *this;
}

bool
Strategies::contains (int index) const
{
  uint64_t indici = _data.indici;
  for (int i = 0; i < _data.mobility; i++)
    if ((indici >> (i * cnst::indici_offset)) & 0b11111UL == index)
      return true;
  return false;
}

uint64_t
Strategies::seed () const
{
  return _data.seed;
}

int
Strategies::push (int index, Action action)
{
  if (_data.mobility == cnst::max_strategies)
    return cnst::max_strategies;

  _data.indici = bitf::solid::set_scalar (_data.indici, _data.mobility,
                                          cnst::indici_offset, index);
  _data.actions = bitf::solid::set_scalar (_data.actions, _data.mobility,
                                           cnst::actions_offset,
                                           static_cast<int> (action));
  return ++_data.mobility;
}

}