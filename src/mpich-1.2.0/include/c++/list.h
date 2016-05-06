// -*- C++ -*-


#ifndef LIST_H_
#define LIST_H_

#include "mpi2c++_config.h"

class List {
public:
  typedef void* Data;
  class iter;

  class Link {
    friend class List;
    friend class iter;
    Data data;
    Link *next;
    Link *prev;
    Link() { }
    Link(Data d, Link* p, Link* n) : data(d), prev(p), next(n) { }
  };

  class iter {
    friend class List;
    Link* node;
  public:
    iter(Link* n) : node(n) { }
    iter& operator++() { node = node->next; return *this; }
    iter operator++(int) { iter tmp = *this; ++(*this); return tmp; }
    Data& operator*() const { return node->data; }
    MPI2CPP_BOOL_T operator==(const iter& x) const { return (MPI2CPP_BOOL_T)(node == x.node); }
    MPI2CPP_BOOL_T operator!=(const iter& x) const { return (MPI2CPP_BOOL_T)(node != x.node); }
  };
  
  List() { _end.prev = &_end; _end.next = &_end; }
  ~List() {
    for (iter i = begin(); i != end(); ) {
      Link* garbage = i.node; i++;
      delete garbage;
    }
  }
  virtual iter begin() { return _end.next; }
  virtual iter end() { return &_end; }
  virtual iter insert(iter p, Data d) {
    iter pos(p);
    Link* n = new Link(d, pos.node->prev, pos.node);
    pos.node->prev->next = n;
    pos.node->prev = n;
    return n;
  }
  void erase(iter pos) {
    pos.node->prev->next = pos.node->next;
    pos.node->next->prev = pos.node->prev;
    delete pos.node;
  }

protected:
  Link _end;
};

#endif




