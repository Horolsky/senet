#pragma once
#include "xtc.moves.hpp"
#include "xtc.common.hpp"
#include "xtc.fwd.hpp"

namespace xtc
{

template static Moves::seed<int *> (constants::unit_id _agent, int _steps,
                                    int *_board);
template static Moves::seed<std::vector<int>::iterator> (
    constants::unit_id _agent, int _steps, std::vector<int>::iterator _board);

template int *Moves::indici<int *> (int *buffer);
template std::vector<int>::iterator
Moves::indici<std::vector<int>::iterator> (std::vector<int>::iterator buffer);

template int *Moves::actions<int *> (int *buffer);
template std::vector<int>::iterator
Moves::actions<std::vector<int>::iterator> (std::vector<int>::iterator buffer);

constants::unit_id
Moves::agent () const
{
  return _data._agent;
}
int
Moves::mobility () const
{
  return _data._mobility;
}
constants::action_id
Moves::direction () const
{
  return _data._direction;
}

State &
Moves::operator= (const Moves &other)
{
  _data._data = other._data;
  return *this;
}
State &
Moves::operator= (Moves &&other)
{
  _data._data = other._data;
  return *this;
}

}
