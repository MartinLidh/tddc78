/*
   supply my own strdup just in case

   Ed Karrels
   Argonne National Laboratory
*/


/* don't call str_dup -- use STRDUP */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str_dup.h"
#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif


char *str_dup( str, line, file )
char *str;
int line;
char *file;
{
  char *p;

  p = (char *)malloc( strlen( str )+1 );
  if (!p) {
    fprintf( stderr,
	     "Failed to allocate %d bytes of memory in line %d, file %s\n",
	     (int)(strlen( str )+1), line, file );
    return 0;
  } else {
    strcpy( p, str );
    return p;
  }
}





