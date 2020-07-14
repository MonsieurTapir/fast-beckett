#include "search.h"

using namespace std;

/**
 * Constructor takes the dimension (N), the size of the code (M) and the number of codes to search (tar)
 */
Search::Search(fint N,fint M,long long tar,fint start): N(N),M(M),TAR(tar),start(start){
  seen=new bool[M];
  sol=new fint[M];
  pows=new fint[N];
  fint k=1;
  for(fint i=0; i<N;i++){
    pows[i]=k;
    k*=2;
  }
  for(fint i=0; i<M;i++){
    seen[i]=false;
  }
  sort_strategy=new DefaultSort(this);
}
Search::~Search(){
  for(auto c:cut_heuristics){
    delete c;
  }
  cut_heuristics.clear();
  delete sort_strategy;
  delete[] seen;
  delete[] sol;
  delete[] pows;
}

/**
 * Update data structures according to the next vertex to visit, check the branch return value and reverse the changes.
 */
bool Search::go_deeper(const fint& next){
  sol[depth]=next;
  seen[next]=true;
  depth++;
  
  if(branch(next))
    return true;
  
  depth--;
  seen[next]=false;
  
  return false;
}

/**
 * Decide whether or not the search should consider "next" 
 */
bool Search::process(const fint& current, const fint& next,const fint& i){
  if(!seen[next]){ // If next has yet to be visited
    if(next>current){ // If next is greater than current then i has been turned on
      beckett_queue.push_back(i); // Adding i to the beckett queue
      if(go_deeper(next)) // Updates to data structures then the search continues
	return true; 
      beckett_queue.pop_back(); // Reversing changes to the queue
    }else if(i==beckett_queue.front()){ // Else next is smaller and i has been turned off. Only possible if it is the first in queue.
      beckett_queue.pop_front(); // Removing i from the beckett queue
      if(go_deeper(next))
	return true;
      beckett_queue.push_front(i); // Reversing changes to the queue
    }
  }
  return false;
}

/*
 * Standard recursive depth-first backtracker.
 * Rule of thumb : when a deeper call returns true the program has to stop hence the multiple "if then return true"
 */
bool Search::branch(const fint& current){
  nodes++;
  if(depth==M && is_neighbor(current)){ // Current node is the last one in the path, we check that it is neighbor of 0 and display it.
    found++;
    display();
    if(found==TAR) // If we found the number of codes we wanted, we return true !
      return true;
    return false; 
  }
  
  fint forced=N+1;
  int k; // Persistent variable to keep track of the heuristics applied
  for(k=0; k<cut_heuristics.size();k++){ // Applying the cutting heuristics
    fint temp=cut_heuristics[k]->apply(current);
    if(forced>N) // If forced still has its default value, take the new value
      forced=temp;
    else if(temp<=N && temp!=forced) // If forced was not default and the new cut forces another vertex there is a problem
      forced=0;
    if(forced==0) // If forced is 0 then we prune this branch
      break;
  }
  
  if(forced>N){ // Default value, no vertex forced
    for(auto x:*sort_strategy->sort_neighbors(current)){ // Iterate through the neighbors using the sort strategy
      if(process(current,x,current^x))
	return true;
    }
  }
  else if(forced>0){ // A vertex is forced 
    forced_vertices++;
    if(process(current,pows[forced-1]^current,pows[forced-1]))
      return true;
  }
 
  for(int i=0;i<=k && i<cut_heuristics.size();i++){ // Undoing the cutting operations
    cut_heuristics[i]->undo(current);
  }
  
  return false;
  }

/*
 * Standard recursive depth-first backtracker.
 * Rule of thumb : when a deeper call returns true the program has to stop hence the multiple "if then return true"
 *
bool Search::branch(const fint& current){
  nodes++;
  if(depth==M && is_neighbor(current)){ // Current node is the last one in the path, we check that it is neighbor of 0 and display it.
    found++;
    display();
    if(found==TAR) // If we found the number of codes we wanted, we return true !
      return true;
    return false; 
  }
  
  fint forced=N+1;
  int k; // Persistent variable to keep track of the heuristics applied
  for(k=0; k<cut_heuristics.size();k++){ // Applying the cutting heuristics
    fint temp=cut_heuristics[k]->apply(current);
    if(forced>N) // If forced still has its default value, take the new value
      forced=temp;
    else if(temp<=N && temp!=forced) // If forced was not default and the new cut forces another vertex there is a problem
      forced=0;
    if(forced==0) // If forced is 0 then we prune this branch
      break;
  }

  if(forced>N){
    for(fint i=0; i<=maxpos;i++){ // Iterate through the neighbors using the sort strategy
      if(process(current,current^pows[i],pows[i]))
	return true;
    }
    if(maxpos<N-1){
      maxpos++;
      if(process(current,current^pows[maxpos],pows[maxpos]))
	return true;
      maxpos--;
    }
  }else if(forced>0){
    forced_vertices++;
    forced=forced-1;
    if(process(current,current^pows[forced],pows[forced]))
      return true;
    
  }

  for(int i=0;i<=k && i<cut_heuristics.size();i++){ // Undoing the cutting operations
    cut_heuristics[i]->undo(current);
  }
  
  return false;
}
*/
void Search::display(){
  for(fint i=0;i<M;i++){
    printf("%d ",sol[i]);
  }
  printf("\n");
}


int Search::go(){
  sol[0]=0;
  seen[0]=true;
  
  if(start>0){
    for(int k=0; k<cut_heuristics.size();k++){ // Applying the cutting heuristics
      cut_heuristics[k]->apply(0);
    }

    beckett_queue.push_back(start);
    seen[start]=true;
    sol[depth]=start;
    
    depth++;
  }
  cout << "Searching beckett-gray code of dimension " << N << " (size " << M << ")" << " starting with  "<< start << " initial beckett queue : [" << beckett_queue.front() <<"]" <<  endl;
  clock_t begin = clock();
  branch(start);
  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  
  cout << "Codes found : " << found << endl;
  cout << "Elapsed time : " << elapsed_secs << "s"<< endl;
  return found;
}

void Search::print_details(){
  cout << "Nodes explored : " <<get_nodes() << endl;
  for(auto c:cut_heuristics){
    c->print_details();
  }
  cout << " > Vertices forced : " << get_forced() << endl;
}

unsigned long long Search::get_nodes(){
  return this->nodes;
}

unsigned long long Search::get_cuts(){
  return this->cuts;
}

unsigned long long Search::get_forced(){
  return this->forced_vertices;
}
fint* Search::get_code(){
  return sol;
}
fint Search::get_N(){
  return N;
}
fint Search::get_M(){
  return M;
}
bool* Search::get_seen(){
  return seen;
}
fint* Search::get_pows(){
  return pows;
}
void Search::add_heuristic(AbstractCut* c){
  this->cut_heuristics.push_back(c);
}

void Search::set_sort(AbstractSort* s){
  if(this->sort_strategy!=0)
    delete sort_strategy;
  this->sort_strategy=s;
}
