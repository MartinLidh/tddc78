// -*- c++ -*-
//
// MPI-1.2 C++ bindings
//
// Copyright (C) 1997  Jeremy G. Siek, Michael P. McNally
//                     Jeffrey M. Squyres, Andrew Lumsdaine
// 

#include <iostream.h>
#include <mpi++.h>

void example10_2(void);
void example10_3(void);
void example10_4(void);
void example10_5(void);

// Example 10.1 - Please note foocomm is not foo_comm as on Page 273
// so it will not conflict with later uses of foo_comm.

class foocomm : public MPI::Intracomm {
public:
  void Send(const void* buf, int count, const MPI::Datatype& type,
	    int dest, int tag) const
  {
    // Class library functionality
    MPI::Intracomm::Send(buf, count, type, dest, tag);
    // More class library functionality
  }
};

int
main(int argc, char *argv[])
{
  MPI::Init(argc, argv);

  example10_2();
  
  example10_3();
  
  example10_4();

  example10_5();

  MPI::Finalize();
}
  
// Example 10.2

void
example10_2()
{
  MPI::Intracomm bar;
  if (bar == MPI::COMM_NULL)
    cout << "bar is MPI::COMM_NULL" << endl;
}

// Example 10.3

void
example10_3()
{
  MPI::Intracomm foo_comm, bar_comm, baz_comm;
  foo_comm = MPI::COMM_WORLD;
  bar_comm = MPI::COMM_WORLD.Dup();
  baz_comm = bar_comm;
  cout << "Is foocomm equal barcomm? " << (foo_comm == bar_comm) << endl;
  cout << "Is bazcomm equal barcomm? " << (baz_comm == bar_comm) << endl;
}

// Example 10.4 Erroneous Code

void
example10_4()
{
  MPI::Intracomm intra = MPI::COMM_WORLD.Dup();
  MPI::Cartcomm cart(intra);
}

// Example 10.5

void
example10_5()
{
  MPI::Intercomm comm;
  comm = MPI::COMM_NULL;            // assign with COMM_NULL
  if (comm == MPI::COMM_NULL)       // true
    cout << "comm is NULL" << endl;
  if (MPI::COMM_NULL == comm)       // note -- a different function
    cout << "comm is still NULL" << endl;
}
