/*
   Timeline widget for Upshot

   Ed Karrels
   Argonne National Laboratory

   This widget should handle all the operation of a timeline canvas,
with everything controlled not in pixels or boxes or arrows, but
in seconds, states, and messages.

Operations that will be allowed:

  Create and connect to a logfile

    timeline <window name> <logfile command name> <total width> \
             <total height> <req width> <req height> [options...]
      width - total width, not just the visible width, in one of the
          forms acceptable to Tk_GetPixels
      height - total height, not just the visible height, in one of the
          forms acceptable to Tk_GetPixels

      options:
        -from <time>
        -to <time>
            Specify the earliest point in the logfile that will
            be displayed.  If the logfile is huge, the user may only
            wish to load part of it.  Say, in a 1 hour logfile, the
            user may only want to load from the 15 minute mark to the
            20 minute mark.  It will be possible to reconfigure the
            canvas to dispay a different region.

    Every timeline widget must be connected to a logfile widget that
    has been opened and loaded.  The timeline will get
    all of its settings such as the number of processes and the
    display option for the states from the logfile to which it is
    connected.


  Scrolling
    Like a canvas:
      <timeline> scroll scan mark <x> <y>
      <timeline> scroll scan dragto <x> <y>
      <timeline> scroll xview <x>
      <timeline> scroll yview <y>
      <timeline> config -xscrollcommand <command>
      <timeline> config -yscrollcommand <command>

    The -xscrollcommand option will be bound to the logfile that
    this timeline to which this logfile is connected.  The scan
    mark, scan dragto, and xview commands (changing x-coordinate only)
    will be called by the same logfile.

    In the future, the y-scrolling commands may be controlled, too.

  Zooming
    zoom by a given factor ( f>1 for zoom in, 0<f<1 for zoom out) at
    a given point

      <timeline> zoom_time <time> <factor>


  Eliminating or adding certain processes, events, states, or message
    There will be an option to display or not certain processes,
    events, states, or messages.

*/


#ifndef _TIMELINES_H_
#define _TIMELINES_H_

#include "tcl.h"
#include "tk.h"
#include "log.h"
#include "vis.h"
#include "expandingList.h"
#include "feather.h"


#ifdef __STDC__
int Timeline_Init( Tcl_Interp *interp );
#else
int Timeline_Init();
#endif

  /* bit mask of what needs to be done */
  /* in the next TimeLineWhenIdle() */
#define TL_NEEDS_RESIZE (1<<0)

#define UPDATE_NEW_VIS (1<<0)


typedef struct tl_stateInfo_ {
  int canvasId1, canvasId2;
} tl_stateInfo;
  /* if canvasId1 >= 0, it is the index of the item id of the rectangle
     on the canvas.  Canvasid1 is the index
     of this guys matching coverup rectangle, and canvasId2 is the id
     of the filled rectangle.
  */

typedef struct timeLineOverlap_ {
  xpandList /*int*/ halfWidths;
} timeLineOverlap;


typedef struct timeLineConvert_ {
  double offset;
  double skew;
} timeLineConvert;

typedef struct timeLineInfo_ {
  Tcl_Interp *interp;		/* Tcl interpreter */

  char *windowName;		/* the window for this widget, and its */
				/* widget command.  This is actually an */
				/* overloaded frame widget */

  char *array_name;		/* array name and prefix used for linking */
  char *idx_prefix;		/* this to a Tcl array */

  char *canvasName;		/* the display canvas's name */

  logFile *log;			/* the log its attached to */

  double width;			/* scrollregion */
  double height;

  int visWidth;			/* size of the canvas window */
  int visHeight;
  int lastVisWidth;		/* previous settings */
  int lastVisHeight;

  double xleft, xspan;		/* horizontal scroll info */
  double lastXspan;
  int xview;			/* leftmost visible coordinate */

  double ytop, yspan;		/* vertical scroll info */
  double lastYspan;
  int yview;			/* topmost visible coordinate */

  double startTime, endTime;	/* cached from the logfile's data */
  int np;
  double totalTime;		/* computed once and saved */

  Vis *procVis;			/* visible processes */
  Vis *eventVis;		/* visible events */
  Vis *stateVis;		/* visible states */
  Vis *msgVis;			/* visible messages */

  timeLineOverlap overlap;	/* overlap info */

  char *lineColor;		/* color of background lines */
  char *outlineColor;		/* color of outlines for state bars */
  char *msgColor;		/* color of message arrows */
  char *bg;			/* canvas background color */

  int bw;			/* in black&white */

  int isdrawn;			/* whether the timelines have even been */
				/* drawn yet */

  xpandList /*tl_stateInfo*/ stateList;
    /* list of all states, noting if each was drawn, and if so, what
       its index is.  This list is parallel to the stateData->list list
       of states, so the same index will be valid for both */

  int *msgIds;			/* canvas id's of every message drawn */

    /* a pointer to the canvas data structure */
  void *canvasPtr;
    /* the window for the canvas */
  Tk_Window canvasWin;
    /* allocate colors, bitmaps, and UID names for each state type */
  Feather_Color *fillColors;
  Feather_Bitmap *fillStipples;
  Feather_Color outlineCol;
  Feather_Color bgCol;
  Feather_Color msgCol;
  Tk_Uid *stateTags;		/* a set of tags of the form "state_%d" */
  Tk_Uid *procTags;		/* a set of tags of the form "proc_%d" */
  Tk_Uid *colorTags;		/* a set of tags of the form "color_%d" */
  Tk_Uid colorBgTag;		/* Tk_GetUid( "color_bg" ) */
  Tk_Uid colorOutlineTag;	/* Tk_GetUid( "color_outline" ) */
  Tk_Uid colorArrowTag;		/* Tk_GetUid( "color_arrow" ) */

} timeLineInfo;


#endif  /* _TIMELINES_H_ */
