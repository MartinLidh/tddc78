#include <math.h>
#include "mpeconf.h"
#include "mpetools.h" 
#include "basex11.h"  

#define MPE_INTERNAL
#include "mpe.h"   /*I "mpe.h" I*/

#define DEBUG 0

#define MPE_DRAG_FIXED_RECT 8
/* this shouldn't be in mpe_graphics.h because the user shouldn't use it */

static int ConvertCoords ANSI_ARGS(( int, int, int, int, int, double,
				     int *, int *, int *, int * ));
static int DrawDragVisual ANSI_ARGS(( int, MPE_XGraph, int, int, int, int, 
				      double ));
static int MPE_Get_drag_region2 ANSI_ARGS(( MPE_XGraph, int, int, 
					    int *, int *, int *, int *, 
					    double ));

/*@
  MPE_Get_mouse_press - Waits for mouse button press

  Input Parameter:
. graph - MPE graphics handle

  Output Parameters:
+ x - horizontal coordinate of the point where the mouse button was pressed
. y - vertical coordinate of the point where the mouse button was pressed
- button - which button was pressed: 'MPE_BUTTON[1-5]'

  Notes:
  This routine waits for mouse button press, blocking
  until the mouse button is pressed inside this MPE window.
  When pressed, returns the coordinate relative to the upper right of
  this MPE window and the button that was pressed.

@*/
int MPE_Get_mouse_press( graph, x, y, button )
MPE_XGraph graph;
int *x, *y, *button;
{
  XEvent event;

  if (graph->Cookie != MPE_G_COOKIE) {
    fprintf( stderr, "Handle argument is incorrect or corrupted\n" );
    return MPE_ERR_BAD_ARGS;
  }

  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK | 
	        ButtonPressMask );
  /* add mouse presses to the events being monitored */

  XWindowEvent( graph->xwin->disp, graph->xwin->win, ButtonPressMask,
	        &event );
  /* will block until mouse pressed */

#if DEBUG
  if (event.type != ButtonPress) {
    fprintf( stderr, "In MPE_Get_mouse_press: event not a mouse press, \
it's a %s\n", eventNames[event.type] );
    *x = 0;
    *y = 0;
    *button = 0;
    XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK );
    return 0;
  }
#endif

  *x = event.xbutton.x;
  *y = event.xbutton.y;
  *button = event.xbutton.button;

  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK );
  /* turn off all events */
  return 0;
}



/*@
  MPE_Iget_mouse_press - Checks for mouse button press

  Input Parameter:
. graph - MPE graphics handle

  Output Parameters:
+ x - horizontal coordinate of the point where the mouse button was pressed
. y - vertical coordinate of the point where the mouse button was pressed
. button - which button was pressed: MPE_BUTTON[1-5]
- wasPressed - 1 if the button was pressed, 0 if not

  Notes:
  Checks if the mouse button has been pressed inside this MPE window.
  If pressed, returns the coordinate relative to the upper right of
  this MPE window and the button that was pressed.


@*/
int MPE_Iget_mouse_press( graph, x, y, button, wasPressed )
MPE_XGraph graph;
int *x, *y, *button, *wasPressed;
{
  XEvent event;

  if (graph->Cookie != MPE_G_COOKIE) {
    fprintf( stderr, "Handle argument is incorrect or corrupted\n" );
    return MPE_ERR_BAD_ARGS;
  }

  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK | 
	        ButtonPressMask );
  /* add mouse presses to the events being monitored */

  if (XCheckWindowEvent( graph->xwin->disp, graph->xwin->win, ButtonPressMask,
			 &event ) == False) {
    *wasPressed = 0;
    return MPE_SUCCESS;
  }
  /* will check once if mouse has been pressed */

  *wasPressed = 1;
  *x = event.xbutton.x;
  *y = event.xbutton.y;
  *button = event.xbutton.button;

  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK );
  /* turn off all events */

  return MPE_SUCCESS;
}


/* convert start and end coordinates to x,y,width,height coords. */
/* different transformations required for each dragVisual. */

static int ConvertCoords( dragVisual, x1, y1, x2, y2, ratio,
		      cx1, cy1, cwidth, cheight )
int dragVisual, x1, y1, x2, y2, *cx1, *cy1, *cwidth, *cheight;
double ratio;
{
  int width, height, left, top, longestSide;
  double dist;

  if (x1<x2) {
    left = x1; width = x2-x1;
  } else {
    left = x2; width = x1-x2;
  }
  if (y1<y2) {
    top = y1; height = y2-y1;
  } else {
    top = y2; height = y1-y2;
  }
  dist = (double)sqrt( (double)((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)) );
  longestSide = (width>height)?width:height;

  switch (dragVisual) {
  case MPE_DRAG_LINE:
    *cx1 = x1;
    *cy1 = y1;
    *cwidth = x2;
    *cheight = y2;
    break;

  case MPE_DRAG_FIXED_RECT:
    if (width*ratio > height) {
      /* width is limiting factor */
      height = width * ratio;
    } else {
      width = height / ratio;
    }
    *cx1 = (x1>x2) ? (x1-width)  : x1;
    *cy1 = (y1>y2) ? (y1-height) : y1;
    *cwidth = width;
    *cheight = height;
    break;

  case MPE_DRAG_SQUARE:
    *cx1 = (x1>x2) ? (x1-longestSide) : x1;
    *cy1 = (y1>y2) ? (y1-longestSide) : y1;
    *cwidth = longestSide;
    *cheight = longestSide;
    break;

  case MPE_DRAG_CIRCLE_RADIUS:
    *cx1 = x1-dist;
    *cy1 = y1-dist;
    *cwidth = dist*2;
    *cheight = dist*2;
    break;

  case MPE_DRAG_CIRCLE_DIAMETER:
    *cx1 = (x1+x2)/2 - dist/2;
    *cy1 = (y1+y2)/2 - dist/2;
    *cwidth = dist;
    *cheight = dist;
    break;

  case MPE_DRAG_CIRCLE_BBOX:
    *cx1 = (x1>x2) ? (x1-longestSide) : x1;
    *cy1 = (y1>y2) ? (y1-longestSide) : y1;
    *cwidth = longestSide;
    *cheight = longestSide;
    break;

  default:
  case MPE_DRAG_OVAL_BBOX:
  case MPE_DRAG_RECT:

    *cx1 = left;
    *cy1 = top;
    *cwidth = width;
    *cheight = height;
    break;
  }

  return 0;
}


/* ratio is in terms of height/width */

static int DrawDragVisual( dragVisual, graph, x1, y1, x2, y2, ratio )
int dragVisual, x1, y1, x2, y2;
double ratio;
MPE_XGraph graph;
{
  int width, height, top, left;

/*
  if ( !(x1-x2) || !(y1-y2) ) {
    return 0;
  } else {
    fprintf( stderr, "%d %d %d %d\n", x1, y1, x2, y2 );
  }
*/

  ConvertCoords( dragVisual, x1, y1, x2, y2, ratio, &left, &top,
		 &width, &height );


  switch (dragVisual) {
  case MPE_DRAG_NONE: break;
  case MPE_DRAG_FIXED_RECT:
  case MPE_DRAG_SQUARE:
  case MPE_DRAG_RECT:
    XDrawRectangle( graph->xwin->disp, graph->xwin->win,
		    graph->xwin->gc.set, left, top, width, height );
    break;

  case MPE_DRAG_CIRCLE_RADIUS:
  case MPE_DRAG_CIRCLE_DIAMETER:
  case MPE_DRAG_CIRCLE_BBOX:
  case MPE_DRAG_OVAL_BBOX:
    XDrawArc( graph->xwin->disp, graph->xwin->win,
	      graph->xwin->gc.set, left, top, width, height,
	      0, 360*64 );
    break;

  default:
  case MPE_DRAG_LINE:
    XDrawLine( graph->xwin->disp, graph->xwin->win,
	       graph->xwin->gc.set, left, top, width, height );
    break;

  }
  MPE_Update( graph );
  return 0;
}




static int MPE_Get_drag_region2( graph, button, dragVisual, pressx, pressy,
			     releasex, releasey, ratio )
MPE_XGraph graph;
double ratio;
int *pressx, *pressy, *releasex, *releasey, button, dragVisual;
{
  int ispressed, isreleased, lastx, lasty, firstMotion, dashOffset,
      top, left, width, height;
  unsigned motionMask;
  XEvent event;

  motionMask = (button==Button1) ? Button1MotionMask :
	        (button==Button2) ? Button2MotionMask :
	        (button==Button3) ? Button3MotionMask :
	        (button==Button4) ? Button4MotionMask : Button5MotionMask;

  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK |
	        ButtonPressMask | ButtonReleaseMask |
	        motionMask | graph->input_mask );
  /* add button presses, releases, and motion to the mask of events to
     be monitored */
  
  ispressed = isreleased = dashOffset = 0;
  firstMotion = 1;

  while (!isreleased) {
    while (XCheckWindowEvent( graph->xwin->disp, graph->xwin->win,
			      ButtonPressMask | ButtonReleaseMask |
			      motionMask | graph->input_mask, 
			      &event ) == False) {
      
	if (!firstMotion) {
	  DrawDragVisual( dragVisual, graph, *pressx, *pressy,
			   lastx, lasty, ratio );

	  MPE_Dash_offset( graph, dashOffset++ );
	  if (dashOffset == 8) dashOffset = 0;
	  DrawDragVisual( dragVisual, graph, *pressx, *pressy,
			   event.xmotion.x, event.xmotion.y, ratio );
	  XSync( graph->xwin->disp, False );
	  /* don't want to create a drawing overload */
	}
      }
    if (!ispressed && event.type == ButtonPress &&
	event.xbutton.button == button) {
      /* we want to throw out any releases before the first press */

      *pressx = event.xbutton.x;
      *pressy = event.xbutton.y;
      ispressed = 1;

      MPE_Draw_logic( graph, MPE_LOGIC_XOR );
      MPE_Draw_dashes( graph, 4 );

      /* set all drawing (the drag rectangle) to invert whatever it's on */

    } else if (ispressed) {
      if (event.type == MotionNotify) {
	if (!firstMotion) {
	  DrawDragVisual( dragVisual, graph, *pressx, *pressy,
			  lastx, lasty, ratio );
	}

	DrawDragVisual( dragVisual, graph, *pressx, *pressy,
		        event.xmotion.x, event.xmotion.y, ratio );

	lastx = event.xmotion.x;
	lasty = event.xmotion.y;
	firstMotion = 0;

      } else if (event.type == ButtonRelease && 
		 event.xbutton.button == button) {
	/* we want to throw out any presses after the first, and any
	   releases of buttons other than the one who's press we caught */
	
	DrawDragVisual( dragVisual, graph, *pressx, *pressy,
		        event.xmotion.x, event.xmotion.y, ratio );

	*releasex = event.xbutton.x;
	*releasey = event.xbutton.y;
	isreleased = 1;
      }
      else if (graph->event_routine) {
          /* Process unexpected events */
	  (*graph->event_routine)( graph, &event );
          }
    }
      else if (graph->event_routine) {
          /* Process unexpected events */
	  (*graph->event_routine)( graph, &event );    }
  }

  ConvertCoords( dragVisual, *pressx, *pressy, *releasex, *releasey,
		 ratio, &left, &top, &width, &height );
  *pressx = left;
  *pressy = top;
  *releasex = left + width;
  *releasey = top + height;
  MPE_Draw_logic( graph, MPE_LOGIC_COPY );
  MPE_Draw_dashes( graph, 0 );
  XSelectInput( graph->xwin->disp, graph->xwin->win, MPE_XEVT_IDLE_MASK );

  return 0;
}



int MPE_Get_drag_region( graph, button, dragVisual, pressx, pressy, releasex,
		      releasey )
MPE_XGraph graph;
int *pressx, *pressy, *releasex, *releasey, button, dragVisual;
{
  if (dragVisual != MPE_DRAG_NONE &&
      dragVisual != MPE_DRAG_LINE &&
      dragVisual != MPE_DRAG_RECT &&
      dragVisual != MPE_DRAG_SQUARE &&
      dragVisual != MPE_DRAG_CIRCLE_RADIUS &&
      dragVisual != MPE_DRAG_CIRCLE_DIAMETER &&
      dragVisual != MPE_DRAG_CIRCLE_BBOX &&
      dragVisual != MPE_DRAG_OVAL_BBOX ) {
    fprintf( stderr, "Invalid drag visual.  Using MPE_DRAG_LINE.\n" );
    dragVisual = MPE_DRAG_LINE;
  }


  return MPE_Get_drag_region2( graph, button, dragVisual, pressx, pressy,
			      releasex, releasey, 1.0 );
}

int MPE_Get_drag_region_fixratio( graph, button, ratio, pressx, pressy, 
				  releasex, releasey )
MPE_XGraph graph;
double ratio;
int *pressx, *pressy, *releasex, *releasey, button;
{
  int returnVal, height, width;

  returnVal =  MPE_Get_drag_region2( graph, button, MPE_DRAG_FIXED_RECT,
				    pressx, pressy,
				    releasex, releasey, ratio );
  if (*pressx < *releasex) {
    width = *releasex - *pressx;
  } else {
    width = *pressx - *releasex;
  }
  if (*pressy < *releasey) {
    height = *releasey - *pressy;
  } else {
    height = *pressy - *releasey;
  }

  if (width*ratio > height) {
    /* width is limiting factor */
    height = width * ratio;
    if (*releasey > *pressy) {
      *releasey = *pressy + height;
    } else {
      *releasey = *pressy - height;
    }
  } else {
    width = height / ratio;
    if (*releasex > *pressx) {
      *releasex = *pressx + width;
    } else {
      *releasex = *pressx - width;
    }
  }
  return returnVal;
}


