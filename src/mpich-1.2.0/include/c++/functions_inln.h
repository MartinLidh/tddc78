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


//
// Point-to-Point Communication
//

inline void 
_REAL_MPI_::Attach_buffer(void* buffer, int size)
{
  (void)MPI_Buffer_attach(buffer, size);
}

inline int 
_REAL_MPI_::Detach_buffer(void*& buffer)
{
  int size;
  (void)MPI_Buffer_detach(&buffer, &size);
  return size;
}

//
// Process Topologies
//

inline void
_REAL_MPI_::Compute_dims(int nnodes, int ndims, int dims[])
{
  (void)MPI_Dims_create(nnodes, ndims, dims);
}


//
// Environmental Inquiry
//

inline void 
_REAL_MPI_::Get_processor_name(char*& name, int& resultlen)
{
  (void)MPI_Get_processor_name(name, &resultlen);
}

inline void
_REAL_MPI_::Get_error_string(int errorcode, char* string, int& resultlen)
{
  (void)MPI_Error_string(errorcode, string, &resultlen);
}

inline int 
_REAL_MPI_::Get_error_class(int errorcode) 
{
  int errorclass;
  (void)MPI_Error_class(errorcode, &errorclass);
  return errorclass;
}

inline double 
_REAL_MPI_::Wtime()
{
  return (MPI_Wtime());
}

inline double 
_REAL_MPI_::Wtick()
{
  return (MPI_Wtick());
}

inline void
_REAL_MPI_::Real_init()
{
  MPI::ERRORS_THROW_EXCEPTIONS.init();
}

/*
inline void
_REAL_MPI_::Init(int& argc, char**& argv)
{
  (void)MPI_Init(&argc, &argv);
  Real_init();
}
*/

// The new Solaris CC refuses to accept this, complaining that PMPI::Init
// already has a body - WDG
/*
inline void
_REAL_MPI_::Init()
{
  (void)MPI_Init(0, 0);
  Real_init();
}
*/
inline void
_REAL_MPI_::Finalize()
{
  (void)MPI_Finalize();
}

inline MPI2CPP_BOOL_T
_REAL_MPI_::Is_initialized()
{
  int t;
  (void)MPI_Initialized(&t);
  return (MPI2CPP_BOOL_T) t;
}

//
// Profiling
//

inline void
_REAL_MPI_::Pcontrol(const int level, ...)
{
  va_list ap;
  va_start(ap, level);
 
  (void)MPI_Pcontrol(level, ap);
  va_end(ap);
}

//JGS, MPI_Address soon to be replaced by MPI_Get_address
inline _REAL_MPI_::Aint
_REAL_MPI_::Get_address(void* location)
{
  _REAL_MPI_::Aint ret;
  MPI_Address(location, &ret);
  return ret;
}


