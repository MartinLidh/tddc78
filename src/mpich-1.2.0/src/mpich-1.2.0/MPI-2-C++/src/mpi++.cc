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


#include "mpi++.h"
#if HPUX_OS & LAM61
  // #%$^#$%^#$%^$ LAM on HP'S!!!!
#include <mpisys.h>
#undef MIN
#undef MAX
#endif

#if IBM_SP
  void throw_excptn_fctn(MPI_Comm* , int* errcode, char*, int*, int*)
#else
  void throw_excptn_fctn(MPI_Comm* , int* errcode, ...)
#endif
{
#if _MPIPP_USEEXCEPTIONS_
  throw(MPI::Exception(*errcode));
#else
  cerr << "exception throwing is disabled, MPI::errno has the error code" << endl;
  MPI::errno = *errcode;
#endif  
}

Map _REAL_MPI_::Comm::mpi_comm_map;

#if IBM_SP
void
errhandler_intercept(MPI_Comm * mpi_comm, int * err, char*, int*, int*)
#else
void
errhandler_intercept(MPI_Comm * mpi_comm, int* err, ...)
#endif

{
  _REAL_MPI_::Comm* comm =
    (_REAL_MPI_::Comm*)_REAL_MPI_::Comm::mpi_comm_map[(void*)*mpi_comm];
  if (comm && comm->my_errhandler) {
    va_list ap;
    va_start(ap, err);
    comm->my_errhandler->handler_fn(*comm, err, ap);
    va_end(ap);
  }
}

_REAL_MPI_::Op* _REAL_MPI_::Intracomm::current_op;

void
op_intercept(void *invec, void *outvec, int *len, MPI_Datatype *datatype)
{
  _REAL_MPI_::Op* op = _REAL_MPI_::Intracomm::current_op;
  MPI::Datatype thedata = *datatype;
  ((MPI::User_function*)op->op_user_function)(invec, outvec, *len, thedata);
  //JGS the above cast is a bit of a hack, I'll explain:
  //  the type for the PMPI::Op::op_user_function is PMPI::User_function
  //  but what it really stores is the user's MPI::User_function supplied when
  //  the user did an Op::Init. We need to cast the function pointer back to
  //  the MPI::User_function. The reason the PMPI::Op::op_user_function was
  //  not declared a MPI::User_function instead of a PMPI::User_function is
  //  that without namespaces we cannot do forward declarations.
  //  Anyway, without the cast the code breaks on HP LAM with the aCC compiler.
}

Map _REAL_MPI_::Comm::key_fn_map;

int
copy_attr_intercept(MPI_Comm oldcomm, int keyval, 
		    void *extra_state, void *attribute_val_in, 
		    void *attribute_val_out, int *flag)
{
  int ret = 0;
  Map::Pair* copy_and_delete = (Map::Pair*)_REAL_MPI_::Comm::key_fn_map[(Map::address)keyval];
  MPI::Comm::Copy_attr_function* copy_fn;
  copy_fn = (MPI::Comm::Copy_attr_function*)copy_and_delete->first_f;

  Map::Pair* comm_type = (Map::Pair*)_REAL_MPI_::Comm::mpi_comm_map[(Map::address)oldcomm];
  
  MPI::Intracomm intracomm;
  MPI::Intercomm intercomm;
  MPI::Graphcomm graphcomm;
  MPI::Cartcomm cartcomm;
  
  int thetype = (ATTR)comm_type->second;
  MPI2CPP_BOOL_T bflag = (MPI2CPP_BOOL_T)*flag; 

  switch (thetype) {
  case eIntracomm:
    intracomm = MPI::Intracomm(*(_REAL_MPI_::Intracomm*)comm_type->first);
    ret = copy_fn(intracomm, keyval, extra_state,
		  attribute_val_in, attribute_val_out, bflag);
  case eIntercomm:
    intercomm = MPI::Intercomm(*(_REAL_MPI_::Intercomm*)comm_type->first);
    ret = copy_fn(intercomm, keyval, extra_state,
		  attribute_val_in, attribute_val_out, bflag);
  case eGraphcomm:
    graphcomm = MPI::Graphcomm(*(_REAL_MPI_::Graphcomm*)comm_type->first);
    ret = copy_fn(graphcomm, keyval, extra_state,
		  attribute_val_in, attribute_val_out, bflag);
  case eCartcomm:
    cartcomm = MPI::Cartcomm(*(_REAL_MPI_::Cartcomm*)comm_type->first);
    ret = copy_fn(cartcomm, keyval, extra_state,
		  attribute_val_in, attribute_val_out, bflag);
  }

  *flag = (int)bflag;
  return ret;
}

// WDG - Changed to use pointers to functions in Map instead of pointers to 
// non-functions
int
delete_attr_intercept(MPI_Comm comm, int keyval, 
		      void *attribute_val, void *extra_state)
{
  int ret = 0;

  Map::Pair* copy_and_delete = (Map::Pair*)_REAL_MPI_::Comm::key_fn_map[(Map::address)keyval];

  MPI::Comm::Delete_attr_function* delete_fn;  
  delete_fn = (MPI::Comm::Delete_attr_function*)copy_and_delete->second_f;

  Map::Pair* comm_type = (Map::Pair*)_REAL_MPI_::Comm::mpi_comm_map[(Map::address)comm];

  MPI::Intracomm intracomm;
  MPI::Intercomm intercomm;
  MPI::Graphcomm graphcomm;
  MPI::Cartcomm cartcomm;
  
  int thetype = (long)(comm_type->second);

  switch (thetype) {
  case eIntracomm:
    intracomm = MPI::Intracomm(*(_REAL_MPI_::Intracomm*)comm_type->first);
    ret = delete_fn(intracomm, keyval, attribute_val, extra_state);
  case eIntercomm:
    intercomm = MPI::Intercomm(*(_REAL_MPI_::Intercomm*)comm_type->first);
    ret = delete_fn(intercomm, keyval, attribute_val, extra_state);
  case eGraphcomm:
    graphcomm = MPI::Graphcomm(*(_REAL_MPI_::Graphcomm*)comm_type->first);
    ret = delete_fn(graphcomm, keyval, attribute_val, extra_state);
  case eCartcomm:
    cartcomm = MPI::Cartcomm(*(_REAL_MPI_::Cartcomm*)comm_type->first);
    ret = delete_fn(cartcomm, keyval, attribute_val, extra_state);
  }
  return ret;
}

//$)(*!&@)$(*!&)@($*&!@(*&$(@*  HPUX !!!!!!!!!!!!!!!!!!!
#if _MPIPP_USENAMESPACE_
namespace MPI {
#define MPIPPNSPACE(X) X
#else
#define MPIPPNSPACE(X) MPI::##X
#endif

MPIPPNSPACE(Status) MPIPPNSPACE(Comm)::ignored_status;
MPIPPNSPACE(Status) MPIPPNSPACE(Request)::ignored_status;

#if ! _MPIPP_USEEXCEPTIONS_
int MPIPPNSPACE(errno) = MPI_SUCCESS;
#endif


const void* MPIPPNSPACE(BOTTOM) = (void*) MPI_BOTTOM;

// return  codes
const int MPIPPNSPACE(SUCCESS) = MPI_SUCCESS;
const int MPIPPNSPACE(ERR_BUFFER) = MPI_ERR_COUNT;
const int MPIPPNSPACE(ERR_TYPE) = MPI_ERR_TYPE;
const int MPIPPNSPACE(ERR_TAG) = MPI_ERR_TAG;
const int MPIPPNSPACE(ERR_COMM) = MPI_ERR_COMM;
const int MPIPPNSPACE(ERR_RANK) = MPI_ERR_RANK;
const int MPIPPNSPACE(ERR_REQUEST) = MPI_ERR_REQUEST;
const int MPIPPNSPACE(ERR_ROOT) = MPI_ERR_ROOT;
const int MPIPPNSPACE(ERR_GROUP) = MPI_ERR_GROUP;
const int MPIPPNSPACE(ERR_OP) = MPI_ERR_OP;
const int MPIPPNSPACE(ERR_TOPOLOGY) = MPI_ERR_TOPOLOGY;
const int MPIPPNSPACE(ERR_DIMS) = MPI_ERR_DIMS;
const int MPIPPNSPACE(ERR_ARG) = MPI_ERR_ARG;
const int MPIPPNSPACE(ERR_UNKNOWN) = MPI_ERR_UNKNOWN;
const int MPIPPNSPACE(ERR_TRUNCATE) = MPI_ERR_TRUNCATE;
const int MPIPPNSPACE(ERR_OTHER) = MPI_ERR_OTHER;
const int MPIPPNSPACE(ERR_INTERN) = MPI_ERR_INTERN;
#if HAVE_PENDING
const int MPIPPNSPACE(ERR_PENDING) = MPI_PENDING;
#else
const int MPIPPNSPACE(ERR_PENDING) = MPI_ERR_PENDING;
#endif
const int MPIPPNSPACE(ERR_IN_STATUS) = MPI_ERR_IN_STATUS;
const int MPIPPNSPACE(ERR_LASTCODE) = MPI_ERR_LASTCODE;

// assorted constants
const int MPIPPNSPACE(PROC_NULL) = MPI_PROC_NULL;
const int MPIPPNSPACE(ANY_SOURCE) = MPI_ANY_SOURCE;
const int MPIPPNSPACE(ANY_TAG) = MPI_ANY_TAG;
const int MPIPPNSPACE(UNDEFINED) = MPI_UNDEFINED;
const int MPIPPNSPACE(BSEND_OVERHEAD) = MPI_BSEND_OVERHEAD;
const int MPIPPNSPACE(KEYVAL_INVALID) = MPI_KEYVAL_INVALID;

// error-handling specifiers
const MPIPPNSPACE(Errhandler)  MPIPPNSPACE(ERRORS_ARE_FATAL)(MPI_ERRORS_ARE_FATAL);
const MPIPPNSPACE(Errhandler)  MPIPPNSPACE(ERRORS_RETURN)(MPI_ERRORS_RETURN);
const MPIPPNSPACE(Errhandler)  MPIPPNSPACE(ERRORS_THROW_EXCEPTIONS)(MPI_ERRORS_RETURN);
//JGS: the MPI_ERRORS_RETURN function in ERRORS_THROW_EXCEPTIONS gets replaced
//by the throw_exptn_fctn in MPIPPNSPACE(Init)

// maximum sizes for strings
const int MPIPPNSPACE(MAX_PROCESSOR_NAME) = MPI_MAX_PROCESSOR_NAME;
const int MPIPPNSPACE(MAX_ERROR_STRING) = MPI_MAX_ERROR_STRING;

// elementary datatypes
const MPIPPNSPACE(Datatype) MPIPPNSPACE(CHAR)(MPI_CHAR);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(SHORT)(MPI_SHORT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INT)(MPI_INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG)(MPI_LONG);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UNSIGNED_CHAR)(MPI_UNSIGNED_CHAR);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UNSIGNED_SHORT)(MPI_UNSIGNED_SHORT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UNSIGNED)(MPI_UNSIGNED);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UNSIGNED_LONG)(MPI_UNSIGNED_LONG);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(FLOAT)(MPI_FLOAT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(DOUBLE)(MPI_DOUBLE);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG_DOUBLE)(MPI_LONG_DOUBLE);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(BYTE)(MPI_BYTE);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(PACKED)(MPI_PACKED);

// datatypes for reductions functions (C / C++)
const MPIPPNSPACE(Datatype) MPIPPNSPACE(FLOAT_INT)(MPI_FLOAT_INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(DOUBLE_INT)(MPI_FLOAT_INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG_INT)(MPI_LONG_INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(TWOINT)(MPI_2INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(SHORT_INT)(MPI_SHORT_INT);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG_DOUBLE_INT)(MPI_LONG_DOUBLE);

#if FORTRAN
// elementary datatype (Fortran)
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INTEGER)(MPI_INTEGER);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(REAL)(MPI_REAL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(DOUBLE_PRECISION)(MPI_DOUBLE_PRECISION);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(F_COMPLEX)(MPI_COMPLEX);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LOGICAL)(MPI_LOGICAL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(CHARACTER)(MPI_CHARACTER);

// datatype for reduction functions (Fortran)
const MPIPPNSPACE(Datatype) MPIPPNSPACE(TWOREAL)(MPI_2REAL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(TWODOUBLE_PRECISION)(MPI_2DOUBLE_PRECISION);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(TWOINTEGER)(MPI_2INTEGER);

// optional datatypes (Fortran)
#if ALL_OPTIONAL_FORTRAN
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INTEGER1)(MPI_1INTEGER);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INTEGER2)(MPI_2INTEGER);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INTEGER4)(MPI_4INTEGER);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(REAL2)(MPI_2REAL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(REAL4)(MPI_4REAL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(REAL8)(MPI_8REAL);
#elif SOME_OPTIONAL_FORTRAN
const MPIPPNSPACE(Datatype) MPIPPNSPACE(INTEGER2)(MPI_2INTEGER);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(REAL2)(MPI_2REAL);
#endif //optional datatypes (Fortran)

#endif //FORTRAN

#if OPTIONAL_C
// optional datatype (C / C++)
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG_LONG)(MPI_LONG_LONG);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UNSIGNED_LONG_LONG)(MPI_UNSIGNED_LONG_LONG);
#endif //OPTIONAL_C

#if 0
// c++ type)s
const MPIPPNSPACE(Datatype) MPIPPNSPACE(BOOL);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(COMPLEX);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(DOUBLE_COMPLEX);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LONG_DOUBLE_COMPLEX);
#endif

// reserved communicators
MPIPPNSPACE(Intracomm) MPIPPNSPACE(COMM_WORLD)(MPI_COMM_WORLD);
MPIPPNSPACE(Intracomm) MPIPPNSPACE(COMM_SELF)(MPI_COMM_SELF);

// results of communicator and group comparisons
const int MPIPPNSPACE(IDENT) = MPI_IDENT;
const int MPIPPNSPACE(CONGRUENT) = MPI_CONGRUENT;
const int MPIPPNSPACE(SIMILAR) = MPI_SIMILAR;
const int MPIPPNSPACE(UNEQUAL) = MPI_UNEQUAL;

// environmental inquiry keys
const int MPIPPNSPACE(TAG_UB) = MPI_TAG_UB;
const int MPIPPNSPACE(IO) = MPI_IO;
const int MPIPPNSPACE(HOST) = MPI_HOST;
const int MPIPPNSPACE(WTIME_IS_GLOBAL) = MPI_WTIME_IS_GLOBAL;

// collective operations
const MPIPPNSPACE(Op) MPIPPNSPACE(MAX)(MPI_MAX);
const MPIPPNSPACE(Op) MPIPPNSPACE(MIN)(MPI_MIN);
const MPIPPNSPACE(Op) MPIPPNSPACE(SUM)(MPI_SUM);
const MPIPPNSPACE(Op) MPIPPNSPACE(PROD)(MPI_PROD);
const MPIPPNSPACE(Op) MPIPPNSPACE(MAXLOC)(MPI_MAXLOC);
const MPIPPNSPACE(Op) MPIPPNSPACE(MINLOC)(MPI_MINLOC);
const MPIPPNSPACE(Op) MPIPPNSPACE(BAND)(MPI_BAND);
const MPIPPNSPACE(Op) MPIPPNSPACE(BOR)(MPI_BOR);
const MPIPPNSPACE(Op) MPIPPNSPACE(BXOR)(MPI_BXOR);
const MPIPPNSPACE(Op) MPIPPNSPACE(LAND)(MPI_LAND);
const MPIPPNSPACE(Op) MPIPPNSPACE(LOR)(MPI_LOR);
const MPIPPNSPACE(Op) MPIPPNSPACE(LXOR)(MPI_LXOR);

// null handles
const MPIPPNSPACE(Group)        MPIPPNSPACE(GROUP_NULL) = MPI_GROUP_NULL;
//const MPIPPNSPACE(Comm)         MPIPPNSPACE(COMM_NULL) = MPI_COMM_NULL;
//const MPI_Comm          MPIPPNSPACE(COMM_NULL) = MPI_COMM_NULL;
MPIPPNSPACE(Comm_Null)    MPIPPNSPACE(COMM_NULL);
const MPIPPNSPACE(Datatype)     MPIPPNSPACE(DATATYPE_NULL) = MPI_DATATYPE_NULL;
MPIPPNSPACE(Request)      MPIPPNSPACE(REQUEST_NULL) = MPI_REQUEST_NULL;
const MPIPPNSPACE(Op)           MPIPPNSPACE(OP_NULL) = MPI_OP_NULL;
const MPIPPNSPACE(Errhandler)   MPIPPNSPACE(ERRHANDLER_NULL);  

// empty group
const MPIPPNSPACE(Group) MPIPPNSPACE(GROUP_EMPTY)(MPI_GROUP_EMPTY);

// topologies
const int MPIPPNSPACE(GRAPH) = MPI_GRAPH;
const int MPIPPNSPACE(CART) = MPI_CART;

// special datatypes for contstruction of derived datatypes
const MPIPPNSPACE(Datatype) MPIPPNSPACE(UB)(MPI_UB);
const MPIPPNSPACE(Datatype) MPIPPNSPACE(LB)(MPI_LB);


#if _MPIPP_USENAMESPACE_
}; /* namespace MPI */
#endif
