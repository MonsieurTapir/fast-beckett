#include "heuristics.h"
#include "search.h"
using namespace std;


/**
 * DefaultSort simply computes the neighborhoods for each vertex and returns it in O(1) when sort_neighbors is called
 */
DefaultSort::DefaultSort(Search* s){
  neighbors=new vector<fint>*[s->get_M()];
  for(fint i=0; i<s->get_M();i++){
    neighbors[i]=new vector<fint>();
    for(fint j=0;j<s->get_N();j++){
      neighbors[i]->push_back(i^(s->get_pows()[j]));
    }
  }
  this->M=s->get_M();
}
DefaultSort::~DefaultSort(){
  for(int i=0;i<M;i++){
    delete neighbors[i];
  }
  delete neighbors;
}
vector<fint>* DefaultSort::sort_neighbors(const fint& x){
  return neighbors[x];
}

bool DegreeSort::authorized(const fint& a, const fint& b){
  fint mask=pows[N-1]+pows[N-2];
  fint a_res=a&mask;
  fint b_res=b&mask;

  if(a_res==b_res){
    return true;
  }else if( (a_res==0 && b_res==pows[N-2])
	    || (a_res==pows[N-2] && b_res==mask)
	    || (a_res==mask && b_res==pows[N-1])
	    || (a_res==pows[N-1] && b_res==0))
    return true;
  return false;
}
/**
 * DegreeSort will sort neighbors of node x on call of sort_neighbors(x) according to their remaining degree in the graph
 */
DegreeSort::DegreeSort(Search* s,fint* degrees,const fint& N,const fint& M){
  this->degrees=degrees;
  this->N=N;
  this->M=M;
  this->pows=s->get_pows();

  this->neighbors=new vector<fint>*[M];
  for(int i=0; i<M;i++){
    this->neighbors[i]=new vector<fint>();
    int k=1;
    for(fint j=0;j<N;j++){
      // if(authorized(i,i^k))
	neighbors[i]->push_back(i^k);
      k*=2;
    }
  }
}
DegreeSort::~DegreeSort(){
  for(int i=0;i<M;i++){
    delete neighbors[i];
  }
  delete neighbors;
}
vector<fint>* DegreeSort::sort_neighbors(const fint& x){
  sort(neighbors[x]->begin(), neighbors[x]->end(),[this,x](const fint& a,const fint& b){
      if(this->degrees[a]==this->degrees[b])
	return ((a>x && b<x )||( a>x && b>x && a<b));
      else
	return (this->degrees[a]>this->degrees[b]);
    });

  return neighbors[x];
}
unsigned long long AbstractCut::get_count(){
  return this->count;
}
void AbstractCut::print_details(){
  cout << "> " << name << ": " << count << " cuts " << endl;
}
PendantVertices::PendantVertices(Search* s){
  this->name="Pendant vertices";
  this->N=s->get_N();
  this->seen=s->get_seen();
  this->pows=s->get_pows();
  this->neighbors=new fint[s->get_M()];
  for(int i=0;i<s->get_M();i++)
    neighbors[i]=N;
}
PendantVertices::~PendantVertices(){
  delete[] neighbors;
}
fint* PendantVertices::get_neighbors(){
  return neighbors;
}
/**
 * Pendant vertices heuristic. Returns N+1 if everything is okay, 0 if no solution will be found and a vertex ( <= N) if it has to be forced.
 */
fint PendantVertices::apply(const fint& current){
  fint ret=N; // Default return value is N+1
  fint nb=0;
  for(fint i=0; i<N;i++){ // Processing the neighbors of current
    fint next=pows[i]^current; // next is a neighbor
    if(!seen[next]){
      neighbors[next]--; // Decrease the remaining degree of next
      if(neighbors[next]==1){ // Degree 1, there might be a problem.
	if(!is_neighbor(next)){ // If next is not a neighbor of 0 it has to be forced
	  if(ret==N){ // If no other vertex has to be forced, update the return value
	    ret=i;
	    nb++;
	  }else{// Else another vertex was forced and the return value will be 0
	    ret=-1;
	  }
	}else{
	  nb++;
	}
      }
    }
  }
  if((++ret)==0 || nb>=3){
    count++;
    return 0;
  }
  return ret;
}
void PendantVertices::undo(const fint& current){
  for(fint i=0; i<N;i++){
    fint next=pows[i]^current;
    if(!seen[next]){
      neighbors[next]++;
    }
  }
}

ConnectivityCheck::ConnectivityCheck(Search* s){
  this->name="Connectivity check";
  this->M=s->get_M();
  this->N=s->get_N();
  this->seen=s->get_seen();
  this->pows=s->get_pows();
  this->path_seen=new bool[M];
}
ConnectivityCheck::~ConnectivityCheck(){
  delete[] path_seen;
}

/**
 * Connectivity check on the remaining vertices (depth first search)
 */
fint ConnectivityCheck::apply(const fint& current){
  for(int i=0;i<M;i++)
    path_seen[i]=seen[i];
  path_seen[0]=false; // To ensure we reach 0 during the search
  stack<fint>to_visit;
  to_visit.push(current);
  while(!to_visit.empty()){
    fint curr=to_visit.top();
    to_visit.pop();
    for(fint i=0; i<N;i++){
      fint y=curr^pows[i];
      if(!path_seen[y]){
	path_seen[y]=1;
	to_visit.push(y);
      }
    }
  }
  for(int i=0;i<M;i++){
    if(!path_seen[i]){ // If some vertex has not been visited the graph is disconnected we return 0
      count++;
      return 0;
    }
  }
  return N+1;
}

OnesCheck::OnesCheck(Search *s){
  this->name="One-1";
  this->depth=0;
  this->nb_ones=s->get_N();
  this->N=s->get_N();
}
fint OnesCheck::apply(const fint& x){
  if(is_neighbor(x)){
    nb_ones--;
    if(nb_ones==0){
      count++;
      return 0;
    }
  }
  return N+1;
}
void OnesCheck::undo(const fint& x){
  if(is_neighbor(x)){
    nb_ones++;
  }
}

LowerDimCheck::LowerDimCheck(Search *s,const fint& dim){
  this->depth=0;
  this->M=s->get_pows()[dim];
  this->dim=dim;
  this->N=s->get_N();
  code=new fint[M];
  this->code[depth]=0;
  this->name="Dim "+to_string(dim)+" check("+to_string(M)+")";
}
fint LowerDimCheck::apply(const fint& x){
  if(depth>0 && depth<M){
    fint prev=code[depth-1];
    if(x<M){
      depth++;
      code[depth]=x;
      if( !is_neighbor(x^prev)){
	count++;
	return 0;
      }
    }
  }else if(depth==0){
    depth++;
  }

  
  return N+1;
}
void LowerDimCheck::undo(const fint& x){
  if(depth>0 && depth<=M && x<M)
    depth--;
  else if (depth==1)
    depth--;
}
SymmetryCheck::SymmetryCheck(Search *s){
  this->depth=1;
  this->M=s->get_M();
  this->name="Symmetry";
  this->seen=s->get_seen();
  this->N=s->get_N();
}
fint SymmetryCheck::apply(const fint& x){
  fint ret=N+1;
  if(!seen[M-1] && depth>(M/2)){
    ret=0;
    count++;
  }/*else if(x==2 && depth<M){
     ret=0;
     count++;
     }*/
  depth++;
  return ret;
}
void SymmetryCheck::undo(const fint& x){
  depth--;
}

PlaneRepresentation::PlaneRepresentation(Search* s){
  this->code=s->get_code();
  this->N=s->get_N();
  this->M=s->get_M();
  this->depth=0;
  this->pows=s->get_pows();
  this->p_code=new fint*[M];
  this->name="Plane connection";
  for(fint i=0;i<M;i++){
    this->p_code[i]=new fint[N];
    for(fint j=0;j<N;j++){
      this->p_code[i][j]=0;
    }
  }
}
fint PlaneRepresentation::apply(const fint& x){
  
  fint ret=N+1;
  if(depth>1){
    for(fint i=0;i<N;i++){
      if(x&pows[i]){
	p_code[depth][i]=1;
      }
    }
    for(fint i=0;i<N;i++){
      if(!p_code[depth-1][i] && p_code[depth][i]){
	if(  i>0 && p_code[depth][i-1]){
	  continue;
	}else if(i<N-1 && p_code[depth][i+1]){
	  continue;
	}else {
	  count++;
	  ret=0;
	  break;
	}
      }
    }
  }else if(depth>0){
    for(fint i=0;i<N;i++){
      if(x&pows[i]){
	p_code[depth][i]=1;
      }
    }
  }
  depth++;
  return ret;
}
void PlaneRepresentation::undo(const fint& x){
  depth--;
}

CompleteTransition::CompleteTransition(Search* s){
  this->code=s->get_code();
  this->N=s->get_N();
  this->M=s->get_M();
  this->depth=0;
  this->pows=s->get_pows();
  this->authorized=new fint*[N];
  this->transitions=new fint[M+1];
  this->name="Transition";
  transitions[0]=0;
  for(fint i=0;i<N;i++){
    this->authorized[i]=new fint[N];
    for(fint j=0;j<N;j++){
      this->authorized[i][j]=0;
    }
  }
  for(fint i=0;i<N;i++){
      fint j=(i+3)%(N);
    authorized[i][j]=1;
     j=(i+2)%(N);
    authorized[i][j]=1;
  }
  

 
}
fint CompleteTransition::apply(const fint& x){
  fint ret=N+1;
  if(depth>0){
    fint diff=x^code[depth-1];
    fint pos=0;
    while(diff){
      diff=diff>>1;
      pos++;
    }
    pos--;
    transitions[depth]=pos;
    if(depth>1 && !authorized[transitions[depth-1]][pos]){
      ret=0;
      count++;
    }
  }
  depth++;

  return ret;
}
void CompleteTransition::undo(const fint& x){
  depth--;
}

