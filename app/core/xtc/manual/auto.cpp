#include "../src/xtc.auto.hpp"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
    uint64_t seed = xtc::StrategyNode(xtc::State(),1).seed();
    int n_of_plays {1};
    int timer {1000};
    int depth1 {0};
    int depth2 {0};

    if (argc > 1) seed = stoull(argv[1]);
    if (argc > 2) n_of_plays = stoi(argv[2]);
    if (argc > 3) timer = stoi(argv[3]);
    if (argc > 4) depth1 = stoi(argv[4]);
    if (argc > 5) depth2 = stoi(argv[5]);

    

    cout << xtc::autoplay(seed, n_of_plays, depth1, depth2, timer) << endl;
    return 0;
}