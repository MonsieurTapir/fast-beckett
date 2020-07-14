#include <iostream>
#include <ctime>
#include <unistd.h>
#include "search.h"
#include "heuristics.h"
using namespace std;

bool is_neighbor(const fint& x){
  return (x && (!(x & (x-1))));
}

int main(int argc, char** argv){
  fint N=6;
  long tar=1;
  int opt;
  bool flag_conn= false;
  bool flag_sort=false;
  bool flag_dim=false;
  bool flag_iso=false;
  // Retrieve the (non-option) argument:
  if ( (argc <= 1) || (argv[1] == NULL) || (argv[1][0] == '-')) {  // there is NO input...
    cerr << "Missing non-optional argument : dimension" << endl;
    cerr << "Usage: "<< argv[0] << "<dimension>  [options]  "<< endl;
    cerr << "Options"<< endl
	 <<  "\t-c <long> : search <long> codes " << endl
	 << "\t-f : use connectivity cuts " << endl
	 << "\t-s : use degree sort" << endl;
    return 1;
  }
  N=atoi(argv[1]);
  // Shut GetOpt error messages down (return '?'): 
  opterr = 0;
  // Retrieve the options:
  while ( (opt = getopt(argc, argv, "c:fsdi")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'c':
      tar=atol(optarg);
      break;
    case 'f':
      flag_conn=true;
      cout << "> Using connectivity cuts" << endl;
      break;
    case 's':
      flag_sort=true;
      cout << "> Using degree sort" << endl;
      break;
    case 'd':
      flag_dim=true;
      cout << "> Enforcing Kn transition graph" << endl;
      break;
    case 'i':
      flag_iso=true;
      cout << "> Enumerating non isomorph codes " << endl;
      break;
    case '?':  // unknown option...
      cerr << "Unknown option: '" << char(optopt) << "'!" << endl;
      break;
    }
  }
  fint M=pow(2,N);
    Search* s=new Search(N,M,tar,1);
  
    PendantVertices* c=new PendantVertices(s);
    s->add_heuristic(c);
    OnesCheck* o=new OnesCheck(s);
    s->add_heuristic(o);

    if(flag_conn){
      ConnectivityCheck* c2=new ConnectivityCheck(s);
      s->add_heuristic(c2);
    }
    if(flag_sort){
      s->set_sort(new DegreeSort(s,c->get_neighbors(),s->get_N(),s->get_M()));
    }
    if(flag_dim){
      CompleteTransition *p=new CompleteTransition(s);
      s->add_heuristic(p);
    }
    if(flag_iso){
      SymmetryCheck* cs=new SymmetryCheck(s);
      s->add_heuristic(cs);
    }
    int found=s->go();
    s->print_details();
  
    delete s;
}
      
