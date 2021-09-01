# Tree-packing algorithms
Optimisation of iterative expectimax BFS using **n-ary heap** as priority queue  

## Abbreviations & terms
**D**: global decision tree depth  
**B**: max node branching factor  
**S**: strategy node type (choice to be made)  
**C**: chance node type (dice to b thrown)  
**P**: chance probability function

Lower-case letters stands for local variables  
Depth, element indexation and subnodes indexation starts from 0, i. e. heap[0] is root element of depth

NB: **n-ary heap** in some sources is named as *d-heap*, and binary heap as *B-heap*. 
As we are using D for depth and B for branching factor, here B-heap means heap of B-arity.

## Ply node vs Atomic Node
As the game includes chance events, each game **ply** represents a tree with strategy node at the root and chance nodes as it's leafs.  
The whole game decision tree can be represented either as an atomic tree with nodes of both S and C types, or as a molecular (packed) tree with aggregate nodes (Ply nodes).  
Packed tree branching factor `B(Ply) = B(S) * B(C)`  

![Tree packing illustration](../drawio/tree_packing.svg)

## Math properties of B-heap 
- max n of leaves (working level) = B^D  
- ratio of inner nodes to leafs:  
    - R (depth: 0) = 1:B  
    - R (depth: lim->inf) = 1:(B-1)  
- d(node) =~ log(node, B) + 1


## Basic idea
- use fixed-size queue with random access   
- allocate queue of `B^D` capacity  
- build tree on initial launch and keep the last level in queue  
- update in place only one tree level on each iteration  
- backtrack expectimax values using heap indexation  
- use previously calculated values to return expectimax with `d = D-1` if time exceeded  
- if tree is sparsed, use free space to work forward, (needs additional indexation bookkeeping)  
- optionally shrink the queue on *Endspiels*, when `b` decrease is guaranteed  
- use thread pool (*producer-consumer*) for state iteration (hic random access needed)

## B-Heap Indexation 

B-heap node position determining for tree with an arbitrary branching:

```python

def parent_index(node: int):
    return (node-1) // B

def child_index(node: int, i: int):
    return B * node + 1 + i

def depth(node: int):   #indexation from 0
    d = 0
    li = 0      #index of first node in level
    while(li <= node):
        li += (B^d)
        d += 1
    return d-1

```

## Emax

### Priority queue initialization
```python
def init_queue(node: Node):
    d_queue = Queue(size=B^D) # depth bookkeeping
    n_queue = Queue(size=B^D) # working nodes storage
    d_queue.push(0)
    n_queue.push(node)
    while peek(d_queue) < D:
        d = d_queue.pop()
        for child in n_queue.pop(): 
            n_queue.push(child) 
            d_queue.push(d+1)
    return n_queue
```

### Inplace leafs update
```python
def cut_dead_leafs(tree: Queue):
    """
    This subroutine updates the inner head/size properties,
    cutting the unused brunches from previous iteration.
    Working leafs group of size B^(D-1) would be used as a source for the new iteration.
    This process affects only inner indexation mechanism of the container,
    actual data is not mutated or deleted
    """

def update_tree(n_queue: Queue):
    cut_dead_leafs(tree)
    for i in range(B^(D-1)):
        for child in n_queue.pop(): 
            n_queue.push(child) 
    return n_queue
```

NB: dead subbranches/leafs are not handled in this pseudocode.  
Use random access with B-heap indexation to workaround it.