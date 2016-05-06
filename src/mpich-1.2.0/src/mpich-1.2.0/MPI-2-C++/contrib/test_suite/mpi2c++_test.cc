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

#include <iostream.h>
#include "mpi++.h"
#include "mpi2c++_test.h"
#if SGI30
extern "C" {
#include <string.h>
}
#endif
#if CRAY
#include <mpp/rastream.h>
#endif

//
// Global variables
//

int comm_size = -1;
int my_rank = -1;
int to = -1;
int from = -1;
MPI2CPP_BOOL_T CANCEL_WORKS = false;
MPI2CPP_BOOL_T TIGHTLY_COUPLED = false;
const int version[2] = {1, 0};
const double Epsilon = 0.001;
MPI2CPP_BOOL_T flags[SKIP_MAX];


//
// Local functions
//

static void check_args(int argc, char *argv[]);
static int my_strcasecmp(char *a, char *b);
static void check_minimals(void);


//
// main
//
int
main(int argc, char *argv[])
{
#if CRAY
  int oldstr = get_d_stream();
  set_d_stream(0);
#endif

  // Start up MPI

  check_args(argc, argv);

  initialized1();

  MPI::Init(argc, argv);

  // Define some globals

  comm_size = MPI::COMM_WORLD.Get_size();
  my_rank = MPI::COMM_WORLD.Get_rank();
  to = (my_rank + 1) % comm_size;
  from = (my_rank + comm_size - 1) % comm_size;

  // Announce

  if (my_rank == 0)
    cout << endl
	 << "Since we made it this far, we will assume that" << endl
	 << "MPI::Init() worked properly." << endl 
	 << "----------------------------------------------" 
	 << endl 
	 << "MPI-2 C++ bindings test suite" << endl 
	 << "------------------------------" << endl 
	 << "LSC Version " 
	 << version[0] << "." << version[1] << endl 
	 << endl 
	 << "*** There are delays built into some of the tests" << endl 
	 << "*** Please let them complete" << endl 
	 << "*** No test should take more than 10 seconds" << endl << endl;

  // Catch all fatal signals

  signal_init();

  // Check for minimal testing conditions in MPI environment

  check_minimals();

  // Ensure that all the ranks have the relevant command line args
  // That is, pass on any of the _flag arguments

#if BOOL_NE_INT
  MPI::COMM_WORLD.Bcast(flags, SKIP_MAX * sizeof(MPI2CPP_BOOL_T), MPI::CHAR, 0);
#else
  MPI::COMM_WORLD.Bcast(flags, SKIP_MAX, MPI::INT, 0);
#endif
  // Test all the objects
  // WDG - Make "xxx" a char * instead of a String
  Testing((char *)"MPI namespace");
  initialized2();
  procname();
  Pass(); // MPI namespace

  Testing((char *)"MPI::Comm");
  rank_size();
  Pass(); // MPI::Comm

  Testing((char *)"MPI::Status");
  status_test();
  Pass(); // MPI::Status

  Testing((char *)"MPI::Comm");
  send();

  errhandler();

  Pass(); // MPI::Comm

  Testing((char *)"MPI::Request");
  request1();
  Pass(); // MPI::Request

  Testing((char *)"MPI::Status");
  getcount();
  getel();
  Pass(); // MPI::Status

  Testing((char *)"MPI namespace");
  buffer();
  dims();
  pcontrol();
  wtime();
  Pass(); // MPI namespace

  Testing((char *)"MPI::Comm");
  topo();
  bsend();
  rsend();
  ssend();
  isend();
  sendrecv();
  sendrecv_rep();
  iprobe();
  probe();
  Pass(); // MPI::Comm

  Testing((char *)"MPI::Request");
  waitany();
  testany();
  waitall();
  testall();
  waitsome();
  testsome();
  cancel();
  Pass(); // MPI::Request

  Testing((char *)"MPI::Comm");
  start();
  startall();
  Pass(); // MPI::Comm

  Testing((char *)"MPI::Intracomm");
  dup_test();
  bcast();
  gather();
  struct_gatherv();
  scatter();
  allgather();
  alltoall();
  reduce();
  allreduce();
  reduce_scatter();
  scan();
  split();
  Pass(); // MPI::Intracomm

  Testing((char *)"MPI::Cartcomm");
  cartcomm(); 
  Pass(); // MPI::Cartcomm

  Testing((char *)"MPI::Graphcomm");
  graphcomm();
  Pass(); // MPI::Graphcomm

  Testing((char *)"MPI::Datatype");
  bcast_struct();
  pack_test();
  Pass(); // MPI::Datatype

  Testing((char *)"MPI::Intracomm");
  compare();
  Pass(); // MPI::Intracomm

  Testing((char *)"MPI::");
  intercomm1();
  Pass(); // MPI::

  Testing((char *)"MPI::Comm");
  attr();
  Pass(); // MPI::Comm

  // MD 92 bytes leak
  Testing((char *)"MPI::Group");
  group();
  groupfree();
  Pass(); // MPI::Group

  Testing((char *)"MPI::Op");
  op_test();
  Pass(); // MPI::Op 

  // All done.  Call MPI_Finalize()

  if(my_rank == 0)
    cout << endl << "* MPI::Finalize..." << endl;

  MPI::COMM_WORLD.Barrier();

  MPI::Finalize();

  if(my_rank == 0)
    cout << endl << endl
	 << "Since we made it this far, we will assume that" << endl
	 << "MPI::Finalize() did what we wanted it to." << endl
	 << "(Or, at the very least, it didn't fail.)" << endl
	 << "-----------------------------------------------------------" << endl
	 << "MPI-2 C++ bindings test suite: All done.  All tests passed." << endl
	 << endl;

#if CRAY
  set_d_stream(oldstr);
#endif

  return 0;
}


//
// Check the args for command line flags
//
void
check_args(int argc, char *argv[])
{
  int i;

  for (i = 0; i <= SKIP_MAX; i++)
    flags[i] = false;

  for (i = 1; i < argc; i++) {
    if (my_strcasecmp(argv[i], (char *)"-lam61") == 0)
      flags[SKIP_LAM61] = true;
    else if (my_strcasecmp(argv[i], (char *)"-mpich1013") == 0)
      flags[SKIP_MPICH1013] = true;
    else if (my_strcasecmp(argv[i], (char *)"-mpich110") == 0)
      flags[SKIP_MPICH110] = true;
    else if (my_strcasecmp(argv[i], (char *)"-mpich111") == 0)
      flags[SKIP_MPICH111] = true;
    else if (my_strcasecmp(argv[i], (char *)"-mpich112") == 0)
      flags[SKIP_MPICH112] = true;
    else if (my_strcasecmp(argv[i], (char *)"-ibm21014") == 0)
      flags[SKIP_IBM21014] = true;
    else if (my_strcasecmp(argv[i], (char *)"-ibm21015") == 0)
      flags[SKIP_IBM21015] = true;
    else if (my_strcasecmp(argv[i], (char *)"-ibm21016") == 0)
      flags[SKIP_IBM21016] = true;
    else if (my_strcasecmp(argv[i], (char *)"-ibm21017") == 0)
      flags[SKIP_IBM21017] = true;
    else if (my_strcasecmp(argv[i], (char *)"-ibm21018") == 0)
      flags[SKIP_IBM21018] = true;
    else if (my_strcasecmp(argv[i], (char *)"-sgi20") == 0)
      flags[SKIP_SGI20] = true;
    else if (my_strcasecmp(argv[i], (char *)"-sgi30") == 0)
      flags[SKIP_SGI30] = true;
    else if (my_strcasecmp(argv[i], (char *)"-hpux0102") == 0)
      flags[SKIP_HPUX0102] = true;
    else if (my_strcasecmp(argv[i], (char *)"-hpux0103") == 0)
      flags[SKIP_HPUX0103] = true;
    else if (my_strcasecmp(argv[i], (char *)"-hpux0105") == 0)
      flags[SKIP_HPUX0105] = true;
    else if (my_strcasecmp(argv[i], (char *)"-cray1104") == 0)
      flags[SKIP_CRAY1104] = true;
    else if (my_strcasecmp(argv[i], (char *)"-g++") == 0)
      flags[SKIP_G_PLUS_PLUS] = true;
    else if (my_strcasecmp(argv[i], (char *)"-nothrow") == 0)
      flags[SKIP_NO_THROW] = true;
    else if (my_strcasecmp(argv[i], (char *)"-help") == 0 ||
	     my_strcasecmp(argv[i], (char *)"-h") == 0) {
      cout << "The following command line options are available:" << endl 
	   << " -help        This message" << endl 
	   << " -lam61       Skip tests for LAM (only on IRIX)" << endl 
	   << " -mpich1013   Skip tests for buggy MPICH 1.0.13" << endl
	   << " -mpich110    Skip tests for buggy MPICH 1.1.0" << endl
	   << " -mpich111    Skip tests for buggy MPICH 1.1.1" << endl
	   << " -mpich112    Skip tests for buggy MPICH 1.1.2" << endl
	   << " -ibm21014    Skip tests for buggy IBM SP MPI 2.1.0.14" << endl
	   << " -ibm21015    Skip tests for buggy IBM SP MPI 2.1.0.15" << endl
	   << " -ibm21016    Skip tests for buggy IBM SP MPI 2.1.0.16" << endl
	   << " -ibm21017    Skip tests for buggy IBM SP MPI 2.1.0.17" << endl
	   << " -ibm21018    Skip tests for buggy IBM SP MPI 2.1.0.18" << endl
	   << " -sgi20       Skip tests for buggy SGI MPI 2.0" << endl
	   << " -sgi30       Skip tests for buggy SGI MPI 3.0" << endl
	   << " -hpux0102    Skip tests for buggy HPUX 01.02" << endl
	   << " -hpux0103    Skip tests for buggy HPUX 01.03" << endl
	   << " -hpux0105    Skip tests for buggy HPUX 01.05" << endl
	   << " -cray1104    Skip tests for buggy CRAY 1.1.0.4" << endl
	   << " -g++         Skip tests for buggy G++ (exceptions)" << endl;

      exit(0);
    }
  }
}


int
my_strcasecmp(char *a, char *b)
{
  while ((a != 0) && (b != 0) && (*a != '\0') && (*b != '\0') && (*a == *b))
    a++, b++;
  if (*a == *b)
    return 0;
  return 1;
}


//
// Do meaningless work to burn up time
//
void
do_work(int top)
{
  double start1 = MPI::Wtime() + .25;
  top++;

  while(MPI::Wtime() < start1)
    continue;
}


//
// Check for minimal MPI environment
//
static void
check_minimals()
{
  MPI2CPP_BOOL_T need_flag = false;
  char *msg = (char *)"";

  if (my_rank == 0)
    cout << "Test suite running on " << comm_size << " nodes" << endl;

  if (comm_size < 2) {
    if (my_rank == 0) {
      cout << "Sorry, the MPI2C++ test suite must be run with at least 2 processors" << endl;
      cout << "Please re-run the program with 2 or more processors." << 
	endl << endl;
    }
    MPI::Finalize();
    exit(1);
  }
  
  if ((comm_size % 2) != 0) {
    if (my_rank == 0)
      cout << "The MPI2C++ test suite can only run on an even number" << endl 
	   << "of processors.  Please re-run the program with an even" << endl 
	   << "number of ranks." << endl << endl;

    MPI::Finalize();
    exit(1);
  }

  // Check to see if we *should* be using one of the above flags

#if LAM61
  if (!flags[SKIP_LAM61]) {
    need_flag = true;
    msg = (char *)"-lam61";
  }
#elif LAMIRIX
  if (!flags[SKIP_LAM61]) {
    need_flag = true;
    msg = (char *)"-lam61";
  }
#elif MPICH1013
  if (!flags[SKIP_MPICH1013]) {
    need_flag = true;
    msg = (char *)"-mpich1013";
  }
#elif MPICH110
  if (!flags[SKIP_MPICH110]) {
    need_flag = true;
    msg = (char *)"-mpich110";
  }
#elif MPICH111
  if (!flags[SKIP_MPICH111]) {
    need_flag = true;
    msg = (char *)"-mpich111";
  }
#elif MPICH112
  if (!flags[SKIP_MPICH112]) {
    need_flag = true;
    msg = (char *)"-mpich112";
  }
#elif IBM21014
  if (!flags[SKIP_IBM21014]) {
    need_flag = true;
    msg = (char *)"-ibm21014";
  }
#elif IBM21015
  if (!flags[SKIP_IBM21015]) {
    need_flag = true;
    msg = (char *)"-ibm21015";
  }
#elif IBM21016
  if (!flags[SKIP_IBM21016]) {
    need_flag = true;
    msg = (char *)"-ibm21016";
  }
#elif IBM21017
  if (!flags[SKIP_IBM21017]) {
    need_flag = true;
    msg = (char *)"-ibm21017";
  }
#elif IBM21018
  if (!flags[SKIP_IBM21018]) {
    need_flag = true;
    msg = (char *)"-ibm21018";
  }
#elif SGI20
  if (!flags[SKIP_SGI20]) {
    need_flag = true;
    msg = (char *)"-sgi20";
  }
#elif SGI30
  if (!flags[SKIP_SGI30]) {
    need_flag = true;
    msg = (char *)"-sgi30";
  }
#elif HPUX0102
  if (!flags[SKIP_HPUX0102]) {
    need_flag = true;
    msg = (char *)"-hpux0102";
  }
#elif HPUX0103
  if (!flags[SKIP_HPUX0103]) {
    need_flag = true;
    msg = (char *)"-hpux0103";
  }
#elif CRAY1104
  if (!flags[SKIP_CRAY1104]) {
    need_flag = true;
    msg = (char *)"-cray1104";
  }
#endif

  if (need_flag && my_rank == 0) {
    cout << "**** WARNING!! ****" << endl << endl
	 << "You really should use the \"" << msg << "\" flag when running the " 
	 << endl 
	 << "test suite on this architecture/OS.  If you do not use this flag," 
	 << endl
	 << "certain tests will probably fail, and the test suite will abort." 
	 << endl << endl
	 << "The test suite will now commence without this flag so that you " 
	 << endl
	 << "can see which tests will fail on this architecture/OS." 
	 << endl << endl;
  }

  need_flag = false;
#if G_PLUS_PLUS
  if (!flags[SKIP_G_PLUS_PLUS]) {
    need_flag = true;
    msg = "-g++";
  }
#endif

  if (need_flag && my_rank == 0) {
    cout << "**** WARNING!! ****" << endl << endl
	 << "You really should use the \"" << msg << "\" flag when running the " 
	 << endl 
	 << "test suite on this architecture/OS.  If you do not use this flag," 
	 << endl
	 << "certain tests will probably fail, and the test suite will abort." 
	 << endl << endl
	 << "The test suite will now commence without this flag so that you " 
	 << endl
	 << "can see which tests will fail on this architecture/OS." 
	 << endl << endl;
  }
}
