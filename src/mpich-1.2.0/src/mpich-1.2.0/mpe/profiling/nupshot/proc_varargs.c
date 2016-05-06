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


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif


#include <stdio.h>
#include <stdlib.h>
#include "tcl.h"
#include "str_dup.h"

#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif



static int Call ANSI_ARGS(( ClientData, Tcl_Interp*, int, char ** ));
static int Command ANSI_ARGS(( ClientData, Tcl_Interp*, int, char ** ));


int Proc_VarArgsInit( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "proc_varargs", Command,
		     (ClientData)0, (Tcl_CmdDeleteProc*) 0 );
  return 0;
}



static int Command( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  Tcl_DString cmd;

  /*
     proc_varargs myproc {argc argv} {body}
  */

  if (argc != 4) {
    Tcl_AppendResult( interp, "wrong # args: should be \"", argv[0],
		      " name args body\"", (char *) NULL);
    return TCL_ERROR;
  }

  Tcl_DStringInit( &cmd );
  Tcl_DStringAppend( &cmd, "proc {proc_vararg(", -1 );
  Tcl_DStringAppend( &cmd, argv[1], -1 );
  Tcl_DStringAppend( &cmd, ")} ", -1 );
  Tcl_DStringAppendElement( &cmd, argv[2] );
  Tcl_DStringAppendElement( &cmd, argv[3] );

  Tcl_Eval( interp, Tcl_DStringValue( &cmd ) );

  Tcl_DStringFree( &cmd );

  Tcl_CreateCommand( interp, argv[1], Call, (ClientData)0,
		     (Tcl_CmdDeleteProc*)0 );

  return TCL_OK;
}



static int Call( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  char nargs[100];
  char *arg_list;

  arg_list = Tcl_Merge( argc, argv );
  sprintf( nargs, "%d", argc );

  return Tcl_VarEval( interp, "{proc_vararg(", argv[0], ")} ", nargs,
		      " {", arg_list, "}", (char*)0 );
}
