#include "../src/xtc.emax.hpp"
#include <chrono>
#include <iostream>
#include <string>
using namespace xtc;
using namespace std;

int main(int argc, char* argv[])
{
    
    uint64_t seed = StrategyNode(State(),1).seed();
    int depth = 4;
    int time = 1000;

    if (argc > 1) seed = stoull(argv[1]);
    if (argc > 2) depth = stoi(argv[2]);
    if (argc > 3) time = stoi(argv[3]);

    auto e = Emax(Eval(), Emax::AlgoID::RECURSIVE,depth,time);
    auto s = StrategyNode(seed);
    int nodes, leaves, threads, jobs;

    auto start = chrono::steady_clock::now();
    int choice = e.test_call(s, &nodes, &leaves, &threads, &jobs);
    auto wtime = (chrono::steady_clock::now() - start);
    
    cout << "seed: " << s.seed() << endl;
    cout << "depth arg: " << depth << endl;
    cout << "time arg: " << time << endl;
    cout << "choice res: " << choice << endl;
    cout << "nodes visited: " << nodes << endl;
    cout << "leaves visited: " << leaves << endl;
    cout << "threads created: " << threads << endl;
    cout << "jobs finished: " << jobs << endl;
    cout << "working time: " << chrono::duration_cast<chrono::milliseconds>(wtime).count() << endl;
    return 0;
}