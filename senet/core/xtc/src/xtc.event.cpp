#pragma once
#include "xtc.common.hpp"
#include "xtc.internal.hpp"
namespace xtc
{
uint64_t
Event::build_seed (Unit agent, Action action, int start, int destination)
{
  bitfield data{ .seed = 0 };
  data.agent = static_cast<uint64_t>(agent);
  data.action = static_cast<uint64_t>(action);
  data.start = start;
  data.destination = destination;
  return data.seed;
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
  return static_cast<Unit>(_data.agent);
}
Action
Event::action () const
{
  return static_cast<Action>(_data.action);
}
int
Event::start () const
{
  return _data.start;
}
int
Event::destination () const
{
  return _data.destination;
}

uint16_t Event::seed () const
{
  return _data.seed;
}

}