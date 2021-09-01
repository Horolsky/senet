# Iterative optimisation of decision tree backward induction
this optimisation focuses on specific expectiminimax algorithm but can be extrapolated to any decision tree backward induction problem

## Main concepts: 
- n-ary heap storage for the last level of the decision tree
- backward induction process utilizes the strict indexation properties of n-ary heap
- priority queue interface wraps the n-ary heap to facilitate the the levelwise tree update (BFS)
- subject specific: decision tree packing through the aggregate node type
- subject specific: if tree is sparsed, lacunae in heap can be reused for the forward work 

### Advantages over the trivial recursive methods:
- gives better intermediate result if work is not finished due to Zeitnot  
- forward working  
- more efficient parallelization  
- efficient stack memory usage in case with deep low-branching algorithms  

### Disadvantages
- memory greedy  
- complex implementation  

## Abbreviations & terms
**D**: global decision tree depth  
**B**: max node branching factor  
**S**: strategy node type (choice to be made)  
**C**: chance node type (dice to be thrown)  
**P**: chance probability function

Lower-case letters stands for local variables  
Depth, element indexation and subnodes indexation starts from 0, i. e. heap[0] is a root element of zero depth

NB: **n-ary heap** in some sources is named as *d-heap*, and binary heap as *B-heap*. 
As we are using here `D` for depth and `B` for branching factor, the `B-heap` means *B-ary heap*.

## Process
- allocate a storage of `B^D` capacity  
- build a tree on initial launch and store the last level  
- on game iteration update the stored data in place  
- get expectimax values using the heap indexation  
- if the time exceeded, use previously calculated values to return expectimax with `d = D-1`  
- if the tree is sparsed, use lacunae to work forward (needs additional indexation bookkeeping)  
- optionally shrink the storage on *Endspiels*, when `b` decrease is guaranteed  
- use a thread pool (*producer-consumer*) to iterate algorithm  

## Ply node vs Atomic Node
As the game includes chance events, each game **ply** represents a tree with strategy node S at the root and chance nodes C as it's leafs.  
The whole game decision tree can be represented either as an atomic tree with nodes of both S and C types, or as a molecular (packed) tree with aggregate nodes (Ply nodes).  
Packed tree branching factor `B(Ply) = B(S) * B(C)`  
Packed node technique helps to reduce memory usage, as C nodes differs from S node only by few bits, needed to store the chance index  

![Tree packing illustration](../drawio/tree_packing.svg)

## Data storage

the tree leafs storage is a data structure that utilizes both B-heap and priority queue properties:  
- fixed-size buffer with cyclic pointers to head/tail  
- push/pop/peek methods  
- random access by index, wrapped above the head/tail pointers  
- B-heap indexation helps to bookkeep the parent/child connections in a tree  

### Math properties of B-heap 
- max n of leaves (working level) = B^D  
- ratio of inner nodes to leafs:  
    - R (depth = 0) = 1:B  
    - lim{depth -> inf} R (depth) = 1:(B-1)  
- d(node) =~ log(node, B) + 1

### B-Heap Indexation 

B-heap node position determining for tree with an arbitrary branching:

```python

def parent_index(node_index: int):
    return (node_index-1) // B

def child_index(node_index: int, i: int):
    return B * node_index + 1 + i

def depth(node_index: int):  
    d = 0
    lvl = 0
    while(lvl <= node_index):
        lvl += (B^d)
        d += 1
    return d-1

```

### Tree processing

![Tree update](../drawio/tree_update.svg)

#### Priority queue initialization
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

#### Inplace leafs update
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
    # the queue head is guaranteed to point to the beginning of the source branch
    for _ in range(B^(D-1)):    
        for child in n_queue.pop(): 
            n_queue.push(child) 
    return n_queue
```

NB: dead subbranches/leafs are not handled in this pseudocode.  
Use random access with B-heap indexation to workaround it.