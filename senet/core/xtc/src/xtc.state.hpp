#pragma once
#include "xtc.common.hpp"
#include "xtc.fwd.hpp"

_XTC_EXTERN_BULK

namespace xtc
{
using constants;

template <class Iterator> 
static State::seed (constants::unit_id _agent, int _steps, Iterator _board)
{
  State::bitfield data {._seed=0UL};
  data._agent = _agent;
  data._steps = _steps;
  data._board = bitf::solid::set_bulk<Iterator, uint64_t> (_board, _board + board_size, 0UL, board_offset);
  return data._seed;
}


template <class Iterator>
Iterator
State::board (Iterator buffer)
{
  bitf::solid::get_bulk<Iterator, uint64_t> (
      buffer, buffer + board_size, _data._board, board_offset);
  return buffer;
}

} // namespace xtc