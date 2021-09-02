TEST (Logic, ATTACK_HOUSE):
corrupted board after move, contains 3

```c++
int board[30];
empty_board(board);
board[25] = 0;
board[29] = 1;
int chance = 4;
auto state = xtc::StrategyNode(xtc::State::build_seed(xtc::Unit::X, chance, board));
    
EXPECT_EQ(state.strategies().actions(0), xtc::Action::ATTACK_HOUSE);
    
state = state.child(25).child(3);
//_data.board value after this action:
// 0b1010000010101010101010101010101010101110101010101010101010

//problem in xtc.state.cpp StrategyNode::child function, line 221:
//init:
//01 10 10 10 00 1010101010 10101010101010101010 10101010101010101010
//before:
//00 10 10 10 00 1010101010 10101010101010101010 10101010101010101010
//   0b001010100010101010101010101010101010101010101010101010101010
//after:

//expected:
//00 10 10 01 10 1010101010 10101010101010101010 10101010101010101010

//new_state.update_board (choice, Unit::NONE);
//does nothing


```

