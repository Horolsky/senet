//seed=12143157482611190434
#include "../src/xtc.state.hpp"
#include "../src/xtc.strategies.hpp"
#include <iostream>
#include <string>
using namespace xtc;
using namespace std;
void printboard(const State &node)
{
    const string units[3] {"X ","Y ","_ "};
    cout << "board:\n";
        for (int i = 0; i < 10; i++)
        {
            cout << units[int(node.board(i))];
        }
        cout << endl;
        for (int i = 19; i >= 10; i--)
        {
            cout << units[int(node.board(i))];
        }
        cout << endl;
        for (int i = 20; i < 30; i++)
        {
            cout << units[int(node.board(i))];
        }
        cout << endl;
}
int main(int argc, char* argv[])
{
    uint64_t seed = 12143157482611190434;
    
    if (argc > 1)
    {
        seed = stoull(argv[1]);
    }
    auto state = State(seed);
    
    string actions[11] {
        "SKIP",
        "MOVE",
        "RETREAT",
        "SWAPBACK",
        "DROW",
        "ATTACK",
        "ATTACK_HOUSE",
        "ESCAPE",
        "PANIC",
        "NONE", // for game start
        "ERROR" // reserved
        };
    if (state.is_chance_node())
    {
        auto node = ChanceNode(seed);
        Unit agent = node.agent();
        cout << "seed: " << seed << endl;
        cout << "type: ChanceNode\n";
        cout << "agent: " << int(agent) << endl;
        printboard(node);
    }
    else if (state.is_strategy_node())
    {
        auto node = StrategyNode(seed);
        Unit agent = node.agent();
        auto strats = node.strategies();
        cout << "seed: " << seed << endl;
        cout << "type: StrategyNode\n";
        cout << "agent: " << int(agent) << endl;
        cout << "steps: " << node.steps() << endl;
        cout << "mobility: " << strats.mobility() << endl;
        for (int i = 0; i < strats.mobility(); i++)
        {
            cout << strats.indici(i) << ": " << actions[int(strats.actions(i))] << endl;
        }
        printboard(node);
    }
    return 0;
}