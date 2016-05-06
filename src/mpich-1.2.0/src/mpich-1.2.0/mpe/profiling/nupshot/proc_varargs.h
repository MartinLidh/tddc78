/*
   Variable-length argument lists for Tcl

      proc_varargs my_proc {argc argv} {
         puts "$argc args"
         foreach arg $argv {
	    puts $arg
	 }
      }

  Ed Karrels
  Argonne National Laboratory

*/

#include "tcl.h"

#ifdef __STDC__
int Proc_VarArgsInit( Tcl_Interp *interp );
#else
int Proc_VarArgsInit();
#endif
