#include "../src/xtc.emax.hpp"
#include <chrono>
#include <iostream>
#include <string>
using namespace xtc;
using namespace std;

int main(int argc, char* argv[])
{
    int depth = 4;
    int time = 1000;
    if (argc > 1)
    {
        depth = stoi(argv[1]);
    }
    if (argc > 2)
    {
        time = stoi(argv[2]);
    }
    auto e = Emax();
    auto s = StrategyNode(State(),1);
    
    auto start = chrono::steady_clock::now();
    int choice = e(s,depth,time);
    auto wtime = (chrono::steady_clock::now() - start);
    
    cout << "seed: " << s.seed() << endl;
    cout << "depth arg: " << depth << endl;
    cout << "time arg: " << time << endl;
    cout << "choice res: " << choice << endl;
    cout << "working time: " << chrono::duration_cast<chrono::milliseconds>(wtime).count() << endl;
    return 0;
}