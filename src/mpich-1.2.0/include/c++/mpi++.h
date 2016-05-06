// -*- c++ -*-
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

#ifndef MPIPP_H
#define MPIPP_H

#include "mpi2c++_config.h"

extern "C" {
#include <mpi.h>
}

#include <iostream.h>
#include <stdarg.h>
#include "map.h"



//JGS: this is used for implementing user functions for MPI::Op
extern "C" {
extern void
op_intercept(void *invec, void *outvec, int *len, MPI_Datatype *datatype);
}

#if IBM_SP
//Here's the sp2 typedeffrom their header file:
//  typedef void MPI_Handler_function(MPI_Comm *,int *,char *,int *,int *);
extern void
errhandler_intercept(MPI_Comm * mpi_comm, int * err, char*, int*, int*);

extern void
throw_excptn_fctn(MPI_Comm* comm, int* errcode, char*, int*, int*);


#else

//JGS: this is used as the MPI_Handler_function for
// the mpi_errhandler in ERRORS_THROW_EXCEPTIONS
extern "C" {
extern void
throw_excptn_fctn(MPI_Comm* comm, int* errcode, ...);

extern void
errhandler_intercept(MPI_Comm * mpi_comm, int * err, ...);
}
#endif


//used for attr intercept functions
enum CommType { eIntracomm, eIntercomm, eCartcomm, eGraphcomm};

extern "C" {
extern int
copy_attr_intercept(MPI_Comm oldcomm, int keyval, 
		    void *extra_state, void *attribute_val_in, 
		    void *attribute_val_out, int *flag);

extern int
delete_attr_intercept(MPI_Comm comm, int keyval, 
		      void *attribute_val, void *extra_state);
}

#if _MPIPP_PROFILING_
#include "pmpi++.h"
#endif

#if _MPIPP_USENAMESPACE_
namespace MPI {
#else
class MPI {
public:
#endif

#if ! _MPIPP_USEEXCEPTIONS_
  _MPIPP_EXTERN_ _MPIPP_STATIC_ int errno;
#endif

  class Comm_Null;
  class Comm;
  class Intracomm;
  class Intercomm;
  class Graphcomm;
  class Cartcomm;
  class Datatype;
  class Errhandler;
  class Group;
  class Op;
  class Request;
  class Status;

  typedef MPI_Aint Aint;

#include "constants.h"
#include "functions.h"
#include "datatype.h"

#if _MPIPP_USENAMESPACE_
  // We can't put this inside of a class.
  extern "C" {
  typedef void User_function(const void* invec, void* inoutvec, int len,
			     const Datatype& datatype);
  }
#else
  typedef void User_function(const void* invec, void* inoutvec, int len,
			     const Datatype& datatype);
#endif

#include "exception.h"
#include "op.h"
#include "status.h"
#include "request.h"   //includes class Prequest
#include "group.h" 
#include "comm.h"
#include "errhandler.h"
#include "intracomm.h"
#include "topology.h"  //includes Cartcomm and Graphcomm
#include "intercomm.h"
  
#if ! _MPIPP_USENAMESPACE_
private:
  MPI() { }
#endif
};

#if _MPIPP_PROFILING_
#include "pop_inln.h"
#include "pgroup_inln.h"
#include "pstatus_inln.h"
#include "prequest_inln.h"
#endif

//
// These are the "real" functions, whether prototyping is enabled
// or not. These functions are assigned to either the MPI::XXX class
// or the PMPI::XXX class based on the value of the macro _REAL_MPI_
// which is set in mpi2c++_config.h.
// If prototyping is enabled, there is a top layer that calls these
// PMPI functions, and this top layer is in the XXX.cc files.
//



#include "datatype_inln.h"
#include "functions_inln.h"
#include "request_inln.h"
#include "comm_inln.h"
#include "intracomm_inln.h"
#include "topology_inln.h"
#include "intercomm_inln.h"
#include "group_inln.h"
#include "op_inln.h"
#include "errhandler_inln.h"
#include "status_inln.h"

#endif


