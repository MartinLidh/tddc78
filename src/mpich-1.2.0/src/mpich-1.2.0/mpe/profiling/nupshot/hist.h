/*
   Histogram handling stuff for states in Upshot

   Ed Karrels
   Argonne National Laboratory
*/


#ifndef _HIST_H_
#define _HIST_H_

#include "states.h"
#include "stats.h"


typedef struct stateHist_ {
  Tcl_Interp *interp;
  stateData *state;
  int state_no;
  double *lens;
  double last_left, last_right;
  double shortLen, longLen;
  int firstIdx, lastIdx;
  int *last_bins, last_nbins;
  statData *vis_stats;
  char *array_name, *idx_prefix;

/* all the fields from here on down are mirrored in the Tcl array */
  char *window;
  char *canvas;
  char *time_lbl;
  char *xscrollbar;
  char *xscrollcommand;
  char *color;
  char *bitmap;
  char *outlineColor;
  int bw;			/* is black&white? */
  int nbins;
  double maxbin;
  double start, end;
  double left, right;
  int width, height;

    /* might want to write these guys in C */
  double scan_time, rclick_time, lclick_time, yclick;

    /* stats on all the data */
  int n;
  double sum, average, std_dev;
  double shortest, longest;

    /* stats on the visible range */
  int vis_n;
  double vis_sum, vis_average, vis_std_dev;
  double vis_shortest, vis_longest;
  
} stateHist;

#ifdef __STDC__
#define ARGS(x) x
#else
#define ARGS(x) ()
#endif

int Hist_Init ARGS(( Tcl_Interp *interp));

#endif
