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

#include "mpi2c++_test.h"

void
errhandler()
{
  int i;
  MPI2CPP_BOOL_T skip = false;
  MPI2CPP_BOOL_T pass = false;

  MPI::Intracomm a;

  a = MPI::COMM_NULL;

  Testing( (char *)"ERRORS_THROW_EXCEPTIONS");

  if (flags[SKIP_CRAY1104])
    Done( (char *)"Skipped (CRAY 1.1.0.4)");
  else if (flags[SKIP_SGI20])
    Done( (char *)"Skipped (SGI 2.0)");
  else if (flags[SKIP_SGI30])
    Done( (char *)"Skipped (SGI 3.0)");
  else if (flags[SKIP_G_PLUS_PLUS])
    Done( (char *)"Skipped (G++ is broken.)");
  else if (flags[SKIP_NO_THROW])
    Done( (char *)"Skipped (Exception throwing broken)");    
  else
    {
      MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);
      
      try {
#if SGI20
	a.Send(&i, -1, MPI::DATATYPE_NULL, -my_rank, 201);
#elif SGI30
	if (getenv("MPI_CHECK_ARGS") == 0) {
	  if (my_rank == 0) {
	    cout << endl << endl 
		 << "The MPI-2 C++ test suite depends on the MPI_CHECK_ARGS"
		 << endl
		 << "environment variable being set to \"1\" *before* mpirun"
		 << endl
		 << "is invoked for successful testing. The test suite will"
		 << endl
		 << "now exit since MPI_CHECK_ARGS is not currently set. Set"
		 << endl
		 << "the MPI_CHECK_ARGS variable and re-run the MPI-2 C++"
		 << endl
		 << "test suite." << endl << endl;
	  }
	  skip = true;
	}
	else
	  MPI::COMM_WORLD.Send(&i, 1, MPI::DATATYPE_NULL, my_rank, 201);
#else
	a.Send(&i, 1, MPI::DATATYPE_NULL, my_rank, 201);
#endif
      }
      catch(MPI::Exception e) {
	pass = true;
      }
      if (skip)
	Fail( (char *)"MPI_CHECK_ARGS not set");
      else if (!pass)
	Fail( (char *)"Exception not caught");
      else
	Pass();
      
      MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_RETURN);
    }

  if(a != MPI::COMM_NULL && a != MPI::COMM_WORLD)
    a.Free();
}

