/* $Id: basex11.h,v 1.2 1998/09/16 19:14:31 gropp Exp $ */


/*
    This file contains a basic X11 data structure that may be used within
    other structures for basic graphics operations.
 */

#if !defined(_BASEX11)
#define _BASEX11

/* AIX assumes that sys/types is included before Xutil is (when it defines
   function prototypes) */
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef unsigned long PixVal;

/* Our rule on GC is that the current pixel value is remembered so that
   we don't contsantly call a routine to change it when it is already the
   value that we want. */
typedef struct {
    GC       set;
    PixVal   cur_pix;
    } GCCache;
   
/* 
   Many routines need the display, window, and a GC; 
   occasionally, routines need the visual and the colormap (particularly
   those doing scientific imaging).  For scaling information, the
   region of the window is also needed (note that many XBWindow
   structures can use the same Window) 
 */
typedef struct {
    Display  *disp;
    int      screen;
    Window   win;
    GCCache  gc;
    Visual   *vis;            /* Graphics visual */
    int      depth;           /* Depth of visual */
    int      numcolors,       /* Number of available colors */
             maxcolors;       /* Current number in use */
    Colormap cmap;
    PixVal   foreground, background;
    PixVal   cmapping[256];
    int      x, y, w, h;      /* Size and location of window */
    /* The following permit double buffering; by making this part of the
       XBWindow structure, everyone can utilize double buffering without
       any special arrangements.  If buf is not null, all routines draw 
       to it instead, and XBFlush does a copyarea. NOT YET IMPLEMENTED */
    Drawable drw;
    } XBWindow;

/* This definition picks the drawable to use for an X operation.  This
   should be used for all drawing routines (note that some routines need
   a Window, not just a drawable). */
#define XBDrawable(w) ((w)->drw ? (w)->drw : (w)->win)

/* There are a number of properties that we'd like to have on hand about 
   a font; in particular, a bound on the size of a character */
typedef struct {
    Font     fnt;
    int      font_w, font_h;
    int      font_descent;
    PixVal   font_pix;
    } XBFont;

/* This is a user-defined coordinates region */
typedef struct {
    double  xmin,xmax,ymin,ymax,zmin,zmax ;
    } XBAppRegion;

typedef struct {
    int      x, y, xh, yh, w, h;
    } XBRegion;

/* This is the "decoration" structure.  This could later involve
   patterns to be used outside the frame, as well as a "background"
   (interior) decoration */
typedef struct {
    XBRegion Box;
    int      width, HasColor, is_in;
    PixVal   Hi, Lo;
    } XBDecoration;
    
#define XBSetPixVal( xbwin, pixval ) \
if (xbwin->gc.cur_pix != pixval) { \
    XSetForeground( xbwin->disp, xbwin->gc.set, pixval ); \
    xbwin->gc.cur_pix   = pixval;\
    }

/* Error returns */
#define ERR_CAN_NOT_OPEN_DISPLAY 0x10001
#define ERR_NO_DISPLAY           0x10002
#define ERR_CAN_NOT_OPEN_WINDOW  0x10003
#define ERR_ILLEGAL_SIZE         0x10004

/* Routines */
#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

extern PixVal    XBGetColor ANSI_ARGS((XBWindow *, char *, int));

/* xwmap */
extern int XB_wait_map ANSI_ARGS(( XBWindow *, 
			       void (*)( XBWindow *, int, int, int, int ) ));
extern void XBSync ANSI_ARGS(( XBWindow * ));

/* xinit */
extern XBWindow *XBWinCreate ANSI_ARGS((void));
extern void      XBWinDestroy ANSI_ARGS((XBWindow *));
extern int XBOpenDisplay ANSI_ARGS(( XBWindow *, char * ));
extern int XBSetVisual   ANSI_ARGS(( XBWindow *, int, Colormap, int ));
extern int XBSetGC       ANSI_ARGS(( XBWindow *, PixVal ));
extern int XBOpenWindow  ANSI_ARGS(( XBWindow * ));
extern int XBDisplayWindow ANSI_ARGS(( XBWindow *, char *, int, int, int, int,
				       PixVal ));
extern void XBGetArgs    ANSI_ARGS(( int *, char **, int, int *, int *, 
				     int *, int * ));
extern void XBGetArgsDisplay ANSI_ARGS(( int *, char **, int, int, char * ));
extern int XBiQuickWindow ANSI_ARGS(( XBWindow *, char *, char *, 
				      int, int, int, int, int ));
extern int XBQuickWindow ANSI_ARGS((XBWindow *, char *, char *, 
				    int,int,int,int));
extern int XBQuickWindowFromWindow ANSI_ARGS(( XBWindow *, char *, Window ));
extern void XBFlush      ANSI_ARGS(( XBWindow * ));
extern void XBSetWindowLabel ANSI_ARGS(( XBWindow *, char * ));
extern void XBCaptureWindowToFile ANSI_ARGS(( XBWindow *, char * ));

/* xframe */
extern int XBFrameColors ANSI_ARGS(( XBWindow *, XBDecoration *, 
				     char *, char * ));
extern int XBDrawFrame ANSI_ARGS(( XBWindow *, XBDecoration * ));
extern void XBClearWindow ANSI_ARGS(( XBWindow *, int, int, int, int ));
extern void XBFrameColorsByName ANSI_ARGS(( XBWindow *, char *, char * ));

/* xcolor */
extern void XBInitColors ANSI_ARGS(( XBWindow *, Colormap, int ));
extern int XBInitCmap    ANSI_ARGS(( XBWindow * ));
extern int XBCmap        ANSI_ARGS(( unsigned char [], unsigned char [],
				     unsigned char [], int, XBWindow * ));
extern int XBSetVisualClass ANSI_ARGS(( XBWindow * ));
extern int XBGetVisualClass ANSI_ARGS(( XBWindow * ));
extern Colormap XBCreateColormap ANSI_ARGS(( Display *, int, Visual * ));
extern int XBSetColormap ANSI_ARGS(( XBWindow * ));
extern int XBAllocBW     ANSI_ARGS(( XBWindow *, PixVal *, PixVal * ));
extern int XBGetBaseColor ANSI_ARGS(( XBWindow *, PixVal *, PixVal * ));
extern int XBSetGamma     ANSI_ARGS(( double ));
extern int XBSetCmapHue   ANSI_ARGS(( unsigned char *, unsigned char *,
				      unsigned char *, int ));
extern int XBFindColor    ANSI_ARGS(( XBWindow *, char *, PixVal * ));
extern int XBAddCmap      ANSI_ARGS(( unsigned char [], unsigned char [],
				      unsigned char [], int, XBWindow * ));
extern PixVal XBGetColor  ANSI_ARGS(( XBWindow *, char *, int ));
extern PixVal XBSimColor  ANSI_ARGS(( XBWindow *, PixVal, int, int ));
extern void XBUniformHues ANSI_ARGS(( XBWindow *, int ));
extern void XBSetCmapLight ANSI_ARGS(( unsigned char *, unsigned char *,
				       unsigned char *, int ));
#endif
