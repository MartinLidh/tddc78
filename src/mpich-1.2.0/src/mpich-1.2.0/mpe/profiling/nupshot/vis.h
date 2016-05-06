/*
   Object visibility abstraction for Upshot

   Ed Karrels
   Argonne National Laboratory
*/

#ifndef _VIS_H_
#define _VIS_H_

#include "expandingList.h"

typedef struct Vis_ {
  xpandList /*int*/ list;
} Vis;


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

Vis *Vis_new ANSI_ARGS(( void ));
  /* create new visibility list */
int Vis_close ANSI_ARGS(( Vis * ));
  /* close visibility list */
#if 0
Vis *Vis_all();
  /* create visibility list so that everything is visible (fast) */
  /* note - this is a special case.  Nothing may be added, removed,
     or read */
#endif
int Vis_add ANSI_ARGS(( Vis *, int x, int before ));
  /* add x to visible list, before the given index (-1 for end) */ 
  /* may function as a move also--if the x is in the list
     it will be moved */
int Vis_rm ANSI_ARGS(( Vis *, int x ));
  /* remove x from visible list, returns -1 if not found */
int Vis_n ANSI_ARGS(( Vis * ));
  /* return the number of visible items */
int IsVis ANSI_ARGS(( Vis *, int x ));
  /* return nonzero if x is visible */
int Vis_x2i ANSI_ARGS(( Vis *, int x ));
  /* return the index of x in the visibility list */
int Vis_i2x ANSI_ARGS(( Vis *, int idx ));
  /* return the item at the specified index in the visibility list */

#endif
