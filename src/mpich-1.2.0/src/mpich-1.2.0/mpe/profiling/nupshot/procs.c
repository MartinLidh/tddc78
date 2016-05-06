/*
   Process information for Upshot
   Thanks to dennis@rats-b.nosc.mil (Dennis Cottel) for this file
*/

#include <stdio.h>
#include <string.h>
#include "procs.h"
#include "str_dup.h"


processData *Process_Create()
{
  processData *process_data;

  process_data = (processData*)malloc( sizeof(processData) );
  if (!process_data) {
    fprintf( stderr, "Out of memory in Process_Create()\n" );
    return 0;
  }

  ListCreate( process_data->defs.list, processDefInfo, 10 );

  return process_data;
}



int Process_DataInit( process_data, np )
processData *process_data;
int np; /* number of processes in log file */
{
  int p;
  processDefInfo procDef;
  char tbuf[30];

    /* initialize process names to their numbers */
  for (p = 0; p < np; p++) {
    sprintf(tbuf, "%d", p);
    procDef.name = STRDUP( tbuf );
    ListAddItem( process_data->defs.list, processDefInfo, procDef );
  }

  return 0;
}



int Process_GetDef( process_data, n, name )
processData *process_data;
int n;
char **name;
{
  *name = ListItem( process_data->defs.list, processDefInfo, n ).name;

  return 0;
}



int Process_SetDef( process_data, n, name )
processData *process_data;
int n;
char *name;
{
  char ** namePtr;
  /* int namelength, oldnamelength; */

    /* quick-access temp var. */
  namePtr = &ListItem( process_data->defs.list, processDefInfo, n ).name;

    /* if already set, deallocate memory */
  if (*namePtr) free( *namePtr );

    /* make a copy of the name */
  *namePtr =  STRDUP( name );

  return 0;
}



int Process_Close( data )
processData *data;
{
  int def;

    /* free process definitions */
  for (def = 0; def < ListSize( data->defs.list, processDefInfo ); def++) {
    free( ListItem( data->defs.list, processDefInfo, def ).name );
  }
  ListDestroy( data->defs.list, processDefInfo );

  free( data );

  return 0;
}
