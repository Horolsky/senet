#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"
namespace xtc
{
uint64_t
Event::build_seed (Unit _agent, Action _action, int _start, int _destination)
{
  bitfield data{ ._seed = 0 };
  data._agent = static_cast<uint64_t>(_agent);
  data._action = static_cast<uint64_t>(_action);
  data._start = _start;
  data._destination = _destination;
  return data._seed;
}

Event &
Event::operator= (const Event &other)
{
  _data = other._data;
  return *this;
}
Event &
Event::operator= (Event &&other)
{
  _data = other._data;
  return *this;
}

Unit
Event::agent () const
{
  return static_cast<Unit>(_data._agent);
}
Action
Event::action () const
{
  return static_cast<Action>(_data._action);
}
int
Event::start () const
{
  return _data._start;
}
int
Event::destination () const
{
  return _data._destination;
}

uint16_t Event::seed () const
{
  return _data._seed;
}

}