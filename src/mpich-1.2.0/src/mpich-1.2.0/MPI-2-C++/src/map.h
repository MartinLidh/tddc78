// -*- C++ -*-

// WDG - Added address of function, since pointers to functions are 
// different from pointers to non-functions in C++

#ifndef MAP_H_
#define MAP_H_

#include "list.h"
typedef List Container;

class Map {
  Container c;
public:

  typedef void* address;
  typedef void  (*address_f)(void);
  typedef List::iter iter;

  struct Pair {
    Pair(address f, address s) : first(f), second(s) {}
    Pair(address_f f, address_f s) : first_f(f), second_f(s) {}
    Pair() : first(0), second(0) { }
    address first;
    address second;
    address_f first_f;
    address_f second_f;
  };

  Map() { }

  ~Map() {
    for (iter i = c.end(); i != c.end(); i++) {
      delete (Pair*)(*i);
    }
  }
  
  Pair* begin();
  Pair* end();
  
  address& operator[](address key)
  {
    address* found = (address*)0;
    for (iter i = c.begin(); i != c.end(); i++) {
      if (((Pair*)*i)->first == key)
	found = &((Pair*)*i)->second;
    }
    if (! found) {
      iter tmp = c.insert(c.begin(), new Pair(key,0));
      found = &((Pair*)*tmp)->second;
    }
    return *found;
  }

  void erase(address key)
  {
    for (iter i = c.begin(); i != c.end(); i++) {
      if (((Pair*)*i)->first == key) {
	delete (Pair*)*i;
	c.erase(i); break;
      }
    }
  }

  address_f& operator[](address_f key)
  {
    address_f* found = (address_f*)0;
    for (iter i = c.begin(); i != c.end(); i++) {
      if (((Pair*)*i)->first_f == key)
	found = &((Pair*)*i)->second_f;
    }
    if (! found) {
      iter tmp = c.insert(c.begin(), new Pair(key,0));
      found = &((Pair*)*tmp)->second_f;
    }
    return *found;
  }

  void erase(address_f key)
  {
    for (iter i = c.begin(); i != c.end(); i++) {
      if (((Pair*)*i)->first_f == key) {
	delete (Pair*)*i;
	c.erase(i); break;
      }
    }
  }

};

#endif




