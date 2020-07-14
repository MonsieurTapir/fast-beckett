Search Algorithm for Beckett-Gray codes
===
## What are Beckett-Gray codes
For any integer *n*, a *n*-dimensional Gray code is a Hamiltonian cycle of the *n*-dimensional hypercube. A Beckett-Gray code has the additionnal property that it can be traversed in a direction ensuring that whenever a bit is turned off it must be the one that has been on for the longest time.
### Articles 
[A note on Beckett-Gray codes (Cooke, North, Dewar, Stevens))](https://arxiv.org/abs/1608.06001)
[A fast algorithm to generate Beckett-Gray codes (Sawada, Wong)](http://www.cis.uoguelph.ca/~sawada/papers/beckett_endm.pdf)

Usage
------

Compile with `make`. Run with `./bin/beckett <N>` where *N* is the dimension of the looked for code.
Additionnal options can be provided:

*  `-c <long>` searches for `<long>` codes instead of just one
*  `-f` use connectivity cuts (as described in Sawada&Wong's article)
*  `-s` use degree sorting

Code structure
------
Standard backtracking algorithm in class `Search` (**search.cc**) for finding hamiltonian cycles in the hypercube of dimension N.
Abstract class `AbstractCut` defined in **cuts.h** provides tools to implement cutting heuristics.
```c++
 class AbstractCut{
 public:
  virtual fint apply(const fint&){};
  virtual void undo(const fint&){};
};
```
Cutting heuristics should extend this class and implement the methods above. `fint apply(const fint& x)` should return *0* when the current node **x** won't lead to a solution, a value between *1* and *2^N* if it wishes to force a vertex after **x** and anything greater than *2^N* otherwise. `void undo(const fint& x)` should undo changes to internal data structures that might have occurred during the application of the heuristic. Then the heuristic should be added to the search using `Search::add_heuristic(AbstractCut*)`. For instance in **main.cc**:
```c++
  ...
  
  Search* s=new Search(N,M,tar);
  
  PendantVertices* c=new PendantVertices(s); 
  s->add_heuristic(c);
  ...
```

Same goes for neighbor-sorting heuristics which should extend `AbstractSort` :

```c++
class AbstractSort{
 public:
  virtual std::vector<fint>* sort_neighbors(const fint&){};
};
```
and implement `vector<fint>* sort_neighbors(const fint& x)` which given a node **x** should return its neighbors in a vector. Then the sorting heuristic should be added using `Search::set_sort(AbstractSort *)`.

Sorting and cutting heuristics are used in `Search::branch(const fint&)`.
