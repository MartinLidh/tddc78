/*
   Feather canvas item creation--fast canvas item creation

   Ed Karrels
   Argonne National Laboratory
*/



#ifndef FEATHER_H
#define FEATHER_H

#include "X11/Xlib.h"
#include "tcl.h"
#include "tk.h"


#define FEATHER_FILLSTIPPLE  0x01L
#define FEATHER_FILLCOLOR    0x02L
#define FEATHER_WIDTH        0x04L
#define FEATHER_OUTLINECOLOR 0x08L
#define FEATHER_TAGLIST      0x10L
#define FEATHER_BELOW        0x20L
#define FEATHER_ABOVE        0x40L

  /* line items only */
#define FEATHER_ARROW        0x80L
#define FEATHER_ARROWSHAPE   0x100L
#define FEATHER_CAPSTYLE     0x200L
#define FEATHER_JOINSTYLE    0x400L
#define FEATHER_SMOOTH       0x800L
#define FEATHER_SPLINESTEPS  0x1000L

  /* line items */
typedef enum {none, first, last, both} Feather_Arrow;


typedef void *Feather_Color;
typedef void *Feather_Bitmap;

typedef struct {
    /* Feather_GetColor() */
  Feather_Color fillColor;
  Feather_Color outlineColor;
    /* Tk_GetBitmap */
  Feather_Bitmap fillStipple;
    /* Tk_GetUid */
  Tk_Uid *tagList;
  int ntags;
    /* Tk_GetPixels, or manually */
  int width;
  int above;			/* be warned that above and below are slow */
  int below;
} Feather_RectInfo;


/* default is in () */

typedef struct {    
  Feather_Arrow arrow;		/* (none), first, last, or both */
  double arrowShapeA;		/* default is 8, 10, 3 */
  double arrowShapeB;
  double arrowShapeC;
  int capStyle;			/* (CapButt), CapProjecting, or CapRound. */
  Feather_Color fg;		/* default is black */
  int joinStyle;		/* (JoinRound), JoinBevel, or JoinMiter */
  int smooth;			/* nonzero for bezier curves, 0 for straight */
  int splineSteps;		/* only values 1 <= splineSteps <= 100 */
  Feather_Bitmap fillStipple;	/* default is NULL (None) */
  Tk_Uid *tagList;		/* default is no tags */
  int ntags;
  int width;			/* default is 1 */
  int above;			/* default is to attach to the end */
  int below;
} Feather_LineInfo;
  


#ifdef __STDC__
#define ARGS(x) x
#else
#define ARGS(x) ()
#endif


int Feather_GetCanvasPtr ARGS(( Tcl_Interp *interp,
			        char *canvasName,
			        void **canvasPtr,
			        Tk_Window *canvasWin ));

Feather_Color Feather_GetColor ARGS(( Tcl_Interp*,
				      Tk_Window,
				      Colormap,
				      char *name ));

Feather_Bitmap Feather_GetBitmap ARGS(( Tcl_Interp*,
				        Tk_Window,
				        char *name ));

XColor *Feather_UseColor ARGS(( Feather_Color ));
void Feather_FreeColor ARGS(( Feather_Color ));

  /* return -1 on failure, item id otherwise */
int Feather_CreateRect ARGS(( Tcl_Interp *interp,
			      void *canvasPtr,
			      double x1,
			      double y1,
			      double x2,
			      double y2,
			      Feather_RectInfo *rectInfo,
			      unsigned long rectMask ));

  /* return -1 on failure, item id otherwise */
int Feather_CreateLine ARGS(( Tcl_Interp *interp,
			      void *canvasPtr,
			      int npoints,
			      double *points,
			      int keeppoints,
			      Feather_LineInfo *lineInfo,
			      unsigned long lineMask ));

   /* redraw arrows after a scale */
int Feather_ReconfigureArrows ARGS(( void *canvasVoidPtr ));

int Feather_Lower ARGS(( Tcl_Interp *interp,
			 void *canvasPtr,
			 int source,
			 int dest ));


/*
void FeatherConfigTags ARGS(( Tk_Item *itemPtr,
			      int ntags,
			      Tk_Uid *tagList ));
*/

#endif
  /* FEATHER_H */
