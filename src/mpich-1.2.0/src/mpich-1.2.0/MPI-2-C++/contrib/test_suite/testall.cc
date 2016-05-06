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
/****************************************************************************

 MESSAGE PASSING INTERFACE TEST CASE SUITE

 Copyright IBM Corp. 1995

 IBM Corp. hereby grants a non-exclusive license to use, copy, modify, and
 distribute this software for any purpose and without fee provided that the
 above copyright notice and the following paragraphs appear in all copies.

 IBM Corp. makes no representation that the test cases comprising this
 suite are correct or are an accurate representation of any standard.

 In no event shall IBM be liable to any party for direct, indirect, special
 incidental, or consequential damage arising out of the use of this software
 even if IBM Corp. has been advised of the possibility of such damage.

 IBM CORP. SPECIFICALLY DISCLAIMS ANY WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS AND IBM
 CORP. HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 ENHANCEMENTS, OR MODIFICATIONS.

****************************************************************************

 These test cases reflect an interpretation of the MPI Standard.  They are
 are, in most cases, unit tests of specific MPI behaviors.  If a user of any
 test case from this set believes that the MPI Standard requires behavior
 different than that implied by the test case we would appreciate feedback.

 Comments may be sent to:
    Richard Treumann
    treumann@kgn.ibm.com

****************************************************************************
*/
#include "mpi2c++_test.h"

void
testall()
{
  char msg[150];
  int data[2000];
  int flag;
  int i;
  MPI::Request req[2000];
  MPI::Status status[2000];

  Testing( (char *)"Testall w/ Status");

  if (flags[SKIP_SGI20])
    Done( (char *)"Skipped (SGI 2.0)");
  else {
    for(i = 0; i < 2000; i++) {
      data[i] = -1;
      req[i] = MPI::REQUEST_NULL;
    }
    
    if(my_rank > 0) 
      MPI::COMM_WORLD.Send(&my_rank, 1, MPI::INT, 0, 1);
    else {
      req[0] = MPI::REQUEST_NULL;
      for(i = 1; i < comm_size; i++)  
	req[i] = MPI::COMM_WORLD.Irecv(&data[i], 1, MPI::INT, i, 1);
      
      flag = MPI::Request::Testall(0, req, status);  
      if(flag == 0) {
	sprintf(msg, "NODE %d - 1) ERROR in MPI_Testall: flag is not set",
		my_rank);
	Fail(msg);
      }
      
      flag = 0; 
      while(flag == 0) {
	for(i = 1; i < comm_size; i++) {
	  if(req[i] == MPI::REQUEST_NULL) {
	    sprintf(msg, "NODE %d - 2) ERROR in MPI_Testall: incorrect status",
		    my_rank);
	    Fail(msg);
	  }
	}
	flag = MPI::Request::Testall(comm_size, req, status);  
      }
      for(i = 1; i < comm_size; i++)  {
	if(req[i] != MPI::REQUEST_NULL) {
	  sprintf(msg, "NODE %d - 3) ERROR in Testall: request not set to NULL",
		  my_rank);
	  Fail(msg);
	}
	if(status[i].Get_source() != i) {
	  sprintf(msg, "NODE %d - 4) ERROR in Testall: request prematurely set to NULL", my_rank);
	  Fail(msg);
	}
	if(data[i] != i) {
	  sprintf(msg, "NODE %d - 5) ERROR in MPI_Testall: incorrect data",
		  my_rank);
	  Fail(msg);
	}
      }
      
      flag = MPI::Request::Testall(comm_size, req, status);  
      if(flag == 0) {
	sprintf(msg, "NODE %d - 6) ERROR in MPI_Testall: flag is not set",
		my_rank);
	Fail(msg);
      }
    }
    Pass(); // Testall w/ Status
  }

  for(i = 0; i < 2000; i++) {
    data[i] = -1;
    if(req[i] != MPI::REQUEST_NULL)
      req[i].Free();
  }

  Testing( (char *)"Testall w/o Status");

  if (flags[SKIP_SGI20])
    Done( (char *)"Skipped (SGI 2.0)");
  else {
    if(my_rank > 0) 
      MPI::COMM_WORLD.Send(&my_rank, 1, MPI::INT, 0, 1);
    else {
      req[0] = MPI::REQUEST_NULL;
      for(i = 1; i < comm_size; i++)  
	req[i] = MPI::COMM_WORLD.Irecv(&data[i], 1, MPI::INT, i, 1);
      
      flag = MPI::Request::Testall(0, req);  
      if(flag == 0) {
	sprintf(msg, "NODE %d - 7) ERROR in MPI_Testall: flag is not set",
		my_rank);
	Fail(msg);
      }
      
      flag = 0; 
      while(flag == 0) {
	for(i = 1; i < comm_size; i++) {
	  if(req[i] == MPI::REQUEST_NULL) {
	    sprintf(msg, "NODE %d - 8) ERROR in MPI_Testall: incorrect status",
		    my_rank);
	    Fail(msg);
	  }
	}
	flag = MPI::Request::Testall(comm_size, req);  
      }
      for(i = 1; i < comm_size; i++) {
	if(req[i] != MPI::REQUEST_NULL) {
	  sprintf(msg, "NODE %d - 9) ERROR in Testall: request not set to NULL",
		  my_rank);
	  Fail(msg);
	}
	if(data[i] != i) {
	  sprintf(msg, "NODE %d - 10) ERROR in MPI_Testall: incorrect data",
		  my_rank);
	  Fail(msg);
	}
      }
      
      flag = MPI::Request::Testall(comm_size,req);  
      if(flag == 0) {
	sprintf(msg, "NODE %d - 11) ERROR in MPI_Testany: flag is not set",
		my_rank);
	Fail(msg);
      }
    }
    Pass(); // Testall w/o Status
  }

  for(i = 0; i < 2000; i++)
    if(req[i] != MPI::REQUEST_NULL)
      req[i].Free();
}
