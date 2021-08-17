#pragma once
#include "xtc.common.hpp"
#include "xtc.fwd.hpp"
namespace xtc
{
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

constants::unit_id
Event::agent () const
{
  return (constants::unit_id) _data._agent;
}
constants::action_id
Event::action () const
{
  return (constants::action_id) _data._action;
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
}