#include "xtc.objects.hpp"
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
State::set_agent (Unit agent)
{
  _data.agent = static_cast<uint64_t> (agent);
}

void
State::set_steps (int steps)
{
  _data.steps = steps;
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

uint64_t
Strategies::build_seed (Unit agent, int mobility,
                        int *indici, int *actions)
{
  Strategies::bitfield data{ .seed = 0UL };
  data.agent = static_cast<uint64_t> (agent);
  data.mobility = mobility;
  data.indici = bitf::solid::set_bulk<int *, uint64_t> (
      indici, indici + mobility, 0UL, indici_offset);
  data.actions = bitf::solid::set_bulk<int *, uint64_t> (
      actions, actions + mobility, 0UL, actions_offset);
  return data.seed;
}

int *
Strategies::indici (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.indici, indici_offset);
  return buffer;
}

int
Strategies::indici (int index) const
{
  return bitf::solid::get_scalar<int, uint64_t> (_data.indici, indici_offset, index*indici_offset);
}

int *
Strategies::actions (int *buffer) const
{
  bitf::solid::get_bulk<int *, uint64_t> (buffer, buffer + _data.mobility,
                                          _data.actions, actions_offset);
  return buffer;
}

Action
Strategies::actions (int index) const
{
  return static_cast<Action> (bitf::solid::get_scalar<int, uint64_t> (
      _data.actions, actions_offset, index*actions_offset));
}

Unit
Strategies::agent () const
{
  return static_cast<Unit> (_data.agent);
}

int
Strategies::mobility () const
{
  return _data.mobility;
}

Strategies &
Strategies::operator= (const Strategies &other)
{
  _data = other._data;
  return *this;
}

Strategies &
Strategies::operator= (Strategies &&other)
{
  _data = other._data;
  return *this;
}

bool
Strategies::contains (int index) const
{
  uint64_t indici = _data.indici;
  for (int i = 0; i < _data.mobility; i++)
    if (((indici >> (i * indici_offset)) & 0b11111UL) == index)
      return true;
  return false;
}

uint64_t
Strategies::seed () const
{
  return _data.seed;
}
//private member
int
Strategies::push (int index, Action action)
{
  if (_data.mobility == max_strategies)
    return max_strategies;

  _data.indici = bitf::solid::set_scalar (index, _data.indici, indici_offset, _data.mobility*indici_offset);
  _data.actions
      = bitf::solid::set_scalar (static_cast<int> (action), _data.actions, actions_offset, _data.mobility*actions_offset);
  return ++_data.mobility;
}

} // namespace xtc
