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


#ifndef PMPIPP_H
#define PMPIPP_H


#if _MPIPP_USENAMESPACE_
namespace PMPI {
#else
class PMPI {
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

#include "functions.h" //profile version same
#include "pdatatype.h"

  typedef void User_function(const void* invec, void* inoutvec, int len,
			     const Datatype& datatype);

#include "pexception.h"
#include "pop.h"
#include "pstatus.h"
#include "prequest.h"   //includes class Prequest
#include "pgroup.h" 
#include "pcomm.h"
#include "perrhandler.h"
#include "pintracomm.h"
#include "ptopology.h"  //includes Cartcomm and Graphcomm
#include "pintercomm.h"
  
#if ! _MPIPP_USENAMESPACE_
private:
  PMPI() { }
#endif
};


#endif
