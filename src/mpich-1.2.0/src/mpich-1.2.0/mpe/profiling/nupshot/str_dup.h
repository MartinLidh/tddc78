/*
   supply my own strdup just in case

   Ed Karrels
   Argonne National Laboratory
*/

#ifndef _STR_DUP_H_
#define _STR_DUP_H_

/* don't call str_dup -- use STRDUP */

#ifdef __STDC__
char *str_dup( char *, int line, char *file );
#else
char *str_dup();
#endif

#define STRDUP(str) str_dup(str, __LINE__, __FILE__)


#endif
