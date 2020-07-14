#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <stack>
#include <vector>
#include <algorithm>
#include <string>
#include "common.h"

class Search;

class AbstractSort{
 public:
  virtual std::vector<fint>* sort_neighbors(const fint&){};
};

class DegreeSort : public AbstractSort{
 public:
  std::vector<fint>* sort_neighbors(const fint&);
  DegreeSort(Search*,fint *,const fint&,const fint&);
  ~DegreeSort();
 private:
  fint* degrees;
  fint N;
  fint M;
  fint* pows;
  bool authorized(const fint&, const fint&);
  std::vector<fint>** neighbors;
};
  
class DefaultSort : public AbstractSort{
 public:
  std::vector<fint>* sort_neighbors(const fint&);
  DefaultSort(Search*);
  ~DefaultSort();
  fint M;
 private:
  std::vector<fint>** neighbors;
};
  
class AbstractCut{
 public:
  virtual fint apply(const fint&){};
  virtual void undo(const fint&){};
  virtual unsigned long long get_count();
  void print_details();
 protected:
  unsigned long long count=0;
  std::string name;
};

class OnesCheck : public AbstractCut{
 private:
  fint depth;
  fint nb_ones;
  fint N;
 public:
  OnesCheck(Search*);
  fint apply(const fint&);
  void undo(const fint&);
  
  void print_details();
};

class PendantVertices : public AbstractCut{
 private:
  bool* seen;
  fint* neighbors;
  fint* pows;
  fint N;
 public:
  PendantVertices(Search*);
  ~PendantVertices();
  fint apply(const fint&);
  void undo(const fint&);
  fint* get_neighbors();  
};
class LowerDimCheck: public AbstractCut{
 private:
  fint depth;
  fint N;
  fint dim;
  fint M;
  fint* code;
 public:
  LowerDimCheck(Search*,const fint&);
  fint apply(const fint&);
  void undo(const fint&);
  
};
class SymmetryCheck: public AbstractCut{
 private:
  fint depth;
  fint M;
  bool* seen;
  fint N;
 public:
  SymmetryCheck(Search*);
  fint apply(const fint&);
  void undo(const fint&);
  
};
class ConnectivityCheck : public AbstractCut{
 private:
  bool* seen;
  bool* path_seen;
  fint* pows;
  fint M;
  fint N;
 public:
  ConnectivityCheck(Search*);
  ~ConnectivityCheck();
  fint apply(const fint&);
  void undo(const fint& x){};
};
class PlaneRepresentation: public AbstractCut{
 private:
  fint* code;
  fint** p_code;
  fint* pows;
  fint N;
  fint M;
  fint depth=0;
 public:
  PlaneRepresentation(Search*);
  fint apply(const fint&);
  void undo(const fint&);
  
};
class CompleteTransition: public AbstractCut{
 private:
  fint* code;
  fint* transitions;
  fint** authorized;
  fint* pows;
  fint N;
  fint M;
  fint depth=0;
 public:
  CompleteTransition(Search*);
  fint apply(const fint&);
  void undo(const fint&);

  
};
#endif
