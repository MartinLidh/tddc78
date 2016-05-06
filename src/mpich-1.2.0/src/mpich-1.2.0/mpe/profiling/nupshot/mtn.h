/*
   Mountain-range view stuff for Upshot

   Ed Karrels
   Argonne National Laboratory
*/


#ifndef MTN_H
#define MTN_H

#include "heapsort.h"


typedef struct mtnEvt_ {
  double time;
  int isStartEvt;
  int stateNo;
} mtnEvt;


/* of the following structure:

   canvasName, bg, outline, bw, width, height, xsc_t, xsc_w, xsc_f, xsc_l,
   ysc_t, ysc_w, ysc_f, ysc_l, startTime, endTime

   are linked to Tcl array elements.
*/

typedef struct mtnInfo_ {
  Tcl_Interp *interp;		/* just in case */
  logFile *log;
  char *canvasName;		/* canvas that we'll draw on */
  char *bg;			/* background color */
  char *outline;		/* outline color */
  int bw;			/* whether to draw in b&w */

  char *array_name;		/* array name and index prefix used for */
  char *idx_prefix;		/* linking this to a Tcl array */

  double startTime, endTime;	/* copied from the logfile info */
  int np;
  double totalTime;		/* compute it once only */

  double width;			/* size of the canvas */
  double height;

  int visWidth;			/* size of the canvas window */
  int visHeight;
  int lastVisWidth;		/* previous settings */
  int lastVisHeight;

  double xleft, xspan;		/* horizontal scroll info */
  double lastXspan;

  int xview;			/* leftmost visible x-coordinate */

  double ytop, yspan;		/* vertical scroll info */
  double lastYspan;

  int yview;			/* leftmost visible y-coordinate */

  heapList /*mtnEvt*/ evtHeap;	/* initial heap */

  mtnEvt *evtList;		/* sorted list of start/stop events */
  int nevents;			/* # of events */
} mtnInfo;


#ifdef __STDC__
int Mtn_Init( Tcl_Interp *interp );
#else
int Mtn_Init();
#endif


#endif

/* MTN_H */

