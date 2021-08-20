#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"

namespace xtc
{

uint64_t
Moves::build_seed (Unit _agent, Action _direction, int _mobility, int *_indici,
             int *_actions)
{
  bitfield data{ ._seed = 0UL };
  data._agent = static_cast<uint64_t>(_agent);
  data._mobility = _mobility;
  data._direction = static_cast<uint64_t>(_direction);
  data._indici = bitf::solid::set_bulk<int *, uint64_t> (
      _indici, _indici + cnst::max_moves, 0UL, cnst::indici_offset);
  data._actions = bitf::solid::set_bulk<int *, uint64_t> (
      _actions, _actions + cnst::max_moves, 0UL, cnst::actions_offset);
  return data._seed;
}

uint64_t
Moves::build_seed (Unit _agent, Action _direction, int _mobility, int *_indici)
{
  bitfield data{ ._seed = 0UL };
  data._agent = static_cast<uint64_t>(_agent);
  data._mobility = _mobility;
  data._direction = static_cast<uint64_t>(_direction);
  data._indici = bitf::solid::set_bulk<int *, uint64_t> (
      _indici, _indici + cnst::max_moves, 0UL, cnst::indici_offset);
  return data._seed;
}

int *
Moves::indici (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data._mobility,
                                          _data._indici, cnst::indici_offset);
  return buffer;
}

int *
Moves::actions (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (
      buffer, buffer + _data._mobility, _data._actions, cnst::actions_offset);
  return buffer;
}

Unit
Moves::agent () const
{
  return static_cast<Unit>(_data._agent);
}

int
Moves::mobility () const
{
  return _data._mobility;
}

Action
Moves::direction () const
{
  return static_cast<Action>(_data._direction);
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

uint64_t Moves::seed () const
{
  return _data._seed;
}

int Moves::add_move(int index, Action action)
{
  if (_data._mobility == cnst::max_moves) return cnst::max_moves;
  
  _data._indici = bitf::solid::set_scalar(_data._indici, _data._mobility, cnst::indici_offset, index);
  _data._actions = bitf::solid::set_scalar(_data._actions, _data._mobility, cnst::actions_offset, static_cast<int>(action));
  return ++_data._mobility;
}

}
