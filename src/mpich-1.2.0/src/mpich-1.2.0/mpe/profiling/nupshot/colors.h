/*
   Color stuff for Upshot

   Ed Karrels
   Argonne National Laboratory
*/
#ifndef _NUPSHOT_COLORS
#define _NUPSHOT_COLORS

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif


int Color_Reset ANSI_ARGS((void));
char *Color_Get ANSI_ARGS((void));

#endif
