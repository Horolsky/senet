#pragma once
#include "xtc.common.hpp"
#include "xtc.fwd.hpp"

namespace xtc
{

template uint64_t Moves::seed<int *> (constants::unit_id _agent, constants::action_id _direction,
               int _mobility, int* _indici, int* _actions);

template uint64_t Moves::seed<std::vector<int>::iterator> (constants::unit_id _agent, constants::action_id _direction,
               int _mobility, std::vector<int>::iterator _indici, std::vector<int>::iterator _actions);
template int *Moves::indici<int *> (int *buffer);
template std::vector<int>::iterator
Moves::indici<std::vector<int>::iterator> (std::vector<int>::iterator buffer);

template int *Moves::actions<int *> (int *buffer);
template std::vector<int>::iterator
Moves::actions<std::vector<int>::iterator> (std::vector<int>::iterator buffer);

constants::unit_id
Moves::agent () const
{
  return (constants::unit_id) _data._agent;
}
int
Moves::mobility () const
{
  return _data._mobility;
}
constants::action_id
Moves::direction () const
{
  return (constants::action_id) _data._direction;
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

}
