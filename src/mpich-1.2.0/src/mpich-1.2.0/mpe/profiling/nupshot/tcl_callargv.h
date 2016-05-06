/*
   Tcl_CallArgv - call tcl procedures manually with argc&argv

   Ed Karrels
   Argonne National Laboratory
*/


#ifndef TCL_ARGV_H
#define TCL_ARGV_H


#define Tcl_CallArgv( cmd_info, interp, argc, argv ) do {\
  int i_; \
  interp->result[0]=0; \
    /* for (i_=0; i_<(argc); i_++) { \
      fprintf( stderr, " %s", (argv)[i_] ); \
    } putc( '\n', stderr ); */ \
  if (TCL_OK != (*(cmd_info).proc)( (cmd_info).clientData, (interp), \
                               (argc), (argv) )) { \
    fprintf( stderr, "TCL error in %s, line %d:\n", __FILE__, \
             __LINE__ ); \
    for (i_=0; i_<(argc); i_++) { \
      fprintf( stderr, " %s", (argv)[i_] ); \
    } \
    fprintf( stderr, "\nerror message: %s\n", (interp)->result ); \
  } \
} while (0)



#endif

  /* TCL_ARGV_H */

