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
// I made this.
#include "mpi2c++_test.h"

void
dims()
{
  char msg[150];
  int dims[10];
  int i;


  for(i = 0; i < 10; i++)
    dims[i] = 0;

  dims[0] = 2;

  Testing( (char *)"Compute_dims");

  MPI::Compute_dims(comm_size, 2, dims);
  
  if(dims[0] != 2 || dims[1] != comm_size / 2) {
    sprintf(msg, "NODE %d - 1) ERROR in Compute_dims, dims[0] = %d, dims[1] = %d, should be %d, %d", my_rank, dims[0], dims[1], 2, comm_size / 2);
    Fail(msg);
  }
  
  Pass(); // Compute_dims

  MPI::COMM_WORLD.Barrier();
}
