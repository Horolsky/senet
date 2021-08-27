#include "xtc.state.hpp"
#include "xtc.strategies.hpp"

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
      _data.board, board_offset, index * board_offset));
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
  _data.board = bitf::solid::set_scalar (static_cast<int> (unit), _data.board,
                                         board_offset, index * board_offset);
}
uint64_t
State::seed () const
{
  return _data.seed;
}

bool
State::is_chance_node () const
{
  return _data.steps == 0;
}
bool
State::is_strategy_node () const
{
  return _data.steps > 0;
}
bool
State::is_terminal_node () const
{
  int x = bitf::solid::index_of<int, uint64_t> (static_cast<int> (Unit::X),
                                                _data.board, board_offset);
  int y = bitf::solid::index_of<int, uint64_t> (static_cast<int> (Unit::Y),
                                                _data.board, board_offset);
  return y < 0 || y >= board_size || x < 0 || x >= board_size;
}

StrategyNode
ChanceNode::chance (int chance) const
{
  return StrategyNode (*this, chance);
}

Strategies
StrategyNode::strategies () const
{
  Strategies strategies (0UL);
  int steps = this->steps ();
  Unit agent = this->agent ();
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);

  /* HOUSE OF WATER PENALTY */
  if (this->board (House::WATERS) == agent)
    {
      if (steps == 4)
        {
          strategies.push (House::WATERS, Action::ESCAPE);
        }
      else if (this->board (House::REBIRTH) == Unit::NONE)
        {
          strategies.push (House::WATERS, Action::DROW); // reborning
          strategies.push (House::SKIPTURN, Action::SKIP);
        }
      return strategies;
    }
  /* DIRECT MOVE */
  int team_size = 0;
  int last_paw = -1;
  for (int i = 0; i < State::board_size; i++)
    {
      Unit cell_occupation = this->board (i);
      if (cell_occupation != agent)
        continue;
      else
      {
        ++team_size;
        last_paw = i;
      }
      int dest = i + steps;
      Unit trgt = this->board (dest);
      if (i == House::SCARAB)
        strategies.push (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        strategies.push (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
      {
        strategies._data.seed = 0;
        strategies.push (House::ERROR, Action::ERROR);
        break;
      }
      else if (trgt == Unit::NONE)
        strategies.push (i, Action::MOVE);
      else if (trgt == enemy)
        { // attack
          if (dest == House::WATERS)
            strategies.push (i, Action::ATTACK); // drowed trgt
          else
            {
              Unit prev = dest > 0 ? this->board (dest - 1) : Unit::NONE;
              Unit nxt
                  = dest < House::SCARAB ? this->board (dest + 1) : Unit::NONE;
              if (prev != enemy && nxt != enemy)
                strategies.push (i, dest > House::WATERS ? Action::ATTACK_HOUSE
                                                         : Action::ATTACK);
            }
        }
    }
  /* REVERSE MOVE */
  if (strategies.mobility () == 0)
    {
      Unit trgt = this->board (0);
      for (int i = 1; i < State::board_size; i++)
        {
          Unit cell = this->board (i);

          if (cell == agent && trgt == Unit::NONE)
            strategies.push (i, Action::RETREAT);
          else if (cell == agent && trgt == enemy)
            {
              if (i == House::TRUTHS)
                strategies.push (i, Action::SWAPBACK); // drowed trgt
              else
                {
                  Unit prev = i > 1 ? this->board (i - 2) : Unit::NONE;
                  if (prev != enemy)
                    strategies.push (i, Action::SWAPBACK);
                }
            }
          trgt = cell;
        }
    }

  if (strategies.mobility () == 0)
    {
      strategies.push (House::SKIPTURN, Action::SKIP);
    }
  if (team_size == 1 && last_paw > House::WATERS)
  {
    strategies.push(last_paw, Action::PANIC);
  }

  if (team_size == 0)
    {
      strategies._data.seed = 0;
      strategies.push (House::ERROR, Action::ERROR);
      // throw std::logic_error("corrupted game logic");
    }
  return strategies;
}
ChanceNode
StrategyNode::choice (int choice, Strategies strategies) const
{
  if (strategies.seed () == 0UL)
    strategies = this->strategies ();
  if (strategies.mobility () > 0 && !strategies.contains (choice))
    throw std::logic_error ("illegal move");

  Unit agent = static_cast<Unit> (_data.agent);
  Unit enemy = static_cast<Unit> (_data.agent ^ 1);
  int steps = _data.steps;

  /* next step preparation */
  ChanceNode new_state (_data.seed);
  // next agent rule
  if (steps == 1 || steps == 4 || steps == 5)
    new_state._data.agent = _data.agent; // bonus move
  else
    new_state._data.agent = (_data.agent ^ 1); // normal move

  int index = bitf::solid::index_of<int>(choice, strategies._data.indici,
                                     Strategies::indici_offset);
  Action action = strategies.actions (index);

  /* STATE UPD */
  switch (action)
    {
    case Action::SKIP:
      break;
    case Action::MOVE:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, Unit::NONE);
      break;
    case Action::ESCAPE:
      new_state.update_board (choice, Unit::NONE);
      break;
    case Action::DROW:
      new_state.update_board (choice, Unit::NONE);
      new_state.update_board (House::REBIRTH,
                              agent); // space is guaranteed by strategies func
      break;
    case Action::SWAPBACK:
      new_state.update_board (choice - 1, agent);
      new_state.update_board (choice, enemy);
      break;
    case Action::RETREAT:
      new_state.update_board (choice - 1, agent);
      new_state.update_board (choice, Unit::NONE);
      break;
    case Action::ATTACK:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, enemy);
      break;
    case Action::ATTACK_HOUSE:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, Unit::NONE);
      /* this case is not well defined by Kendall's rules, need review */
      for (int i = House::WATERS; i >= 0; i--)
        {
          if (new_state.board (i) == Unit::NONE)
            {
              new_state.update_board (i, enemy);
              break;
            }
        }
      break;
    case Action::PANIC:
      new_state.update_board (choice, new_state.board(House::BEAUTY));
      new_state.update_board (House::BEAUTY, agent);
      break;
    default:
      throw std::logic_error ("corrupted game logic");
      // new_state._data.seed = 0UL; // corrupted logic
      break;
    }
  return new_state;
}
} // namespace xtc
