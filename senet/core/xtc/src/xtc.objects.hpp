/**
 * game object classes declarations
 * - Dice
 * - State
 * - Strategies
 */
#pragma once
#include "xtc.fwd.hpp"
#include <array>
namespace xtc
{

/**
 * game state node
 * if 0 steps - chance node
 * else - strategy node
 */
class State
{
  /**
   * 1 bit: agent
   * 3 bits: steps
   * 30x2bits: board
   */
  typedef struct
  {
    union
    {
      uint64_t seed;
      struct
      {
        uint64_t agent : 1;  // current ply active agent
        uint64_t steps : 3;  // 1-5, 0 for unset
        uint64_t board : 60; // 30x array,
      };
    };
  } bitfield;
  bitfield _data{ .seed = 0UL };

public:
  friend class FuncStrategies;
  friend class FuncIncrement;
  friend class Eval;
  friend class Expectimax;

  using seed_type = uint64_t;
  // 0b1010101010101010101010101010101010101010010001000100010001000000
  static const uint64_t def_state_seed{ 12297829382466323520UL };
  static const int agent_offset{ 1 };
  static const int steps_offset{ 3 };
  static const int board_offset{ 2 };
  static const int board_size{ 30 };
  static uint64_t build_seed (Unit agent, int steps, int *board);

  State () { _data.seed = def_state_seed; }
  State (uint64_t seed) { _data.seed = seed; }
  State (const State &other) : _data (other._data) {}
  State (State &&other) : _data (other._data) {}

  State (Unit agent, int steps, int *board)
  {
    _data.seed = build_seed (agent, steps, board);
  }

  ~State () = default;

  State &operator= (const State &other);
  State &operator= (State &&other);
  void set_agent (Unit agent);
  void set_steps (int steps);

  /**
   * loads game state to an array buffer of size 30
   * return buffer pointer
   * lookup! raw pointers, size not checked
   */
  int *board (int *buffer) const;
  // return unit id on a given index
  Unit board (int) const;

  Unit agent () const;
  int steps () const;
  uint64_t seed () const;

  bool is_chance_node () const;
  bool is_strategy_node () const;
  bool is_terminal_node () const;

private:
  void update_board (int index, Unit unit);
}; // class State

/**
 * legal moves from a given strategy state node
 */
class Strategies
{
  /**
   * 1 bit: agent
   * 3 bits: mobility
   * 7x5bits: indici
   * 7x3bits: actions
   */
  typedef struct
  {
    union
    {
      uint64_t seed;
      struct
      {
        uint64_t agent : 1;
        uint64_t mobility : 3;
        uint64_t direction : 4; // NB: redundant field
        uint64_t indici : 35;
        uint64_t actions : 21;
      };
    };
  } bitfield;
  bitfield _data{ .seed = 0UL };
  int push (int index, Action action);

public:
  friend class FuncStrategies;
  friend class FuncIncrement;
  friend class Expectimax;

  using seed_type = uint64_t;
  static const int agent_offset{ 1 };
  static const int mobility_offset{ 3 };
  static const int indici_offset{ 5 };
  static const int actions_offset{ 5 };
  static const int max_strategies{ 7 };

  static uint64_t build_seed (Unit agent, int mobility, int *indici,
                              int *actions);

  Strategies () = default;
  Strategies (uint64_t seed) { _data.seed = seed; }
  Strategies (const Strategies &other) : _data (other._data) {}
  Strategies (Strategies &&other) : _data (other._data) {}

  Strategies (Unit agent, int mobility, int *indici, int *actions)
  {
    _data.seed = build_seed (agent, mobility, indici, actions);
  }

  ~Strategies () = default;

  Strategies &operator= (const Strategies &other);
  Strategies &operator= (Strategies &&other);

  int *indici (int *buffer) const;
  int indici (int index) const;
  int *actions (int *buffer) const;
  Action actions (int index) const;

  Unit agent () const;
  int mobility () const;
  bool contains (int) const;
  uint64_t seed () const;
}; // class Strategies

} // namespace xtc
