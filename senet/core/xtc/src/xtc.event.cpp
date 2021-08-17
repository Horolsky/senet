#pragma once
#include "xtc.common.hpp"
namespace xtc
{
Event &
Event::operator= (const Event &other)
{
  _data._data = other._data;
  return *this;
}
Event &
Event::operator= (Event &&other)
{
  _data._data = other._data;
  return *this;
}

constants::agent_id
Event::agent () const
{
  return _data._agent;
}
constants::action_id
Event::action () const
{
  return _data._action;
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