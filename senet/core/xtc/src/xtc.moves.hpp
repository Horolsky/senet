#pragma once
#include "xtc.common.hpp"
#include "xtc.fwd.hpp"

_XTC_EXTERN_BULK

namespace xtc
{
using constants;

template <class Iterator> 
static Moves::seed (constants::unit_id _agent, constants::action_id _direction, int _mobility, Iterator _indici, Iterator _actions)
{
  Moves::bitfield data {._seed=0UL};
  data._agent = _agent;
  data._mobility = _mobility;
  data._direction = _direction;
  data._indici = bitf::solid::set_bulk<Iterator, uint64_t> (_indici, _indici + max_moves, 0UL, indici_offset);
  data._actions = bitf::solid::set_bulk<Iterator, uint64_t> (_actions, _actions + max_moves, 0UL, actions_offset);
  return data._seed;
}
template <class Iterator> 
static Moves::seed (constants::unit_id _agent, constants::action_id _direction, int _mobility, Iterator _indici)
{
  Moves::bitfield data {._seed=0UL};
  data._agent = _agent;
  data._mobility = _mobility;
  data._direction = _direction;
  data._indici = bitf::solid::set_bulk<Iterator, uint64_t> (_indici, _indici + max_moves, 0UL, indici_offset);
  return data._seed;
}

template <class Iterator>
Iterator
Moves::indici (Iterator buffer)
{
  bitf::solid::get_bulk<Iterator, uint64_t> (
      buffer, buffer + max_moves, _data._indici, indici_offset);
  return buffer;
}

template <class Iterator>
Iterator
Moves::actions (Iterator buffer)
{
  bitf::solid::get_bulk<Iterator, uint64_t> (
      buffer, buffer + max_moves, _data._actions, actions_offset);
  return buffer;
}

} // namespace xtc