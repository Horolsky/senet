#include "xtc.state.hpp"
//Dice class members defined in separate .cpp

namespace xtc
{

uint64_t
State::build_seed (Unit agent, int steps, int *board)
{
  State::bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.steps = steps;
  data.board = bitf::solid::set_bulk<int *, uint64_t> (
      board, board + board_size, 0UL, board_offset);
  return data.seed;
}

int *
State::board (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + board_size,
                                          _data.board, board_offset);
  return buffer;
}

Unit
State::board (int index) const
{
  return static_cast<Unit> (bitf::solid::get_scalar<int, uint64_t> (
      _data.board, board_offset, index*board_offset));
}

Unit
State::agent () const
{
  return static_cast<Unit> (_data.agent);
}
int
State::steps () const
{
  return (int)_data.steps;
}

State &
State::operator= (const State &other)
{
  _data = other._data;
  return *this;
}
State &
State::operator= (State &&other)
{
  _data = other._data;
  return *this;
}

void
State::update_board (int index, Unit unit)
{
  _data.board = bitf::solid::set_scalar (static_cast<int> (unit), _data.board, board_offset, index*board_offset);
}
uint64_t
State::seed () const
{
  return _data.seed;
}

bool State::is_chance_node () const
{
  return _data.steps == 0;
}
bool State::is_strategy_node () const
{
  return _data.steps > 0;
}
bool State::is_terminal_node () const
{
  int x = bitf::solid::index_of<int, uint64_t>(static_cast<int>(Unit::X), _data.board, board_offset);
  int y = bitf::solid::index_of<int, uint64_t>(static_cast<int>(Unit::Y), _data.board, board_offset);
  return ( y < 0 ) || ( x < 0 );
}
} // namespace xtc
