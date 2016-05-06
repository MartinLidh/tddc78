/*
   Visibility abstraction for Upshot

   Use these routines to set particular states or process or whatever
   visible or not.

   Ed Karrels
   Argonne National Laboratory
*/

#include <stdio.h>
#include "vis.h"
#include "expandingList.h"

#define DEBUG 0

Vis *Vis_new()
{
  Vis *tmp;

  tmp = (Vis*) malloc( sizeof(Vis) );
  if (!tmp) {
    fprintf( stderr, "Failed to allocate memory for visibility list.\n" );
    return 0;
  }

  ListCreate( tmp->list, int, 10 );

  return tmp;
}


int Vis_close( vis )
Vis *vis;
{
  ListDestroy( vis->list, int );
  free( vis );
  return 0;
}



int Vis_add( vis, x, before )
Vis *vis;
int x, before;
{
  int i, n, at;

#if DEBUG>1
  fprintf( stderr, "Adding %d to %p.\n", x, (void*)vis );
#endif

  at = -1;
  n = ListSize( vis->list, int );
  for (i=0; i<n; i++) {
    if (ListItem( vis->list, int, i ) == x) {
      at = i;
      break;
    }
  }

  if (at == -1) {
    /* adding--item is not in the list */
    if (before == -1) {
      /* appending */
      ListAddItem( vis->list, int, x );
    } else {
      /* copy last item to the new end */
      ListAddItem( vis->list, int,
		   ListItem( vis->list, int, n-1 ) );
      for (i=n-1; i>before; i--) {
	ListItem( vis->list, int, i ) =
	  ListItem( vis->list, int, i-1 );
      }
      ListItem( vis->list, int, i ) = x;
    }
  } else {
    /* moving */
    if (before == -1) {
        /* moving to final position */
        /* shift everyone up */
      for (i=at; i<n-1; i++) {
	ListItem( vis->list, int, i ) =
	  ListItem( vis->list, int, i+1 );
      }
        /* place at end */
      ListItem( vis->list, int, n-1 ) = x;
    } else {
      if (at < before-1) {
	  /* if it has to shifted down */
	for (i=at; i<before-1; i++) {
	  ListItem( vis->list, int, i ) =
	    ListItem( vis->list, int, i+1 );
	}
	  /* insert before 'before' */
	ListItem( vis->list, int, i ) = x;
      } else if (at > before) {
	  /* if it has to shifted up */
	for (i=at-1; i>=before; i++) {
	  ListItem( vis->list, int, i+1 ) =
	    ListItem( vis->list, int, i );
	}
	  /* insert x */
	ListItem( vis->list, int, before ) = x;
      }
    }
  }

  return 0;
}

int Vis_rm( vis, x )
Vis *vis;
int x;
{
  int i, n;

  n = ListSize( vis->list, int );
  for (i=0; i<n; i++) {
    if (ListItem( vis->list, int, i ) == x) {
      for (;i<n-1; i++) {
	ListItem( vis->list, int, i ) =
	  ListItem( vis->list, int, i+1 );
      }
      ListRemoveItems( vis->list, int, 1 );
      return 0;
    }
  }
  return -1;
}



int Vis_n( vis )
Vis *vis;
{
  return ListSize( vis->list, int );
}


int IsVis( vis, x )
Vis *vis;
int x;
{
  return Vis_x2i( vis, x )!=-1;
}


int Vis_x2i( vis, x )
Vis *vis;
int x;
{
  int i, n;

  n = ListSize( vis->list, int );

#if DEBUG>1
  fprintf( stderr, "checking if in list of %d elements\n", n );
#endif

  for (i=0; i<n; i++) {
#if DEBUG>1
    fprintf( stderr, "is %d %d?\n", ListItem( vis->list, int, i ), x );
#endif
    if (ListItem( vis->list, int, i ) == x) {
      return i;
    }
  }

  return -1;
}


int Vis_i2x( vis, idx )
Vis *vis;
int idx;
{
  if (idx >= Vis_n( vis ))
    return -1;
  
  return ListItem( vis->list, int, idx );
}

		
