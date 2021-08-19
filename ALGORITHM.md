
## expectiminimax pseudocode

### Wikipedia version
```python
def expectiminimax(node, depth)
    if node is a terminal node or depth = 0
        return the heuristic value of node
    if the adversary is to play at node
        # Return value of minimum-valued child node
        let α := +∞
        foreach child of node
            α := min(α, expectiminimax(child, depth-1))
    else if we are to play at node
        # Return value of maximum-valued child node
        let α := -∞
        foreach child of node
            α := max(α, expectiminimax(child, depth-1))
    else if random event at node
        # Return weighted average of all child nodes' values
        let α := 0
        foreach child of node
            α := α + (Probability[child] × expectiminimax(child, depth-1))
    return α
```

### DFS version
one iteration includes both decision and chance  
TURNS = finite set of possible turns  

```python
def minimax():
    "return min or max depending on the players role"
def update():
    "applies chance data to chance node, in our case updates steps"
def subnodes():
    "return set of all possible subnodes of decision node"
def evaluate():
    "return math expectation of node without forward searching"

def expectiminimax(minimax, chance_node, depth):
  
  if chance_node is terminal or depth is 0: 
    return evaluate(chance_node)
  
  expectation = minimax(0, 1)
  for chance in CHANCES:
    decision_node = update(chance_node, chance)
    chance_expect = 0
    strategies = subnodes(decision_node)
    
    for subnode in subnodes:
      chance_expect += expectiminimax(subnode, depth-1)

    expectation = minimax(expectation, chance_expect)
    
  return expectation
```