#ifndef MPI_DEFS
#define MPI_DEFS
typedef int MPI_Fint;
typedef int MPI_Aint;

/* Eventually, these should be integrated into mpi.h by using an mpi.h.in 
   file */
/* If const is not supported, define HAVE_NO_C_CONST here */
#if 0 && !defined(HAVE_NO_C_CONST)
#define HAVE_NO_C_CONST 1
#endif

/* If stdargs supported, define USE_STDARG here */
#if 1 && !defined(USE_STDARG)
#define USE_STDARG 1
#endif

#define ROMIO 1
#if ROMIO == 1
#include "mpio.h"
#endif

#endif
