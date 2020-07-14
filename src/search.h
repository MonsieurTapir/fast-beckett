#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <deque>
#include <iostream>
#include <bitset>
#include <cmath>
#include "heuristics.h"
#include "common.h"


class Search{
 private:
  fint N;
  fint M;
  long long found=0;
  fint* sol;
  long long TAR;
  std::deque<fint> beckett_queue;
  fint* pows;
  bool* seen;
  fint depth=1;
  fint start=0;
  fint maxpos=0;
  std::vector<AbstractCut*> cut_heuristics;
  unsigned long long nodes=0;
  unsigned long long forced_vertices=0;
  unsigned long long cuts=0;
  bool branch(const fint&);
  bool go_deeper(const fint&);
  bool process(const fint&,const fint&,const fint&);
  AbstractSort* sort_strategy=0;
 public:
  ~Search();
  Search(fint,fint,long long,fint);
  int go();
  void display();
  fint get_N();
  fint get_M();
  bool* get_seen();
  fint* get_pows();
  void set_sort(AbstractSort*);
  void add_heuristic(AbstractCut*);
  void print_details();
  fint* get_code();
  unsigned long long get_nodes();
  unsigned long long get_cuts();
  unsigned long long get_forced();
};

#endif
