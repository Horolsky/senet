/**
 * Strategies class
 */
#pragma once
#include "xtc.fwd.hpp"

namespace xtc
{
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
  friend StrategyNode;

  using seed_type = uint64_t;
  static const int agent_offset{ 1 };
  static const int mobility_offset{ 3 };
  static const int indici_offset{ 5 };
  static const int actions_offset{ 5 };
  static const int max_strategies{ 5 };

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
