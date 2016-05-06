/*
   time_lbl.h - time label widget for Upshot

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
#include <math.h>
#include <string.h>
#include "tcl.h"
#include "tk.h"
#include "time_lbl.h"
#include "cvt_args.h"
#include "str_dup.h"

#define DEBUG 0
#define DEBUG_NO_CONFIG 0


/* Sorry about this kludge, but our ANSI C compiler on our suns has broken
   header files */
#ifdef GCC_WALL
int sscanf( char *, const char *, ... );
#endif

  /* size of temporary buffers for building Tcl commands */
#define TMP_CMD_LEN      200
  /* length of hash marks */
#define HASH_LEN         10
  /* # of pixels to leave between the hash mark and the text label */
#define SPACE_HASH_LBL   3
  /* # of pixels to leave between labels */
#define SPACE_LBL_LBL    10
  /* # of pixels of space below the labels */
#define SPACE_LBL_BOTTOM 3

  /* default colors */
#define DEF_TIMELBL_MONO_BG "white"
#define DEF_TIMELBL_COLOR_BG "SteelBlue"
#define DEF_TIMELBL_MONO_FG "black"
#define DEF_TIMELBL_COLOR_FG "snow"

  /* default font */
/* #define DEF_TIMELBL_FONT "7x13bold" */
/* just use the canvas default font, 7x13bold does not convert to PostScript */


static Tk_ConfigSpec configSpecs[] = {
  {TK_CONFIG_STRING, "-foreground", "foreground", "Foreground",
     DEF_TIMELBL_COLOR_FG, Tk_Offset( timelbl, fg ),
     TK_CONFIG_COLOR_ONLY, (Tk_CustomOption*)NULL},
  {TK_CONFIG_STRING, "-foreground", "foreground", "Foreground",
     DEF_TIMELBL_MONO_FG, Tk_Offset( timelbl, fg ),
     TK_CONFIG_MONO_ONLY, (Tk_CustomOption*)NULL},
  {TK_CONFIG_SYNONYM, "-fg", "foreground", (char*)0,
     (char*)0, 0, 0, (Tk_CustomOption*)0},
  {TK_CONFIG_STRING, "-background", "background", "Background",
     DEF_TIMELBL_COLOR_BG, Tk_Offset( timelbl, bg ),
     TK_CONFIG_COLOR_ONLY, (Tk_CustomOption*)NULL},
  {TK_CONFIG_STRING, "-background", "background", "Background",
     DEF_TIMELBL_MONO_BG, Tk_Offset( timelbl, bg ),
     TK_CONFIG_MONO_ONLY, (Tk_CustomOption*)NULL},
  {TK_CONFIG_SYNONYM, "-bg", "background", (char*)0,
     (char*)0, 0, 0, (Tk_CustomOption*)0},
/*
  {TK_CONFIG_STRING, "-font", "font", "Font",
     DEF_TIMELBL_FONT, Tk_Offset( timelbl, font ),
     0, (Tk_CustomOption*)NULL},
*/
  {TK_CONFIG_END, (char*)0, (char*)0, (char*)0,
     (char*)0, 0, 0, (Tk_CustomOption*)NULL}
};


#ifdef __STDC__
#define ARGS(x) x
#else
#define ARGS(x) ()
#endif

/*
   Create a timelbl widget.
*/
static int TimeLbl_create ARGS((ClientData  clientData, Tcl_Interp *interp,
			  int argc, char *argv[]));

/*
   Called on resize events.  Recalculate and redraw all labels.
*/
static int Resize ARGS(( timelbl*, int width, int height ));

/*
   Call Tk_ConfigurreWidget, and any other configuration needed (none).
*/
static int Configure ARGS(( timelbl *lbl, int argc, char **argv ));


/*
   Get the default font from the options database.
*/
/* static int GetFont ARGS(( Tcl_Interp*, char *canvas )); */

/*
   Try drawing some numbers on the canvas and see how big they
   are.  Set lbl->height.
*/
static int CalcLblSize ARGS(( timelbl* ));

/*
   Called to set new 'left' and 'span' values.
   If the span has changed, recalculate and redraw all.  Other wise,
   just shift the view and draw/erase.
*/
static int Set ARGS(( timelbl*, double left, double span ));

/*
   Used when printing.  Copy the timelbl to a different canvas.
*/
static int Copy ARGS(( Tcl_Interp*, timelbl*, int argc, char *argv[] ));

/*
   Create a new hash mark.
*/
static int CreateHash ARGS(( timelbl*, int hashNo, timelbl_hashmark * ));

/*
   Move and put a different time on an existing hash mark.
*/
static int UpdateHash ARGS(( timelbl*, int hashNo, timelbl_hashmark * ));

/*
   Delete a hash mark.
*/
static int DeleteHash ARGS(( timelbl*, timelbl_hashmark * ));

/*
   Delete all hash marks.
*/
static int EraseAllHashes ARGS(( timelbl* ));

/*
   Given the visWidth, span, and left, recalculate and redraw
   anything necessary.
*/
static int Recalc ARGS(( timelbl* ));

/*
   Try out a new hashIncrement by drawing a few of the longest labels.
   If the hashes will fit OK, return 1, otherwise 0.
*/
static int HashesWillFit ARGS(( timelbl *lbl ));

/*
   Calculate lbl->firstHashTime, and lbl->format_str.
*/
static int FiggerHashDrawingInfo ARGS(( timelbl *lbl, int ndec ));

/*
   Convert a hash # to an x coordinate.
*/
static double LblHashNo2X ARGS(( timelbl*, int hashNo ));

/*
   Convert an time to an x coordinate.
*/
static double LblTime2X ARGS(( timelbl*, double time ));

  /* Tcl_CmdProc */
static int TimeLblCmd ARGS(( ClientData, Tcl_Interp *, int argc, char **argv ));

  /* Tcl_CmdDeleteProc */
static void TimeLblDelete ARGS(( ClientData clientData ));

/*
   Event handler
*/
static void TimeLblEvents ARGS(( ClientData data, XEvent *eventPtr ));



int Timelbl_Init( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "time_lbl", TimeLbl_create,
		     (ClientData)Tk_MainWindow( interp ),
		     (Tcl_CmdDeleteProc*) 0 );
  return 0;
}


static int TimeLbl_create( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  timelbl *lbl;
  char cmd[TMP_CMD_LEN];

  lbl = (timelbl*) malloc( sizeof *lbl );
  if (!lbl) {
    Tcl_AppendResult( interp, "Failed to allocate memory for time label ",
		      "widget record.", (char*)0 );
    return TCL_ERROR;
  }

  lbl->interp = interp;

    /* convert the arguments from strings, put them in the widget record */
  if (TCL_OK != ConvertArgs( interp,
		 "time_lbl <window name> <from time> <to time>",
			     "1 sff", argc, argv, &lbl->windowName,
			     &lbl->starttime, &lbl->endtime )) {
    goto failed_1;
  }

  if (lbl->starttime >= lbl->endtime) {
    Tcl_AppendResult( interp, "start time must be less than end time",
		      (char*)0 );
    goto failed_1;
  }

    /* this string won't last, better make a copy */
  lbl->windowName = STRDUP( lbl->windowName );


    /* create the Tk window */
  lbl->win = Tk_CreateWindowFromPath( interp, (Tk_Window)clientData,
				      lbl->windowName, (char*)0 );
  if (!lbl->win) {
    Tcl_AppendResult( interp, "Failed to create window ",
		      lbl->windowName, (char*)0 );
    goto failed_2;
  }

  Tk_SetClass( lbl->win, "time_lbl" );

    /* set default colors */
  Configure( lbl, argc-4, argv+4 );

    /* build canvas name */
  lbl->canvasName = (char*)malloc( strlen(lbl->windowName)+3 );
  sprintf( lbl->canvasName, "%s.c", lbl->windowName );

    /* call Tcl to create the canvas */
  sprintf( cmd, "canvas %s -bg %s -relief sunken -scrollincrement 1",
	   lbl->canvasName, lbl->bg );
    /* and return any error code */
  if (TCL_OK != Tcl_Eval( interp, cmd )) goto failed_3;

    /* pack the canvas inside this widget's window */
  sprintf( cmd, "pack %s -expand 1 -fill both", lbl->canvasName );
    /* and return any error code */
  if (TCL_OK != Tcl_Eval( interp, cmd )) goto failed_3;

    /* calculate the height of the labels, and thus the height
       of the canvas */
  if (CalcLblSize( lbl )!= TCL_OK) goto failed_3;

    /* request the neccessary height, but dunno about width */
  Tk_GeometryRequest( lbl->win, 1, lbl->height );
    /* tell Tcl the size the widget wants */
  sprintf( cmd, "%s config -height %d",
	   lbl->canvasName, lbl->height );
    /* and return any error code */
  if (TCL_OK != Tcl_Eval( interp, cmd )) goto failed_3;

  lbl->totaltime = lbl->endtime - lbl->starttime;

  lbl->visWidth = 1;
  lbl->width = 1;

  lbl->left = -1;
  lbl->span = 0;
  lbl->lastSpan = -1;
  lbl->lastVisWidth = -1;

    /* set to NULL so nobody tries to free() */
  lbl->format_str = 0;

  lbl->xview = 0;

    /* clear the linked list of hash marks */
  lbl->head = lbl->tail = 0;

    /* create the widget command */
  Tcl_CreateCommand( interp, lbl->windowName, TimeLblCmd,
		     (ClientData)lbl, TimeLblDelete );

    /* request resize events */
  Tk_CreateEventHandler( lbl->win, (unsigned long)(StructureNotifyMask),
			 TimeLblEvents, (ClientData)lbl );

  return TCL_OK;

 failed_3:
  Tk_DestroyWindow( lbl->win );
 failed_2:
  free( (char*)lbl->windowName );
  free( (char*)lbl->canvasName );
 failed_1:
  free( (char*)lbl );
  return TCL_ERROR;
}


static void TimeLblDelete( data )
ClientData data;
{
  timelbl *lbl;
  timelbl_hashmark *node, *next;

    /* convert data pointer to my widget record pointer */
  lbl = (timelbl*) data;

  free( lbl->windowName );
  free( lbl->canvasName );

    /* free the linked list of hash labels */
  node = lbl->head;
  while (node) {
    next = node->next;
    free( node );
    node = next;
  }

    /* free the record itself */
  free( lbl );
}



static int TimeLblCmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
  timelbl *lbl;
  Tcl_DString cmd;
  int i, returnVal;
  double left, span;

  lbl = (timelbl*)clientData;

  if (!strcmp( argv[1], "setview" )) {
    if (ConvertArgs( lbl->interp, "timelbl set <left> <span>",
		     "2 ff", argc, argv, &left, &span )
	!= TCL_OK) {
      return TCL_ERROR;
    } else {
      return Set( lbl, left, span );
    }
  }

  if (!strcmp( argv[1], "copy" )) {
    return Copy( interp, lbl, argc, argv );
  }

  Tcl_DStringInit( &cmd );
  Tcl_DStringAppendElement( &cmd, lbl->canvasName );
  for (i=1; i<argc; i++) {
    Tcl_DStringAppendElement( &cmd, argv[i] );
  }
  returnVal = Tcl_Eval( interp, Tcl_DStringValue( &cmd ) );

  Tcl_DStringFree( &cmd );
  
  return returnVal;
}



static void TimeLblEvents( data, event )
ClientData data;
XEvent *event;
{
  timelbl *lbl;

  lbl = (timelbl*) data;

  switch (event->type) {
  case ConfigureNotify:
    Resize( lbl, event->xconfigure.width, event->xconfigure.height );
    break;
  }
}



static int Resize( lbl, width, height )
timelbl *lbl;
int width, height;
{
  lbl->visWidth = width;

  return Recalc( lbl );
}



static int Configure( lbl, argc, argv )
timelbl *lbl;
int argc;
char **argv;
{
  lbl->fg = lbl->bg = lbl->font = 0;

  if (TCL_OK != Tk_ConfigureWidget( lbl->interp, lbl->win, configSpecs,
				    argc, argv, (void*)lbl, 0 )) {
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__,
	     lbl->interp->result );
  }

  return 0;
}




#if 0
static int GetFont( interp, canvas )
Tcl_Interp *interp;
char *canvas;
{
  if (TCL_OK != Tcl_VarEval( interp, "option get ", canvas,
			     " font Font", (char*)0 )) {
    fprintf( stderr, "%s\n", interp->result );
    return TCL_ERROR;
  } else {
    return TCL_OK;
  }
}
#endif




static int CalcLblSize( lbl )
timelbl *lbl;
{
  char cmd[TMP_CMD_LEN];
  int tmp_text_id, top, bottom, height;

 
/*
  if (TCL_OK != GetFont( lbl->interp, lbl->canvasName )) {
    goto err;
  }
*/

/*
  font = STRDUP( lbl->interp->result );
*/

    /* create temporary object on the canvas */
  if (TCL_OK != Tcl_VarEval( lbl->interp, lbl->canvasName,
			     " create text 0 0 -text 8 ", (char*)0 )) {
    goto err;
  }

/*
  free( font );
*/

    /* grab the id of the text created */
  tmp_text_id = atoi( lbl->interp->result );

    /* get the size of the text created */
  sprintf( cmd, "%s bbox %d", lbl->canvasName, tmp_text_id );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd )) goto err;
  sscanf( lbl->interp->result, "%*d %d %*d %d", &top, &bottom );

  sprintf( cmd, "%s delete %d", lbl->canvasName, tmp_text_id );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd )) goto err;

  height = bottom - top;
  lbl->height = HASH_LEN + SPACE_HASH_LBL + height + SPACE_LBL_BOTTOM;

  return TCL_OK;

 err:
  fprintf( stderr, "%s\n", lbl->interp->result );
  return TCL_ERROR;
}



static int Set( lbl, left, span )
timelbl *lbl;
double left, span;
{
  if (span <= 0) {
    Tcl_SetResult( lbl->interp, "invalid span", TCL_STATIC );
    return TCL_ERROR;
  }

  lbl->left = left;
  lbl->span = span;

  return Recalc( lbl );
}





static int CreateHash( lbl, hashNo, node )
timelbl *lbl;
int hashNo;
timelbl_hashmark *node;
{
  char cmd[TMP_CMD_LEN], lbl_str[100];
  double xpos;

#if DEBUG>2
  fprintf( stderr, "Creating hash # %d\n", hashNo );
#endif

  xpos = LblHashNo2X( lbl, hashNo );
  node->lbl_idx = hashNo;

    /* create the hash line */
  sprintf( cmd, "%s create line %.17g 0 %.17g %d -fill %s -tags color_fg",
	   lbl->canvasName, xpos, xpos, HASH_LEN, lbl->fg );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
  sscanf( lbl->interp->result, "%d", &node->hashMark_canvas_idx );

    /* create the text label */
  sprintf( lbl_str, lbl->format_str, lbl->firstHashTime +
	   lbl->hashIncrement * hashNo );
  sprintf( cmd, "%s create text %.17g %d -anchor n -text %s -fill %s -tags color_fg",
	   lbl->canvasName, xpos, HASH_LEN+SPACE_HASH_LBL, lbl_str,
	   lbl->fg );
    /*  -font %s, lbl->font,  */
#if DEBUG>2
  fprintf( stderr, "%s\n", cmd );
#endif

  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
  sscanf( lbl->interp->result, "%d", &node->text_canvas_idx );

  return 0;
}

  

static int UpdateHash( lbl, hashNo, node )
timelbl *lbl;
int hashNo;
timelbl_hashmark *node;
{
  char cmd[TMP_CMD_LEN], lbl_str[100];
  double xpos;

#if DEBUG>2
  fprintf( stderr, "Updating hash # %d from %d\n", hashNo,
	   node->lbl_idx );
#endif

  xpos = LblHashNo2X( lbl, hashNo );
  node->lbl_idx = hashNo;
  sprintf( cmd, "%s coords %d %.17g 0 %.17g %d",
	   lbl->canvasName, node->hashMark_canvas_idx, xpos, xpos,
	   HASH_LEN );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

  /* Shouldn't have to redefine the fill color
  sprintf( cmd, "%s itemconfig %d -fill %s",
	   lbl->canvasName, node->hashMark_canvas_idx, lbl->fg );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
  */

  sprintf( lbl_str, lbl->format_str, lbl->firstHashTime +
	   lbl->hashIncrement * hashNo );
  sprintf( cmd, "%s coords %d %.17g %d", lbl->canvasName,
	   node->text_canvas_idx, xpos,
	   HASH_LEN+SPACE_HASH_LBL );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

  sprintf( cmd, "%s itemconfig %d -anchor n -text %s -fill %s",
	   lbl->canvasName, node->text_canvas_idx, lbl_str,
	   lbl->fg );
  /* -font %s lbl->font,  */
#if DEBUG>2
  fprintf( stderr, "%s\n", cmd );
#endif
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

  return 0;
}


static int DeleteHash( lbl, node )
timelbl *lbl;
timelbl_hashmark *node;
{
  char cmd[TMP_CMD_LEN];

    /* delete the hash line */
  sprintf( cmd, "%s delete %d", lbl->canvasName,
	   node->hashMark_canvas_idx );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

    /* delete the text label */
  sprintf( cmd, "%s delete %d", lbl->canvasName, node->text_canvas_idx );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

  return 0;
}




  /* erase all existing hash marks */
static int EraseAllHashes( lbl )
timelbl *lbl;
{
  timelbl_hashmark *node, *next;

    /* traverse the linked list of hash marks */
  node = lbl->head;
  while (node) {
      /* delete each one from the canvas */
    DeleteHash( lbl, node );

      /* delete each one from the linked list */
    next = node->next;
    free( (char*)node );
    node = next;
  }

  lbl->head = lbl->tail = 0;
  return 0;
}



  /* recalculate where the labels go and how to format them */
  /* this routine sets lbl->hashIncrement, lbl->firstHashTime, and
     lbl->format_str */
static int Recalc( lbl )
timelbl *lbl;
{
    /* text_width - the actual width of the text that makes up an average
       label */

    /* configure event hasn't been sent yet, so don't do anything */
  if (lbl->visWidth == -1) return TCL_OK;

    /* nobody has set me, so don't do anything */
  if (lbl->span == 0) return TCL_OK;

  if (lbl->left > lbl->endtime) {
    lbl->left = lbl->endtime;
  } else if (lbl->left < lbl->starttime) {
    lbl->left = lbl->starttime;
  }
  if (lbl->span > lbl->totaltime) {
    lbl->span = lbl->totaltime;
  }


#if DEBUG>1
  fprintf( stderr, "Start recalc\n" );
  fprintf( stderr, "span: %f, width: %f\n", lbl->span, lbl->width );
  fprintf( stderr, "time: %f to %f\n", lbl->starttime, lbl->endtime );
#endif

  if (lbl->span != lbl->lastSpan || lbl->visWidth != lbl->lastVisWidth) {

    lbl->lastSpan = lbl->span;
    lbl->lastVisWidth = lbl->visWidth;

    RecalcSpan( lbl );
    if (EraseAllHashes( lbl )!=TCL_OK) return TCL_ERROR;
  }
  RecalcLeft( lbl );
  if (Redraw( lbl )!=TCL_OK) return TCL_ERROR;

  return TCL_OK;
}


/*
   Recalculate width and hashIncrement.
*/
int RecalcSpan( lbl )
timelbl *lbl;
{
  int max_nlbls, ndec, try_again;

    /* recalculate the width of the canvas */
  lbl->width = lbl->visWidth * (lbl->totaltime) / lbl->span;

    /* minimum magnitude of the difference between hash marks */
  /* diffMag = log10( lbl->totaltime ); */
    
    /* estimate 5 pixels as the narrowest a label could get */
    /* set how many labels can be displayed */
  max_nlbls = lbl->width / (5 + SPACE_LBL_LBL);
  if (max_nlbls == 0) max_nlbls = 1;

    /* calculate the difference between hash marks */
  lbl->hashIncrement =
    pow( 10.0, ceil( log10( lbl->totaltime ) -
		     log10( (double) max_nlbls ) ) );

    /* start at a really small increment, and keep multiplying it by
       10 until the hashes required to draw it will actually fit */
  do {
    try_again = 0;
      /* figger the number of decimal places in each label */
    ndec = lbl->hashIncrement >= 1
      ? 0
	: (int)(-log10( lbl->hashIncrement )+.5);
      
      /* calculate lbl->firstHashTime, and lbl->format_str */
    FiggerHashDrawingInfo( lbl, ndec );
      /* if the hashes won't fit, spread them out by a factor of
	 10 and try again */
    if (!HashesWillFit( lbl )) {
      lbl->hashIncrement *= 10.0;
      try_again = 1;
    }
  } while (try_again);
  
    /* try 4 subdivisions */
  lbl->hashIncrement /= 4.0;
    /* if the number of decimal places is already > 0,
       increment by 2, otherwise, if the hash increment
       is >= 100, leave at 0, for 10 -> 1, for <10, set to 2 */
  ndec = (ndec) ? ndec+2 :
    (lbl->hashIncrement >= 100.0) ? 0 :
      (lbl->hashIncrement >= 10.0) ? 1 : 2;
  FiggerHashDrawingInfo( lbl, ndec );

  if (!HashesWillFit( lbl )) {
      /* if 4 won't fit, try 2 */
    lbl->hashIncrement *= 2.0;
    ndec -= (ndec) ? 1 : 0;
    FiggerHashDrawingInfo( lbl, ndec );
    if (!HashesWillFit( lbl )) {
        /* if 2 won't fit, just go back to what it was */
      lbl->hashIncrement *= 2.0;
      ndec -= (ndec) ? 1 : 0;
      FiggerHashDrawingInfo( lbl, ndec );
    }
  }

#if DEBUG
  fprintf( stderr, "labels with text of the form %s will be placed at intervals of %f starting at %f, first one at %f, x-intervals of %f\n",
	   lbl->format_str, lbl->hashIncrement,
	   lbl->firstHashTime, LblHashNo2X( lbl, 0),
	   LblHashNo2X( lbl, 1 ) -  LblHashNo2X( lbl, 0) );
#endif

  return TCL_OK;
}



int RecalcLeft( lbl )
timelbl *lbl;
{
  lbl->xview = (lbl->left - lbl->starttime) / lbl->totaltime * lbl->width;

  return TCL_OK;
}



int Redraw( lbl )
timelbl *lbl;
{
  char *cmd;
  int firstVisibleHash, lastVisibleHash;
    /* indices of the first and last visible hash */

  int hashNo;
  timelbl_hashmark *freeList, *last, *node, *freeListTail;
  timelbl_hashmark *firstExisting;

  cmd = malloc( strlen(lbl->canvasName) + 50 );

  sprintf( cmd, "%s xview %d", lbl->canvasName, lbl->xview );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );

    /* figure out the indices of the hashes at the far left and right
       edges */
  firstVisibleHash = (int)((lbl->left - lbl->starttime) / 
			   lbl->hashIncrement + .5);
  lastVisibleHash = (int)((lbl->left + lbl->span - lbl->starttime) / 
			  lbl->hashIncrement + .5);
	       
#if DEBUG>1
  fprintf( stderr, "(left,right)(time,hash): %f %d %f %d\n",
	   leftEdgeTime, firstVisibleHash, rightEdgeTime,
	   lastVisibleHash );
#endif
  

    /* get the necessary hashes drawn */

  freeList = lbl->head;
  if (!freeList) {
    firstExisting = 0;
  } else {
    if (lbl->head->lbl_idx > lastVisibleHash ||
	lbl->tail->lbl_idx < firstVisibleHash) {
        /* the lists are disjoint, reuse all the nodes */
#if DEBUG>1
      fprintf( stderr, "Lists are disjoint, reuse all the nodes.\n" );
#endif
      firstExisting = 0;
    } else {
      last = 0;
      node = freeList;

        /* add any nodes to the left of visible to the free list */
      while (node->lbl_idx < firstVisibleHash) {
#if DEBUG>2
	fprintf( stderr, "add %p %d to free list\n", (void*)node,
		 node->lbl_idx );
#endif
	last = node;
	node = node->next;
      }

      freeListTail = last;
      firstExisting = node;
      if (last) {
          /* if some free nodes were found, cut them off */
	  /* meaning, if free nodes precede existing nodes,
	     terminate the free node list so that when the free
	     nodes are used, they don't continue into existing nodes */
	last->next = 0;
      } else {
	  /* if no nodes were found, set free list to 0 */
	freeList = 0;
      }

        /* skip over the existing nodes */
      while (node && node->lbl_idx <= lastVisibleHash) {
#if DEBUG>2
        fprintf( stderr, "skipping %p %d\n", (void*)node, node->lbl_idx );
#endif
	last = node;
	node = node->next;
      }

        /* if there are still more nodes after the existing nodes */
      if (node) {
#if DEBUG>1
        fprintf( stderr, "Adding nodes after existing list to free list\n" );
#endif
	  /* finish off firstExisting list */
	last->next = 0;
	if (freeList) {
	    /* if a free list exists, attach these excess nodes to 
	       the first part of it */
	  freeListTail->next = node;
	} else {
	    /* if no free list exists, then this is it */
	  freeList = node;
	}
      }
    }
  }

  /* freeList is now either null or points to a list of nodes
     that can be re-used.  firstExisting is also either null
     or points to a list of nodes that are already set correctly */

  hashNo = firstVisibleHash;
  last = 0;

    /* first create any nodes that may be needed to precede the existing
       nodes and skip over the existing nodes */
  if (firstExisting) {  
      /* create nodes preceding existing range */
    while (hashNo < firstExisting->lbl_idx) {
        /* get valid node structure */
        /* grab nodes from the free list if possible */
#if DEBUG>2
      fprintf( stderr, "Creating node %d to precede existing list\n", hashNo );
#endif
      if (freeList) {
	node = freeList;
	freeList = freeList->next;
	UpdateHash( lbl, hashNo, node );
      } else {
	node = (timelbl_hashmark*)malloc( sizeof *node );
	if (!node) {
	  fprintf( stderr,
		   "Failed to allocate memory when creating hash labels\n" );
	}
	CreateHash( lbl, hashNo, node );
      }

        /* tell this node's predecessor who's next */
      if (last) {
	last->next = node;
      } else {
	lbl->head = node;
      }

        /* move to next hash number */
      last = node;
      hashNo++;
    }

    if (last) {
      last->next = firstExisting;
    } else {
      lbl->head = firstExisting;
    }

      /* skip over the exising portion */
    node = firstExisting;
    while (node && hashNo <= lastVisibleHash) {
#if DEBUG>2
        fprintf( stderr, "skipping %p %d\n", (void*)node, node->lbl_idx );
#endif
      last = node;
      node = node->next;
      hashNo++;
    }

      /* if node is nonzero, we have too many nodes */
    if (node) {
      lbl->tail = last;
      last->next = 0;
      while (node) {
        last = node->next;
#if DEBUG>2
        fprintf( stderr, "freeing %p %d\n", (void*)node, node->lbl_idx );
#endif
	DeleteHash( lbl, node );
	free( node );
        node = last;
      }
    }
  }

    /* creating nodes either after the exising range or fresh */
  while (hashNo <= lastVisibleHash) {
#if DEBUG>2
      fprintf( stderr, "Creating node %d after existing list\n", hashNo );
#endif
    if (freeList) {
      node = freeList;
	freeList = freeList->next;
      UpdateHash( lbl, hashNo, node );
    } else {
      node = (timelbl_hashmark*)malloc( sizeof *node );
      if (!node) {
	fprintf( stderr,
		"Failed to allocate memory when creating hash labels\n" );
      }
      CreateHash( lbl, hashNo, node );
    }
    
      /* tell this node's predecessor who's next */
    if (last) {
      last->next = node;
    } else {
      lbl->head = node;
    }
    
    last = node;
    hashNo++;
  }
  last->next = 0;
  lbl->tail = last;

    /* free anything still in the free list */
  while (freeList) {
#if DEBUG>2
        fprintf( stderr, "freeing %p %d\n", (void*)freeList,
                 freeList->lbl_idx );
#endif
    DeleteHash( lbl, freeList );
    last = freeList->next;
    free( freeList );
    freeList = last;
  }

  free( cmd );

#if DEBUG>2
  {
    timelbl_hashmark *node;
    fprintf( stderr, "*End of call to Redraw(), node list:\n" );
    node = lbl->head;
    while (node) {
      fprintf( stderr, "hash idx: %d, canvas idxs: %d %d\n",
	       node->lbl_idx, node->hashMark_canvas_idx,
	       node->text_canvas_idx );
      node = node->next;
    }
  }
#endif


  return TCL_OK;
}
  



/*
   Calculate lbl->firstHashTime and lbl->format_str
*/
static int FiggerHashDrawingInfo( lbl, ndec )
timelbl *lbl;
int ndec;
{
  char format_str[100];

    /* the value of the first label */
  lbl->firstHashTime = (int)(lbl->starttime / lbl->hashIncrement + .5) *
    lbl->hashIncrement;

  sprintf( format_str, "%%.%df", ndec );
  if (lbl->format_str) free( lbl->format_str );
  lbl->format_str = STRDUP( format_str );

  return 0;
}



static int HashesWillFit( lbl )
timelbl *lbl;
{
  char cmd[TMP_CMD_LEN], sample_num[100];
  int test_item_index, left, right, text_width;

    /* the endtime is goint to be the longest possible number displayed */
    /* for example, over the range 1.0 to 100.0 */
  sprintf( sample_num, lbl->format_str, lbl->endtime );

    /* create the text item */
  sprintf( cmd, "%s create text 0 0 -text %s", lbl->canvasName,
	   sample_num );
    /*  -font %s , lbl->font */
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
  sscanf( lbl->interp->result, "%d", &test_item_index );
  
    /* now get the coordinates of a box that encloses the text item */
  sprintf( cmd, "%s bbox %d", lbl->canvasName, test_item_index );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
    /* get only the first and third integers in the result string */
  sscanf( lbl->interp->result, "%d %*d %d", &left, &right );
  
    /* figger the width of the box */
  text_width = right - left;

    /* remove the item */
  sprintf( cmd, "%s delete %d", lbl->canvasName, test_item_index );
  if (TCL_OK != Tcl_Eval( lbl->interp, cmd ))
    fprintf( stderr, "%s, %d: %s\n", __FILE__, __LINE__, lbl->interp->result );
  
#if DEBUG>2
  fprintf( stderr,
	  "text:%s:, width: %d, space: %d, positions of 0 and 1: %f %f\n",
	  sample_num, text_width, SPACE_LBL_LBL, LblHashNo2X( lbl, 0 ),
	  LblHashNo2X( lbl, 1 ) );
#endif

    /* if just this label is wider than the entire window, let it go */
  if (text_width > lbl->width) return 1;

  if (text_width + SPACE_LBL_LBL >
      LblHashNo2X( lbl, 1 ) -  LblHashNo2X( lbl, 0)) {
      /* return 0 if the labels are too closely packed */
    return 0;
  } else {
    return 1;
  }
}



  /* convert hash number to the x coordinate it should be placed at */
static double LblHashNo2X( lbl, hash_no )
timelbl *lbl;
int hash_no;
{
  return LblTime2X( lbl, lbl->firstHashTime + hash_no * lbl->hashIncrement );
}


  /* convert time to x coordinate */
static double LblTime2X( lbl, time )
timelbl *lbl;
double time;
{
  return (time - lbl->starttime) / (lbl->totaltime) * lbl->width;
}



static int Copy( interp, lbl, argc, argv )
Tcl_Interp *interp;
timelbl *lbl;
int argc;
char **argv;
{
  char *dest_canvas;
  int x, y, first_idx, last_idx, left_chop, right_chop;
  double x0, x1;
  char bbox[100];
  char dest_coords[100];

  if (ConvertArgs( interp, "<window> copy <dest_canvas> <x> <y>",
		   "2 sdd", argc, argv, &dest_canvas, &x, &y )
      != TCL_OK) {
    return TCL_ERROR;
  }

  first_idx = lbl->head->lbl_idx;
  x0 = LblHashNo2X( lbl, first_idx );
  x1 = LblHashNo2X( lbl, first_idx+1 );

    /* If the hash mark of the first label is visible, grab the whole
       label (set left chop point to halfway between lbl(0) and lbl(-1)).
       If it is not visible, chop it out of view entirely */
  left_chop = (x0 < lbl->xview) ? (int)((x0 + x1)/2)
                                         : (int)(x0 - (x1 - x0)/2);

  last_idx = lbl->tail->lbl_idx;
  x0 = LblHashNo2X( lbl, last_idx-1 );
  x1 = LblHashNo2X( lbl, last_idx );

    /* similar deal with chopping off the last label */
  right_chop = (x1 > lbl->xview + lbl->visWidth ) ?
    (int)((x0 + x1)/2) : (int)(x1 + (x1 - x0)/2);

  sprintf( bbox, "%d %d %d %d", left_chop, 0, right_chop, lbl->height );
  sprintf( dest_coords, "%d %d", x - lbl->xview + left_chop, y );
  return Tcl_VarEval( interp, "CopyCanvas ", lbl->canvasName, " {", bbox,
		      "} ", dest_canvas, " {", dest_coords, "} color_bg",
		      (char*)0 );
}

