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
sendrecv()
{
  char msg[150];
  int dest;
  int i;
  int recvbuf[1000];
  int recvtag;
  int sendbuf[1000];
  int sendtag;
  int src;
  MPI::Status status;

  Testing( (char *)"Sendrecv w/ Status");

  if(my_rank < 2) {
    src = dest = 1 - my_rank;
    sendtag = my_rank;
    recvtag = src;

    for(i = 0; i < 100; i++) { 
      sendbuf[i] = my_rank; 
      recvbuf[i] = -1; 
    }

    MPI::COMM_WORLD.Sendrecv(sendbuf, 100, MPI::INT, dest, sendtag, recvbuf, 100,
			     MPI::INT, src, recvtag, status);

    for(i = 0; i < 2000000; i++);
    
    for(i = 0; i < 100; i++) 
      if(recvbuf[i] != src) {
	sprintf(msg, "NODE %d - 1) ERROR in MPI::Sendrecv, data = %d, should be %d", my_rank, recvbuf[i], src);
	Fail(msg);
      }

    if(status.Get_source() != src) {
      sprintf(msg, "NODE %d - 2) ERROR in MPI::Sendrecv, source = %d, should be %d", my_rank, status.Get_source(), src);
      Fail(msg);
    }

    if(status.Get_tag() != recvtag) {
      sprintf(msg, "NODE %d - 3) ERROR in MPI::Sendrecv, tag = %d, should be %d",
	      my_rank, status.Get_tag(), recvtag);
      Fail(msg);
    }
  }
  
  src = (my_rank == 0) ? comm_size - 1 : my_rank - 1;
  dest = (my_rank == comm_size - 1) ? 0 : my_rank + 1;
  sendtag = my_rank;
  recvtag = src;

  for(i = 0; i < 100; i++) { 
    sendbuf[i] = my_rank; 
    recvbuf[i] = -1; 
  }

  MPI::COMM_WORLD.Sendrecv(sendbuf, 100, MPI::INT, dest, sendtag, recvbuf, 100,
			   MPI::INT, src, recvtag, status);

  for(i = 0; i < 2000000; i++);

  for(i = 0; i < 100; i++) 
    if(recvbuf[i] != src)  {
      sprintf(msg, "NODE %d - 4) ERROR in MPI::Sendrecv, data = %d, should be %d", my_rank, recvbuf[i], src);
      Fail(msg);
    }
  
  if(status.Get_source() != src) {
    sprintf(msg, "NODE %d - 5) ERROR in MPI::Sendrecv, source = %d, should be %d", my_rank, status.Get_source(), src);
    Fail(msg);
  }
  
  if(status.Get_tag() != recvtag) {
    sprintf(msg, "NODE %d - 6) ERROR in MPI::Sendrecv, tag = %d, should be %d",
	    my_rank, status.Get_tag(), recvtag);
    Fail(msg);
  }
  
  Pass(); // Sendrecv w/ Status

  MPI::COMM_WORLD.Barrier();

  Testing( (char *)"Sendrecv w/o Status");

  if(my_rank < 2) {
    src = dest = 1 - my_rank;
    sendtag = my_rank;
    recvtag = src;
    
    for(i = 0; i < 100; i++) { 
      sendbuf[i] = my_rank; 
      recvbuf[i] = -1; 
    }
    
    MPI::COMM_WORLD.Sendrecv(sendbuf, 100, MPI::INT, dest, sendtag, recvbuf, 100,
			     MPI::INT, src, recvtag);
    
    for(i = 0; i < 2000000; i++);
    
    for(i = 0; i < 100; i++) 
      if(recvbuf[i] != src) {
	sprintf(msg, "NODE %d - 1) ERROR in MPI::Sendrecv, data = %d, should be %d", my_rank, recvbuf[i], src);
	Fail(msg);
      }
  }
  
  src = (my_rank == 0) ? comm_size - 1 : my_rank - 1;
  dest = (my_rank == comm_size - 1) ? 0 : my_rank + 1;
  sendtag = my_rank;
  recvtag = src;
  
  for(i = 0; i < 100; i++) { 
    sendbuf[i] = my_rank; 
    recvbuf[i] = -1; 
  }
  
  MPI::COMM_WORLD.Sendrecv(sendbuf, 100, MPI::INT, dest, sendtag, recvbuf, 100,
			   MPI::INT, src, recvtag);
  
  for(i = 0; i < 2000000; i++);
  
  for(i = 0; i < 100; i++) 
    if(recvbuf[i] != src)  {
      sprintf(msg, "NODE %d - 4) ERROR in MPI::Sendrecv, data = %d, should be %d",
	      my_rank, recvbuf[i], src);
      Fail(msg);
    }
  
  Pass(); // Sendrecv w/o Status
}
