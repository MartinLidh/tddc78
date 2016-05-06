// -*- c++ -*-
//
//
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
//

#include <math.h>
#include "mpi++.h"

int
main(int argc, char *argv[])
{
  int n, rank, size, i;
  double PI25DT = 3.141592653589793238462643;
  double mypi, pi, h, sum, x;

  MPI::Init(argc, argv);
  size = MPI::COMM_WORLD.Get_size();
  rank = MPI::COMM_WORLD.Get_rank();

  while(1) {
    if (rank == 0) {
      cout << "Enter the number of intervals: (0 quits)" << endl;
      cin >> n;
    }

    // Broadcast the number of intervals
    MPI::COMM_WORLD.Bcast(&n, 1, MPI::INT, 0);

    if (n==0)
      break;
    else {
      h = 1.0 / (double) n;
      sum = 0.0;
      for (i = rank + 1; i <= n; i += size) {
	x = h * ((double)i - 0.5);
	sum += (4.0 / (1.0 + x*x));
      }
      mypi = h * sum;

      // Combine all the partial results
      MPI::COMM_WORLD.Reduce(&mypi, &pi, 1, MPI::DOUBLE, MPI::SUM, 0);

      if (rank == 0)
	cout << "pi is approximately " << pi << ", Error is " << 
	  fabs(pi - PI25DT) << endl;
    }
  }

  MPI::Finalize();

  return 0;
}
