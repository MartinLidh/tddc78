/*
   Standard, simple, error-checking Tcl argument conversion

   Ek Darrels
   Argonne National Laboratory
*/

#ifndef _CVT_ARGS_H_
#define _CVT_ARGS_H_

#include "tcl.h"


#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif


/* 
   given argc, argv, and a format string, ConvertArgs converts
   each of the given arguments to the specified formats.  The
   format string will be of the form:

   "<integer> <format char><format char><...>"

   The integer specifies how many arguments to skip, or which
   argv[x] to start on.

   The format characters are one of:

      d - decimal integer, matching argument is of type int *
      f - double, matching argument is of type double *
      s - string, matching argument is of type const char **,
          so the string may not be modified, but the pointer may

   If one of the arguments cannot be converted to the specified
   type, an error message is stored in interp->result and TCL_ERROR
   is returned.  If everything's OK, TCL_OK is returned.
*/

#ifdef __STDC__
int ConvertArgs( Tcl_Interp *interp, char *command, char *format,
		 int argc, char **argv, ... );
#else
int ConvertArgs();
#endif



#endif  /* _CVT_ARGS_H_ */
