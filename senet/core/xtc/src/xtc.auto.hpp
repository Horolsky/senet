#pragma once
#include "xtc.dice.hpp"
#include "xtc.emax.hpp"
#include "xtc.eval.hpp"
#include "xtc.fwd.hpp"
#include "xtc.state.hpp"
#include "xtc.strategies.hpp"
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

namespace xtc
{
int
write_log_file (std::string path, std::string msg)
{
  std::ofstream file;
  file.open (path, std::fstream::app);
  file << msg;
  file.close ();
  return 0;
}

/**
 * play auto game
 * return score of the game, winner is marked by sign (+ for X, - for Y)
 */
int
play_game (uint64_t seed, int depth1, int depth2, int timer)
{
  StrategyNode snode{ seed };
  auto emax_1 = Emax (Eval (), Emax::AlgoID::RECURSIVE, depth1, timer);
  auto emax_2 = Emax (Eval (), Emax::AlgoID::RECURSIVE, depth2, timer);
  Dice dice{};
  int turn{ 0 };
  Unit last = snode.agent();
  while (!snode.is_terminal_node())
    {
      ++turn;
      Strategies strats = snode.strategies ();
      int choice = (snode.seed () & 1) ? emax_1 (snode) : emax_2 (snode);
      int move = strats.indici (choice);
      Action action = strats.actions (choice);
      if (action == Action::ERROR)
        {
          throw std::logic_error("autoplay error, seed: " + std::to_string(snode.seed()));
        }
      ChanceNode cnode = snode.choice (move);
      int chance = dice.roll ();
      snode = cnode.chance (chance);
      last = snode.agent();
    }
  int score = 0;
  for (int i = 0; i < 30; i++)
  {
    if (snode.board(i) != Unit::NONE) ++score;
  }
  return last == Unit::X ? score : -score;
}

/**
 * play n auto games
 * return report in csv row format fo headers:
 * duration; timer; first move; agent 1; depth 1; eval 1; coefs 1; agent 2; depth2; eval 2; coefs 2; winner; score
 */
std::string
autoplay (uint64_t seed, int n, int depth1, int depth2, int timer)
{
  std::string unit_id[2]{ "X", "Y" };

  std::stringstream staticmsg_s{};
  staticmsg_s << timer << ";";
  staticmsg_s << (seed & 1) << ";";
  staticmsg_s << unit_id[(seed & 1UL)] << ";";
  staticmsg_s << depth1 << ";";
  staticmsg_s << "basic;";
  staticmsg_s << "[1,0,0,0];";
  staticmsg_s << unit_id[(seed & 1UL) ^ 1UL] << ";";
  staticmsg_s << depth2 << ";";
  staticmsg_s << "basic;";
  staticmsg_s << "[1,0,0,0];";
  std::string staticmsg = staticmsg_s.str ();

  std::stringstream report{};
  for (int i = 0; i < n; ++i)
    {
      auto start = std::chrono::steady_clock::now ();
      int score = play_game (seed, depth1, depth2, timer);
      auto wtime = (std::chrono::steady_clock::now () - start);

      // duration
      report << (double)std::chrono::duration_cast<std::chrono::milliseconds> (
                    wtime)
                        .count ()
                    / 1000
             << ";";
      report << staticmsg;
      // winner
      report << unit_id[score > 0 ? 0 : 1] << ";";
      // score
      report << (score > 0 ? score : -score) << ";\n";
    }
  return report.str ();
}

} // namespace xtc
