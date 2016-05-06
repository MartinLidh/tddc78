/*
   Event-handling stuff for Upshot
*/

#include <stdio.h>
#include <string.h>
#include "events.h"
#include "str_dup.h"


static int SortEvents ANSI_ARGS(( eventData *event_data, int *list, int n ));


eventData *Event_Create()
{
  eventData *event_data;

  event_data = (eventData*)malloc( sizeof(eventData) );
  if (!event_data) {
    fprintf( stderr, "Out of memory in Event_Create()\n" );
    return 0;
  }

  ListCreate( event_data->defs.list, eventDefInfo, 10 );
  ListCreate( event_data->list, eventInfo, 10 );
  event_data->idx = 0;
  event_data->idx_proc = 0;

  return event_data;
}


int Event_DataInit( event_data, np )
eventData *event_data;
int np;
{
  event_data->np = np;

  return 0;
}


int Event_AddDef( event_data, name )
eventData *event_data;
char *name;
{
  eventDefInfo evtDef;

  evtDef.name = STRDUP( name );

  ListAddItem( event_data->defs.list, eventDefInfo, evtDef );

  return 0;
}


int Event_GetDef( event_data, n, name )
eventData *event_data;
int n;
char **name;
{
  *name = ListItem( event_data->defs.list, eventDefInfo, n ).name;

  return 0;
}

int Event_SetDef( event_data, n, name )
eventData *event_data;
int n;
char *name;
{
  char ** namePtr;

    /* quick-access temp var. */
  namePtr = &ListItem( event_data->defs.list, eventDefInfo, n ).name;

    /* if already set, deallocate memory */
  if (*namePtr) free( *namePtr );

    /* make a copy of the name */
  *namePtr =  STRDUP( name );

  return 0;

}

int Event_Ndefs( event_data )
eventData *event_data;
{
  return ListSize( event_data->defs.list, eventDefInfo );
}



int Event_Add( event_data, type, proc, time )
eventData *event_data;
int type, proc;
double time;
{
  eventInfo evt;

  evt.time = time;
  evt.type = type;
  evt.proc = proc;

  ListAddItem( event_data->list, eventInfo, evt );

  return 0;
}


int Event_N( event_data )
eventData *event_data;
{
  return ListSize( event_data->list, eventInfo );
}

int Event_Get( event_data, n, type, proc, time )
eventData *event_data;
int n, *type, *proc;
double *time;
{
  eventInfo *e;

  e = ListHeadPtr( event_data->list, eventInfo ) + n;
  *type = e->type;
  *proc = e->proc;
  *time = e->time;

  return 0;
}


int Event_DoneAdding( event_data )
eventData *event_data;
{
  int proc, i, nevents;
  eventInfo *evtPtr;

  /* shrink-wrap the list of events */
  ListShrinkToFit( event_data->list, eventInfo );

  nevents = ListSize( event_data->list, eventInfo );

  /* allocate list for the main index */
  ListCreate( event_data->idx, int, nevents );
  /* allocate lists for the per-process indices */
  event_data->idx_proc = (xpandList *)malloc( sizeof(xpandList) *
					      event_data->np );
  for (proc=0; proc<event_data->np; proc++) {
    ListCreate( event_data->idx_proc[proc], int, nevents/event_data->np );
  }

  /* spread the data to the appropriate indices */
  for (i=0,evtPtr = ListHeadPtr( event_data->list, eventInfo );
       i<nevents;
       i++,evtPtr++) {
    ListItem( event_data->idx, int, i ) = i;
    ListAddItem( event_data->idx_proc[evtPtr->proc], int, i );
  }

  /* sort the main index */
  SortEvents( event_data,
	      ListHeadPtr( event_data->idx, int ),
	      ListSize( event_data->idx, int ) );

  /* shrinkwrap and sort the per-process indices */
  for (proc=0; proc<event_data->np; proc++) {
    ListShrinkToFit( event_data->idx_proc[proc], int );
    SortEvents( event_data,
	        ListHeadPtr( event_data->idx_proc[proc], int ),
	        ListSize( event_data->idx_proc[proc], int ) );
  }


  return 0;
}


int Event_Close( data )
eventData *data;
{
  int proc, def;

  ListDestroy( data->list, eventInfo );

    /* if we were done adding, free the indices */
  if (data->idx) {
    ListDestroy( data->idx, int );
    for (proc=0; proc < data->np; proc++) {
      ListDestroy( data->idx_proc[proc], int );
    }
    free( data->idx_proc );
  }

    /* free event definitions */
  for (def = 0; def < ListSize( data->defs.list, eventDefInfo ); def++) {
    free( ListItem( data->defs.list, eventDefInfo, def ).name );
  }
  ListDestroy( data->defs.list, eventDefInfo );

  free( data );

  return 0;
}




/* I'm using my own insertion sort rather that qsort() because the
   list is probably already sorted.  */

/* I change my mind.  I'll use qsort(). */

#define USE_QSORT 1

#if USE_QSORT
static eventData *sort_data;
static int SetSortData ANSI_ARGS(( eventData * ));

static int SetSortData( event_data )
eventData *event_data;
{
  sort_data = event_data;
  return 0;
}

static int Compare( a, b )
#if !(defined(sparc) || defined(hpux)) || defined(__STDC__)
const
#endif
void *a, *b;
{
  return ListItem( sort_data->list, eventInfo, *(int*)a ).time >
         ListItem( sort_data->list, eventInfo, *(int*)b ).time;
}
#endif

static int SortEvents( event_data, list, n )
eventData *event_data;
int *list;
int n;
{
#if USE_QSORT
  SetSortData( event_data );
  qsort( list, n, sizeof(int), Compare );
  return 0;
#else

  register double time;
  int temp;
  int i, m, c;  /* insertPt, movePt, current */

  for (c=1; c<n-1; c++) {
    temp = list[c];
    time = ListItem( event_data->list, eventInfo, list[c]).time;
    for (i=c;
	 i && time < ListItem( event_data->list, eventInfo, list[i-1]).time;
	 i--);
    for (m=c-1; m>=i; m--)
      list[m+1] = list[m];
    list[i] = temp;
  }
  return 0;
#endif
  /* USE_QSORT */
}

