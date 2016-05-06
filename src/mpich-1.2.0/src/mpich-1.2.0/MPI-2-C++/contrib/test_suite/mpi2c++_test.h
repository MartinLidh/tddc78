// Copyright 1997-1999, University of Notre Dame.
// Authors:  Jeremy G. Siek, Michael P. McNally, Jeffery M. Squyres, 
//           Andrew Lumsdaine
//
// This file is part of the Notre Dame C++ bindings for MPI
//
// You should have received a copy of the License Agreement for the
// Notre Dame C++ bindings for MPI along with the software;  see the
// file LICENSE.  If not, contact Office of Research, University of Notre
// Dame, Notre Dame, IN  46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
// The vast majority of this awesome file came from Jeff Squyres,
// Perpetual Obsessive Notre Dame Student Craving Utter Madness, 
// and Brian McCandless, another of the LSC crew, under the guidance
// of Herr Doctor Boss Andrew Lumsdaine. My thanks for making my
// life a whole lot easier.

#ifndef _MPI2CPP_TEST_H_
#define _MPI2CPP_TEST_H_

// WDG - At least on Solaris, iostream MUST come before stdio (because
// stdio loads string, and iostream and string don't mix well
#include <iostream.h>
extern "C" {
#include <stdio.h>
#include <string.h>
}

#include <math.h>
#include <stdlib.h>
#include "mpi++.h"


// Version number

extern const int version[2];


// All the testing functions

void allgather(void);
void allreduce(void);
void alltoall(void);
void attr(void);
void badbuf(void);
void barrier(void);
void bcast(void);
void bcast_struct(void);
void bottom(void);
void bsend(void);
void buffer(void);
void cancel(void);
void cartcomm(void);
void commdup(void);
void commfree(void);
void compare(void);
void dims(void);
void dup_test(void);
void errhandler(void);
void gather(void);
void getcount(void);
void getel(void);
void graphcomm(void);
void group(void);
void groupfree(void);
void initialized1(void);
void initialized2(void);
void intercomm1(void);
void interf(void);
void iprobe(void);
void isend(void);
void lbub(void);
void lbub2(void);
void loop(void);
void op_test(void);
void pack_test(void);
void pcontrol(void);
void pptransp(void);
void probe(void);
void procname(void);
void range(void);
void rank_size(void);
void reduce(void);
void reduce_scatter(void);
void request1(void);
void rsend(void);
void rsend2(void);
void scan(void);
void scatter(void);
void send(void);
void sendrecv(void);
void sendrecv_rep(void);
void split(void);
void ssend(void);
void start(void);
void startall(void);
void status_test(void);
void strangest1(void);
void struct_gatherv(void);
void structsr(void);
void structsr2(void);
void test1(void);
void test3(void);
void testall(void);
void testany(void);
void testsome(void);
void topo(void);
void transp(void);
void transp2(void);
void transp3(void);
void transpa(void);
void waitall(void);
void waitany(void);
void waitsome(void);
void wildcard(void);
void wtime(void);

// Helper functions
// mpi2c++_test.cc

extern int my_rank;
extern int comm_size;
extern int to;
extern int from;
extern const double Epsilon;
extern MPI2CPP_BOOL_T CANCEL_WORKS;
extern MPI2CPP_BOOL_T TIGHTLY_COUPLED;

void do_work(int top = -1);


// messages.cc
// WDG - Make sure that "xxx" is a char * instead of a String
void Testing(char *msg);
void Pass(char *msg = (char *)"PASS");
void Sync(char *msg = 0);
void Postpone(char *class_name);
void Done(char *msg = 0);
void Fail(char *msg = (char *)"FAIL");
void Abort(char *msg = 0);


// stack.cc

void Push(char *msg);
char *Pop(void);


// signal.cc

void signal_init(void);


// General helper functions

inline void Test(int c, char *msg = 0) { (c) ? Pass() : Fail(msg); };
inline void Midtest(int c, char *msg = 0) { (c) ? Sync(msg) : Fail(msg); };
inline MPI2CPP_BOOL_T doublecmp(double a, double b) { return (MPI2CPP_BOOL_T) (fabs(a - b) < Epsilon); };


// Skip test flags

typedef enum _skip_flags {
  SKIP_MPICH1013, SKIP_MPICH110, SKIP_MPICH111, SKIP_MPICH112,
  SKIP_IBM21014, SKIP_IBM21015, SKIP_IBM21016, SKIP_IBM21017, SKIP_IBM21018,
  SKIP_LAM61, 
  SKIP_SGI20, SKIP_SGI30,
  SKIP_HPUX0102, SKIP_HPUX0103, SKIP_HPUX0105,
  SKIP_CRAY1104, SKIP_G_PLUS_PLUS, SKIP_NO_THROW,
  SKIP_MAX
} SKIP_FLAGS;
extern MPI2CPP_BOOL_T flags[SKIP_MAX];


#endif // _MPI2CPP_TEST_H_
