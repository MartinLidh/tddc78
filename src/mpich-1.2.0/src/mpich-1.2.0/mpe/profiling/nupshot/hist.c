/*
   Histogram handling stuff for states in Upshot

   Ed Karrels
   Argonne National Laboratory
*/


#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#include <stdio.h>
#include <string.h>
#include "tcl.h"
#include "log.h"
#include "cvt_args.h"
#include "str_dup.h"
#include "tclptr.h"
#include "states.h"
#include "stats.h"
#include "tcl_callargv.h"
#include "hist.h"


#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif

static int HistCmd ANSI_ARGS(( ClientData data, Tcl_Interp *interp,
			  int argc, char *argv[] ));

  /* Sort the state lengths, return pointer to hist info */
static stateHist *State_HistInit ANSI_ARGS(( stateData *state, int def_no,
				       int *procs, int nprocs ));

  /* fill the given array with the number of states in each bin */
  /* return the number of states included in all the bins */
static int State_HistBins ANSI_ARGS(( stateHist *hist, double shortLen,
				 double longLen, int *bins, int nbins ));

static int PointlistCmd ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist,
			       int argc, char *argv[] ));

static int Pointlist ANSI_ARGS(( stateHist *hist, int *point_list ));

static int Draw ANSI_ARGS(( Tcl_Interp *intepr, stateHist *hist,
		       int argc, char *argv[] ));

static int ScrollCmd ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist,
			    int argc, char *argv[] ));

static int XviewCmd ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist,
			   int argc, char *argv[] ));

static int BinsCmd ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist,
			       int argc, char *argv[] ));

static int LinkVars ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist ));

static int UnlinkVars ANSI_ARGS(( Tcl_Interp *interp, stateHist *hist ));

  /* close this hist instance */
static int State_HistClose ANSI_ARGS(( stateHist *hist ));

static stateHist *HistToken2Ptr ANSI_ARGS(( Tcl_Interp *interp, char *token ));
static int CompareLen ANSI_ARGS(( const void *va, const void *vb ));
static int Bsearch   ANSI_ARGS(( stateHist *hist, double x, int top, int bottom ));
/*
static int BsearchLE ANSI_ARGS(( stateHist *hist, double x, int top, int bottom ));
*/
static int BsearchGE ANSI_ARGS(( stateHist *hist, double x, int top, int bottom ));

int Hist_Init( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "hist", HistCmd, (ClientData) 0,
		     (Tcl_CmdDeleteProc*) 0 );
  return 0;
}



static int HistCmd( data, interp, argc, argv )
ClientData data;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  char *logCmd=0, *array_name=0, *idx_prefix=0;
  int state_no;
  stateHist *hist;
  logFile *log;

  if (argc == 1) {
    Tcl_AppendResult( interp, "missing command for 'hist'", (char*)0 );
    return TCL_ERROR;
  }

  if (!strcmp( argv[1], "open" )) {
    if (TCL_OK != ConvertArgs( interp,
              "hist open <log> <state_no> <array_name> <idx_prefix>",
			       "2 sdss", argc, argv, &logCmd, &state_no,
			       &array_name, &idx_prefix )) {
      return TCL_ERROR;
    }

      /* get pointer to log data */
    log = LogCmd2Ptr( interp, logCmd );
    if (!log) return TCL_ERROR;

      /* open the histogram data */
    hist = State_HistInit( log->states, state_no, (int*)0, -1 );
    hist->interp = interp;

      /* Tcl array and index prefix to link variables to */
    hist->array_name = STRDUP( array_name );
    hist->idx_prefix = STRDUP( idx_prefix );

      /* link certain members of my hist structure to variables
	 int the given Tcl array */
    LinkVars( interp, hist );

      /* return a pointer to my data structure */
    sprintf( interp->result, "hist%d", AllocTclPtr( (void*)hist ) );
    return TCL_OK;

  } else {

    if (!(hist = HistToken2Ptr( interp, argv[1] )))
      return TCL_ERROR;

    if (!strcmp( argv[2], "bins" )) {
      return BinsCmd( interp, hist, argc, argv );

    } else if (!strcmp( argv[2], "pointlist" )) {
      return PointlistCmd( interp, hist, argc, argv );

    } else if (!strcmp( argv[2], "draw" )) {
      return Draw( interp, hist, argc, argv );

    } else if (!strcmp( argv[2], "setscroll" )) {
      return ScrollCmd( interp, hist, argc, argv );


    } else if (!strcmp( argv[2], "xview" )) {
      return XviewCmd( interp, hist, argc, argv );

    } else if (!strcmp( argv[2], "close" )) {
      State_HistClose( hist );
      return TCL_OK;
    } else {
      Tcl_AppendResult( interp, argv[2],
        " -- unrecognized histogram command.  Either 'bins', or 'close'.",
		        (char *)0 );
      return TCL_ERROR;
    }
  }

}


static stateHist *State_HistInit( state, def_no, procs, nprocs )
stateData *state;
int def_no;
int *procs;
int nprocs;
{
  stateHist *hist = 0;
  stateInfo *state_info = 0;
  int *idx_ptr;
  int n, i, nstates_this_proc, proc_no, proc_no_idx, state_no;
  statData *stats;

  n = ListSize( state->list, stateInfo );
    /* initially allocate space for all the states; worst-case
       scenario */
  if ( !(hist = (stateHist*)malloc( sizeof( stateHist ) )) ||
       !(hist->lens = (double*)malloc( sizeof(double) * n )) ) {
    fprintf( stderr, "Out of memory in State_HistInit\n" );
    return 0;
  }

  if (!(stats = Stats_Create())) return 0;

  hist->state = state;
  hist->state_no = def_no;
  hist->sum = 0;
  hist->n = 0;
  hist->last_bins = 0;
  hist->window = hist->canvas = hist->xscrollcommand = 0;
  hist->time_lbl = hist->xscrollbar = 0;
  hist->color = hist->bitmap = hist->outlineColor = 0;
  hist->scan_time = hist->rclick_time = hist->lclick_time = hist->yclick = 0;
  hist->sum = hist->average = hist->std_dev = 0;
  hist->shortest = hist->longest = 0;
  hist->vis_sum = hist->vis_average = hist->vis_std_dev = 0;
  hist->vis_shortest = hist->vis_longest = 0;

  hist->shortLen = hist->longLen = -1;
  hist->firstIdx = hist->lastIdx = -1;

  if (nprocs != -1) {
      /* add states from only a few process */

      /* for each process given, */
    for (proc_no_idx=0; proc_no_idx<nprocs; proc_no_idx++) {
        /* get the process number */
      proc_no = procs[proc_no_idx];

        /* traverse the list of states on this process */
      idx_ptr = ListHeadPtr( state->idx_proc_start[proc_no], int );
      nstates_this_proc = ListSize( state->idx_proc_start[proc_no], int );
      for (state_no=0; state_no < nstates_this_proc; state_no++) {

	  /* finally, a pointer to the state we want */
	state_info = ListHeadPtr( state->list, stateInfo ) +
	  idx_ptr[state_no];

	  /* if the state matches the type we're looking for */
	if (state_info->type == def_no) {
	  
	    /* add this length to the list */
	  Stats_Add( stats, hist->lens[hist->n++] = state_info->endTime - 
		     state_info->startTime );
	}
      }
    }

  } else {

      /* add all states */

    state_info = ListHeadPtr( state->list, stateInfo );
    for (i=0; i<n; i++,state_info++) {

        /* if the state matches the type we're looking for */
      if (state_info->type == def_no) {
	  
	  /* add this length to the list */
	Stats_Add( stats, hist->lens[hist->n++] = state_info->endTime - 
		   state_info->startTime );

      }

    }
  }

  hist->sum      = Stats_Sum   ( stats );
  hist->average  = Stats_Av    ( stats );
  hist->std_dev  = Stats_StdDev( stats );
  hist->shortest = Stats_Min( stats );
  hist->longest  = Stats_Max( stats );

  Stats_Close( stats );

    /* reallocate to fit only the number of slots used */
  hist->lens = (double*) realloc( (void*)hist->lens,
				  sizeof(double) * hist->n );

  qsort( hist->lens, hist->n, sizeof(double), CompareLen );

    /* used each time the data is split into bins */
  hist->vis_stats = Stats_Create();

  /*
  {
    int i;
    for (i=0; i<hist->n; i++) {
      fprintf( stderr, "%d. %f\n", i, hist->lens[i] );
    }
  }
  */


  return hist;
}



#define LINK_ELEMENT( str_name, name, type ) \
  sprintf( tmp, "%s(%s,%s)", hist->array_name, hist->idx_prefix, str_name ); \
  Tcl_LinkVar( interp, tmp, (char*)&hist->name, type );
/*
  fprintf( stderr, "Linking hist->%s at address %p.\n", \
	   str_name, (char*)&hist->name );
*/



static int LinkVars( interp, hist )
Tcl_Interp *interp;
stateHist *hist;
{
  char *tmp;
  int i = TCL_LINK_INT;
  int f = TCL_LINK_DOUBLE;
  int s = TCL_LINK_STRING;

  /* tmp will get filled with something like "timeline(.win.0,stuff)",
     or ("%s(%s,%s)", array_name, idx_prefix, element_name ), to
     be exact.
  */
  int max_element_len = 14;  /* "xscrollcommand" */

  tmp = malloc( strlen(hist->array_name) + 1 + strlen(hist->idx_prefix) + 1 +
		       max_element_len + 1 + 1 );

  LINK_ELEMENT( "window", window, s );
  LINK_ELEMENT( "canvas", canvas, s );
  LINK_ELEMENT( "time_lbl", time_lbl, s );
  LINK_ELEMENT( "xscrollbar", xscrollbar, s );
  LINK_ELEMENT( "xscrollcommand", xscrollcommand, s );
  LINK_ELEMENT( "color", color, s );
  LINK_ELEMENT( "bitmap", bitmap, s );
  LINK_ELEMENT( "outlineColor", outlineColor, s );
  LINK_ELEMENT( "bw", bw, i );
  LINK_ELEMENT( "state_no", state_no, i );
  LINK_ELEMENT( "nbins", nbins, i );
  LINK_ELEMENT( "maxbin", maxbin, f );
  LINK_ELEMENT( "start", start, f );
  LINK_ELEMENT( "end", end, f );
  LINK_ELEMENT( "left", left, f );
  LINK_ELEMENT( "right", right, f );
  LINK_ELEMENT( "width", width, i );
  LINK_ELEMENT( "height", height, i );
  LINK_ELEMENT( "scan_time", scan_time, f );
  LINK_ELEMENT( "rclick_time", rclick_time, f );
  LINK_ELEMENT( "lclick_time", lclick_time, f );
  LINK_ELEMENT( "yclick", yclick, f );
  LINK_ELEMENT( "n", n, i );
  LINK_ELEMENT( "sum", sum, f );
  LINK_ELEMENT( "average", average, f );
  LINK_ELEMENT( "std_dev", std_dev, f );
  LINK_ELEMENT( "shortest", shortest, f );
  LINK_ELEMENT( "longest", longest, f );
  LINK_ELEMENT( "vis_n", vis_n, i );
  LINK_ELEMENT( "vis_sum", vis_sum, f );
  LINK_ELEMENT( "vis_average", vis_average, f );
  LINK_ELEMENT( "vis_std_dev", vis_std_dev, f );
  LINK_ELEMENT( "vis_shortest", vis_shortest, f );
  LINK_ELEMENT( "vis_longest", vis_longest, f );

  free( tmp );

  return 0;
}



#define UNLINK_ELEMENT( str_name ) \
  sprintf( tmp, "%s(%s,%s)", hist->array_name, hist->idx_prefix, str_name ); \
  Tcl_UnlinkVar( interp, tmp );

static int UnlinkVars( interp, hist )
Tcl_Interp *interp;
stateHist *hist;
{
  char *tmp;

  /* tmp will get filled with something like "timeline(.win.0,stuff)",
     or ("%s(%s,%s)", hist->array_name, hist->idx_prefix, element_name ), to
     be exact.
  */
  int max_element_len = 14;  /* "xscrollcommand" */

  tmp = malloc( strlen(hist->array_name) + 1 + strlen(hist->idx_prefix) + 1 +
	        max_element_len + 1 + 1 );

  UNLINK_ELEMENT( "window" );
  UNLINK_ELEMENT( "canvas" );
  UNLINK_ELEMENT( "time_lbl" );
  UNLINK_ELEMENT( "xscrollbar" );
  UNLINK_ELEMENT( "xscrollcommand" );
  UNLINK_ELEMENT( "color" );
  UNLINK_ELEMENT( "bitmap" );
  UNLINK_ELEMENT( "outlineColor" );
  UNLINK_ELEMENT( "bw" );
  UNLINK_ELEMENT( "state_no" );
  UNLINK_ELEMENT( "nbins" );
  UNLINK_ELEMENT( "maxbin" );
  UNLINK_ELEMENT( "start" );
  UNLINK_ELEMENT( "end" );
  UNLINK_ELEMENT( "left" );
  UNLINK_ELEMENT( "right" );
  UNLINK_ELEMENT( "width" );
  UNLINK_ELEMENT( "height" );
  UNLINK_ELEMENT( "scan_time" );
  UNLINK_ELEMENT( "rclick_time" );
  UNLINK_ELEMENT( "lclick_time" );
  UNLINK_ELEMENT( "yclick" );
  UNLINK_ELEMENT( "n" );
  UNLINK_ELEMENT( "sum" );
  UNLINK_ELEMENT( "average" );
  UNLINK_ELEMENT( "std_dev" );
  UNLINK_ELEMENT( "shortest" );
  UNLINK_ELEMENT( "longest" );
  UNLINK_ELEMENT( "vis_n" );
  UNLINK_ELEMENT( "vis_sum" );
  UNLINK_ELEMENT( "vis_average" );
  UNLINK_ELEMENT( "vis_std_dev" );
  UNLINK_ELEMENT( "vis_shortest" );
  UNLINK_ELEMENT( "vis_longest" );

  free( tmp );

  return 0;
}




static int BinsCmd( interp, hist, argc, argv )
Tcl_Interp *interp;
stateHist *hist;
int argc;
char *argv[];
{
  char print_int[50], *tallest_bin_var;
  int nbins, *bins, i, tallest;
  double startTime, endTime;
  
  if (TCL_OK != ConvertArgs( interp,
	   "hist <hist_token> bins <# of bins> <start_time> <end_time> <tallest_bin_var>",
			    "3 dffs", argc, argv,
			    &nbins, &startTime, &endTime, &tallest_bin_var )) {
    return TCL_ERROR;
  }
    /* create array for all the bin sizes */
  bins = (int*)malloc( sizeof(int) * nbins );
    /* split the data into the bins */
  State_HistBins( hist, startTime, endTime, bins, nbins );
  
  tallest = bins[0];

    /* create a list of the the sizes of each bin */
  for (i=0; i<nbins; i++) {
    sprintf( print_int, "%d ", bins[i] );
    Tcl_AppendResult( interp, print_int, (char*)0 );
    if (bins[i] > tallest) tallest = bins[i];
  }

  /* free the array */
  free( (char*)bins );

  sprintf( print_int, "%d", tallest );
  if (!Tcl_SetVar2( interp, tallest_bin_var, (char*)0, print_int, 0 )) {
    return TCL_ERROR;
  }
      
  return TCL_OK;
}



static int PointlistCmd( interp, hist, argc, argv )
Tcl_Interp *interp;
stateHist *hist;
int argc;
char *argv[];
{
  char tmp_str[200], *listVarName;
  int *point_array;
  int nbins, width, height, i;
  double startTime, endTime;
  Tcl_DString point_list;
  
  if (TCL_OK != ConvertArgs( interp,
			     "hist <hist_token> pointlist <list_var> <# of bins> <start_time> <end_time> <width> <height>",
			    "3 sdffdd", argc, argv, &listVarName,
			    &nbins, &startTime, &endTime, &width, &height )) {
    return TCL_ERROR;
  }

    /* zero bins could mess stuff up */
  if (!nbins) return TCL_OK;

  point_array = (int*)malloc( sizeof(int) * 4 * (nbins + 1) );
    /* fill the array with point coordinates */
  Pointlist( hist, point_array );

  Tcl_DStringInit( &point_list );

  for (i=0; i<nbins; i++) {
      /* a little loop unrolling here, why not? */
    sprintf( tmp_str, "%d %d %d %d ",
	     point_array[i*4+0],
	     point_array[i*4+1],
	     point_array[i*4+2],
	     point_array[i*4+3] );
    Tcl_DStringAppend( &point_list, tmp_str, -1 );
  }

  sprintf( tmp_str, "%d %d %d %d",
	   point_array[i*4+0],
	   point_array[i*4+1],
	   point_array[i*4+2],
	   point_array[i*4+3] );
  Tcl_DStringAppend( &point_list, tmp_str, -1 );

  if (!Tcl_SetVar( interp, listVarName, Tcl_DStringValue( &point_list ),
		  TCL_LEAVE_ERR_MSG ))
    return TCL_ERROR;

  Tcl_DStringFree( &point_list );

  return TCL_OK;
}



static int Draw( interp, hist, argc, argv )
Tcl_Interp *interp;
stateHist *hist;
int argc;
char *argv[];
{
  int *point_array;
  typedef char num_str_type[25];
  num_str_type *num_str;
  char **call_argv;
  char tags[50];
  int i, npoints;
  Tcl_CmdInfo cmd_info;

    /* 0 bins could cause me problems */
  if (!hist->nbins) return TCL_OK;

  npoints = 4 * (hist->nbins + 1);
  point_array = (int*)malloc( sizeof(double) * npoints );
    /* fill the array with point coordinates */
  Pointlist( hist, point_array );

  num_str = (num_str_type*)malloc( 25 * npoints );

  for (i=0; i<npoints; i++) {
    sprintf( num_str[i], "%d", point_array[i] );
  }

    /* create call_argv array */
  call_argv = (char**)malloc( sizeof(char*) * (npoints + 9) );

    /* eval $canvas create polygon $pointlist $fill_cmd $filler -tags hist */
  call_argv[0] = hist->canvas;
  call_argv[1] = "create";
  call_argv[2] = "polygon";
  for (i=0; i<npoints; i++) {
    call_argv[i+3] = num_str[i];
  }
  call_argv[i+3] = hist->bw ? "-stipple" : "-fill";
  call_argv[i+4] = hist->bw ? hist->bitmap : hist->color;
  call_argv[i+5] = "-tags";
  sprintf( tags, "hist color_%d", hist->state_no );
  call_argv[i+6] = tags;
  call_argv[i+7] = 0;
  call_argv[i+8] = 0;

  Tcl_GetCommandInfo( interp, hist->canvas, &cmd_info );

  Tcl_CallArgv( cmd_info, interp, i+7, call_argv );

  call_argv[2] = "line";
  call_argv[7] = "-fill";
  call_argv[8] = hist->outlineColor;
  call_argv[9] = "-tags";
  call_argv[10] = "hist color_fg";
  call_argv[11] = call_argv[12] = 0;
  for (i=0; i<npoints-2; i+=2) {
    call_argv[3] = num_str[i];
    call_argv[4] = num_str[i+1];
    call_argv[5] = num_str[i+2];
    call_argv[6] = num_str[i+3];
    Tcl_CallArgv( cmd_info, interp, 11, call_argv );
  }
  call_argv[3] = num_str[i];
  call_argv[4] = num_str[i+1];
  call_argv[5] = num_str[0];
  call_argv[6] = num_str[1];
  Tcl_CallArgv( cmd_info, interp, 11, call_argv );

  free( (char*)call_argv );

  return TCL_OK;
}




/* point_list will be filled with 4*(hist->nbins + 1) coordinates */

static int Pointlist( hist, point_list )
stateHist *hist;
int *point_list;
{
  int i, tallest_bin;
  int *bins;
  double hscale, vscale;
  double this_height, last_height;

    /* zero bins could mess stuff up */
  if (!hist->nbins) return TCL_OK;

    /* if we're computing the same as before, don't compute */
  if (hist->last_bins &&
      hist->last_nbins == hist->nbins &&
      hist->last_left == hist->left &&
      hist->last_right == hist->right) {
    bins = hist->last_bins;
    hist->last_bins = 0;
    /* don't worry, we'll put the pointer back when we're done */
  } else {
    if (hist->last_bins)
      free( (char*)hist->last_bins );
	
      /* create array for all the bin sizes */
    bins = (int*)malloc( sizeof(int) * hist->nbins );
      /* split the data into the bins */
    State_HistBins( hist, hist->left, hist->right,
		    bins, hist->nbins );
  }
  
    /* pick out tallest bin */
  tallest_bin = bins[0];
  for (i=0; i<hist->nbins; i++) {
    if (bins[i] > tallest_bin)
      tallest_bin = bins[i];
  }
  if (!tallest_bin) tallest_bin = 1;

    /* If maxbin is nonpositive, it is a percentage telling how much
       of the vertical height of the window should be filled with the
       tallest bin.  */

  if (hist->maxbin <= 0) {
    vscale = (-hist->maxbin) / (100 * tallest_bin) * hist->height;
  } else {
    vscale = (double)hist->height / hist->maxbin;
  }

  hscale = (double)hist->width / hist->nbins;

  last_height = 0;
    /* build each point of the histogram */

/*
   1----2
   |    |
   |    |    5----6
   |    |    |    |
   |    3----4    |
   |              |
   0              7
*/

  for (i=0; i<hist->nbins; i++) {
    this_height = bins[i] * vscale;
      /* if the bin has anything in it, make it at least show */
    if (this_height < 1  &&  bins[i] != 0)
      this_height = 1.0;

    point_list[i*4+0] = i*hscale;
    point_list[i*4+1] = hist->height - last_height;
    point_list[i*4+2] = i*hscale;
    point_list[i*4+3] = hist->height - this_height;

    last_height = this_height;
  }

  point_list[i*4+0] = hist->width;
  point_list[i*4+1] = hist->height - this_height;
  point_list[i*4+2] = hist->width;
  point_list[i*4+3] = hist->height;

    /* save it.  Free it next time if you have to. */
  hist->last_bins = bins;
  hist->last_nbins = hist->nbins;
  hist->last_left = hist->left;
  hist->last_right = hist->right;

  return TCL_OK;
}




static stateHist *HistToken2Ptr( interp, token )
Tcl_Interp *interp;
char *token;
{
  stateHist *hist;
  int ptr_no;

    /* the token will be of the form hist32, or hist19, or whatever */

  if (!(sscanf( token, "hist%d", &ptr_no )) ||
      !(hist = GetTclPtr( ptr_no ))) {
    Tcl_AppendResult( interp, token, " -- unrecognized histogram token",
		      (char*)0 );
    return 0;
  } else {
    return hist;
  }
}


static int CompareLen( va, vb )
#if !(defined(sparc) || defined(hpux)) || defined(__STDC__)
const
#endif
void *va, *vb;
{
  return (*(double*)va > *(double*)vb) ? 1 : -1;
}



static int State_HistBins( hist, shortLen, longLen, bins, nbins )
stateHist *hist;
double shortLen, longLen;
int *bins, nbins;
{
  int i, bin_no;
  double binLen;		/* length of each bin; how wide a range */
				/* of seconds each bin covers */
  double nextBin;		/* start of the next bin */
  double *lenptr;		/* where we are in the BIG LIST */

  if (!hist->n) {
    for (i=0; i<nbins; i++)
      bins[i] = 0;
    hist->vis_n = 0;
    hist->vis_sum = 0;
    hist->vis_average = 0;
    hist->vis_std_dev = 0;
    hist->vis_shortest = 0;
    hist->vis_longest = 0;
    return 0;
  }

    /* use the latest firstIdx if it's valid */

    /* find first length */
  if (shortLen == hist->shortLen) {
    i = hist->firstIdx;
  } else {
    i = BsearchGE( hist, shortLen, 0, hist->n - 1 );
    hist->shortLen = shortLen;
    hist->firstIdx = i;
  }


  binLen = (longLen - shortLen) / nbins;

  nextBin = shortLen + binLen;
  bin_no = 0;

  lenptr = hist->lens + i;

  Stats_Reset( hist->vis_stats );
  bins[0] = 0;

  for (; i < hist->n && *lenptr <= longLen; i++, lenptr++) {
      /* don't want to go past the last bin */
      /* skip until the bin for this guy is found */
    while (bin_no < nbins-1 && *lenptr > nextBin) {
      bin_no++;
      bins[bin_no] = 0;
      nextBin += binLen;
    }
    Stats_Add( hist->vis_stats, *lenptr );
    bins[bin_no]++;
  }

    /* fill remaining bins with 0 */
  while (++bin_no < nbins) {
    bins[bin_no] = 0;
  }

  hist->vis_n = Stats_N( hist->vis_stats );

  hist->vis_sum = Stats_Sum( hist->vis_stats );
  hist->vis_average = Stats_Av( hist->vis_stats );
  hist->vis_std_dev = Stats_StdDev( hist->vis_stats );
  hist->vis_shortest = Stats_Min( hist->vis_stats );
  hist->vis_longest = Stats_Max( hist->vis_stats );

#if 0
  while (bin_no < nbins-1) {
      /* get the index of the last length contained in this bin */
    bins[bin_no] = binEnd - i;

    /*
      fprintf( stderr, "nextBin= %f, binEnd = %d, bins[%d] = %d\n",
	       nextBin, binEnd, bin_no, binEnd - i );
    */

    i = binEnd;
    nextBin += binLen;
    bin_no++;
  }
  bins[bin_no] = lastIdx - i;
#endif


  return 0;
}



static int ScrollCmd( interp, hist, argc, argv )
Tcl_Interp *interp;
stateHist *hist;
int argc;
char *argv[];
{
  int windowUnits, firstUnit;
  double fullSpan, visSpan;
  char cmd[200];

  visSpan = hist->right - hist->left;
  fullSpan = hist->longest - hist->shortest;

    /* send left/span command to time_lbl */
  sprintf( cmd, "%s setview %.10f %.10f\n", hist->time_lbl, hist->left,
	   visSpan );
  if (Tcl_Eval( hist->interp, cmd ) != TCL_OK) return TCL_ERROR;

  windowUnits = 10000 * visSpan / fullSpan;
  firstUnit = 10000 * (hist->left - hist->shortest) / fullSpan;

    /* send old-fashioned scroll command to the horizontal scrollbar */
  sprintf( cmd, "%s set 10000 %d %d %d", hist->xscrollbar,
	   windowUnits, firstUnit, firstUnit + windowUnits );
  if (Tcl_Eval( hist->interp, cmd ) != TCL_OK) return TCL_ERROR;

  return TCL_OK;
}



static int XviewCmd( interp, hist, argc, argv )
Tcl_Interp *interp;
stateHist *hist;
int argc;
char *argv[];
{
  int xview;
  double visSpan, fullSpan, left;

  if (TCL_OK != ConvertArgs( interp, "hist <token> xview <x>",
			     "3 d", argc, argv, &xview )) {
    return TCL_ERROR;
  }
  
    /* the scrollbar will scroll everyone right off the screen if you
       let it */
  if (xview<0) xview = 0;

  visSpan = hist->right - hist->left;
  fullSpan = hist->longest - hist->shortest;
  left = xview * fullSpan / 10000 + hist->shortest;

  if (left < hist->shortest) {
    left = hist->shortest;
  } else if (left + visSpan > hist->longest) {
    left = hist->longest - visSpan;
  }

  if (left == hist->left) return TCL_OK;

  hist->left = left;
  hist->right = left + visSpan;

  return Tcl_VarEval( interp, "Hist_Draw ", hist->window, (char*)0 );
}




static int State_HistClose( hist )
stateHist *hist;
{
  UnlinkVars( hist->interp, hist );
  Stats_Close( hist->vis_stats );
  free( hist->array_name );
  free( hist->idx_prefix );
  free( (char*)hist->lens );
  free( (char*)hist );

  return 0;
}




  /* straight binary search, if item not found we may be one high or
     one low */
static int Bsearch( hist, x, bottom, top )
stateHist *hist;
double x;
int top, bottom;
{
  int mid;

  while (top > bottom) {
    mid = (top + bottom) / 2;

    if (hist->lens[mid] > x) {
        /* too high */
      top = mid - 1;
    } else if (hist->lens[mid] < x) {
        /* too low */
      bottom = mid + 1;
    } else {
        /* found exact value */
      return mid;
    }
  }

  return bottom;
}


#if 0
  /* binary search, return index to an item less than or equal to the
     given item */
static int BsearchLE( hist, x, bottom, top )
stateHist *hist;
double x;
int bottom, top;
{
  int i;

  i = Bsearch( hist, x, bottom, top );
  if (i && hist->lens[i] > x) {
      /* if the item found is greater than what we want, decrement */
    i--;
  } else {
      /* if the item found is equal to what we want, skip over any preceding
	 equal ones */
    while (i>0 && hist->lens[i-1] == x) i--;
  }

  return i;
}
#endif


  /* binary search, return index to an item less than or equal to */
static int BsearchGE( hist, x, bottom, top )
stateHist *hist;
double x;
int bottom, top;
{
  int i;

  i = Bsearch( hist, x, bottom, top );
  if ((i< hist->n-1) && hist->lens[i] < x) {
      /* if the item found is less than what we want, increment */
    i++;
  } else {
      /* if the item found is equal to what we want, skip over any succeeding
	 equal ones */
    while (i<top-1 && hist->lens[i+1] == x) i++;
  }

  return i;
}

