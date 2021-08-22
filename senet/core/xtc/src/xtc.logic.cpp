#include "xtc.logic.hpp"

namespace xtc
{
/*

  Strategies strategies_kendall () const;
  Strategies strategies_meub () const;
  State increment_kendall (int, Strategies) const;
  State increment_meub (int, Strategies) const;
  void update_board (int index, Unit unit);


  using increment_f = State (State::*) (int, Strategies) const;
  using strategies_f = Strategies (State::*) () const;
  static uint64_t build_seed (Unit agent, int steps, int *board);
  static strategies_f get_strategies_f (Rules);
  static increment_f get_increment_f (Rules);
*/
Strategies
FuncStrategies::kendall (const State state, int steps)
{
  Strategies strategies (0UL);
  if (!steps)
  {
    if (state.is_chance_node())
    return strategies;
    else steps = state.steps();
  } 
  Unit agent = state.agent ();
  Unit enemy = static_cast<Unit> (state._data.agent ^ 1);

  /* HOUSE OF WATER PENALTY */
  if (state.board (House::WATERS) == agent)
    {
      if (steps == 4)
        {
          strategies.push (House::WATERS, Action::ESCAPE);
        }
      else if (state.board (House::REBIRTH) == Unit::NONE)
        {
          strategies.push (House::REBIRTH, Action::DROW); // reborning
          strategies.push (House::SKIPTURN, Action::SKIP);
        }
      return strategies;
    }
  /* DIRECT MOVE */
  for (int i = 0; i < State::board_size; i++)
    {
      Unit cell_occupation = state.board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + steps;
      Unit trgt = state.board (dest);
      if (i == House::SCARAB)
        strategies.push (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        strategies.push (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
        continue; // corrupted logic
      else if (trgt == Unit::NONE)
        strategies.push (i, Action::MOVE);
      else if (trgt == enemy)
        { // attack
          if (dest == House::WATERS)
            strategies.push (i, Action::ATTACK); // drowed trgt
          else
            {
              Unit prev = dest > 0 ? state.board (dest - 1) : Unit::NONE;
              Unit nxt
                  = dest < House::SCARAB ? state.board (dest + 1) : Unit::NONE;
              if (prev != enemy && nxt != enemy)
                strategies.push (i, dest > House::WATERS ? Action::ATTACK_HOUSE
                                                         : Action::ATTACK);
            }
        }
    }
  /* REVERSE MOVE */
  if (strategies.mobility () == 0)
    {
      Unit trgt = state.board (0);
      for (int i = 1; i < State::board_size; i++)
        {
          Unit cell = state.board (i);

          if (cell == agent && trgt == Unit::NONE)
            strategies.push (i, Action::RETREAT);
          else if (cell == agent && trgt == enemy)
            {
              if (i == House::TRUTHS)
                strategies.push (i, Action::SWAPBACK); // drowed trgt
              else
                {
                  Unit prev = i > 1 ? state.board (i - 2) : Unit::NONE;
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
  return strategies;
}

Strategies
FuncStrategies::meub (const State state, int steps)
{
  Strategies strategies (0UL);
  if (!steps)
  {
    if (state.is_chance_node())
    return strategies;
    else steps = state.steps();
  } 
  Unit agent = static_cast<Unit> (state._data.agent);
  Unit enemy = static_cast<Unit> (state._data.agent ^ 1);

  /* DIRECT MOVE */
  for (int i = 0; i < State::board_size; i++)
    {
      Unit cell_occupation = state.board (i);
      if (cell_occupation != agent)
        continue;
      int dest = i + steps;
      Unit trgt = state.board (dest);
      if (i == House::SCARAB)
        strategies.push (i, Action::ESCAPE); // always escaping
      else if (i >= House::BEAUTY && dest == House::NETHER)
        strategies.push (i, Action::ESCAPE); // correct escaping
      else if (i != House::BEAUTY && dest > House::BEAUTY)
        continue; // forbidden house arriving
      else if (dest > House::SCARAB)
        continue; // forbidden escaping
      else if (dest == House::WATERS)
        strategies.push (i, Action::DROW); //
      else if (trgt == Unit::NONE)
        strategies.push (i, Action::MOVE); // normal movement
      else if (trgt == enemy)
        { // attack
          Unit prev = dest > 0 ? state.board (dest - 1) : Unit::NONE;
          Unit nxt
              = dest < House::SCARAB ? state.board (dest + 1) : Unit::NONE;
          if (prev != enemy && nxt != enemy)
            strategies.push (i, dest > House::WATERS ? Action::ATTACK_HOUSE
                                                     : Action::ATTACK);
        }
    }
  /* REVERSE MOVE */
  if (strategies.mobility () == 0)
    {
      Unit trgt = state.board (0);
      for (int i = 1; i < House::NETHER; i++)
        {
          Unit cell = state.board (i);
          if (cell == agent && trgt == Unit::NONE)
            strategies.push (i, Action::RETREAT);
          trgt = cell;
        }
    }
  if (strategies.mobility () == 0)
    {
      strategies.push (House::SKIPTURN, Action::SKIP);
    }
  return strategies;
}

Strategies
FuncStrategies::operator() (const State state, int steps) const
{
  if (_rules == Rules::KENDALL)
    {
      return FuncStrategies::kendall (state, steps);
    }
  else if (_rules == Rules::MEUB)
    {
      return FuncStrategies::meub (state, steps);
    }
  else
    throw "not implemented";
}

Rules FuncStrategies::rules() const
{
    return _rules;
}

State
FuncIncrement::kendall (const State state, const int choice, Strategies strategies)
{
  if (state.is_chance_node()) throw std::logic_error("cannot increment chance node");
  if (strategies.seed() == 0) strategies = FuncStrategies(Rules::KENDALL)(state);
  if (strategies.mobility() > 0 && !strategies.contains (choice))
    throw std::logic_error("illegal move");

  Unit agent = state.agent();
  Unit enemy = static_cast<Unit> (state._data.agent ^ 1);
  int steps = state.steps();

  /* next step preparation */
  State new_state (state);
  new_state._data.steps = 0;
  // next agent rule
  if (steps == 1 || steps == 4 || steps == 5)
    new_state._data.agent = state._data.agent; // bonus move
  else
    new_state._data.agent = (state._data.agent ^ 1); // normal move

  Action action = strategies.actions (choice);

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
    case Action::ATTACK:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, enemy);
      break;
    case Action::ATTACK_HOUSE:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, Unit::NONE);
      for (int i = House::WATERS; i >= 0; i--)
        {
          if (new_state.board (i) == Unit::NONE)
            {
              new_state.update_board (House::WATERS, enemy);
              break;
            }
        }
      break;
    default:
      new_state._data.seed = 0UL; // corrupted logic
      break;
    }

  return new_state;
}

State
FuncIncrement::meub (const State state, const int choice, Strategies strategies)
{
  if (state.is_chance_node()) throw std::logic_error("cannot increment chance node");
  if (strategies.seed() == 0) strategies = FuncStrategies(Rules::KENDALL)(state);
  if (strategies.mobility() > 0 && !strategies.contains (choice))
    throw std::logic_error("illegal move");

  Unit agent = state.agent();
  Unit enemy = static_cast<Unit> (state._data.agent ^ 1);
  int steps = state.steps();

  /* next step preparation */
  State new_state (state);
  new_state._data.steps = 0;
  // next agent rule
  if (steps == 1 || steps == 4 || steps == 5)
    new_state._data.agent = state._data.agent; // bonus move
  else
    new_state._data.agent = (state._data.agent ^ 1); // normal move

  Action action = strategies.actions (choice);

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
      for (int i = House::REBIRTH; i >= 0; i--)
        {
          if (new_state.board (i) == Unit::NONE)
            {
              new_state.update_board (i, agent);
              break;
            }
        }
      break;
    case Action::ATTACK:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, enemy);
      break;
    case Action::ATTACK_HOUSE:
      new_state.update_board (choice + steps, agent);
      new_state.update_board (choice, Unit::NONE);
      for (int i = House::REBIRTH; i >= 0; i--)
        {
          if (new_state.board (i) == Unit::NONE)
            {
              new_state.update_board (i, enemy);
              break;
            }
        }
      break;
    default:
      new_state._data.seed = 0UL; // corrupted logic
      break;
    }

  return new_state;
}

State
FuncIncrement::operator() (const State state, const int choice, Strategies strategies) const
{
  if (_rules == Rules::KENDALL)
    {
      return FuncIncrement::kendall (state, choice, strategies);
    }
  else if (_rules == Rules::MEUB)
    {
      return FuncIncrement::meub (state, choice, strategies);
    }
  else
    throw "not implemented";
}
}