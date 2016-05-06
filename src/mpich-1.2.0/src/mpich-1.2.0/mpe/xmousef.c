/* xmouse.c */
/* Fortran interface file */
#include <stdio.h>
#include "mpeconf.h"
#include "mpe.h"

#include "mpetools.h"
#include "basex11.h"
#include "mpe.h"
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_get_mouse_press_ PMPE_GET_MOUSE_PRESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_get_mouse_press_ pmpe_get_mouse_press__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_get_mouse_press_ pmpe_get_mouse_press
#else
#define mpe_get_mouse_press_ pmpe_get_mouse_press_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_get_mouse_press_ MPE_GET_MOUSE_PRESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_get_mouse_press_ mpe_get_mouse_press__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_get_mouse_press_ mpe_get_mouse_press
#endif
#endif


 mpe_get_mouse_press_( graph, x, y, button, __ierr )
MPE_XGraph*graph;
int *x, *y, *button;
int *__ierr;
{
*__ierr = MPE_Get_mouse_press(*graph,x,y,button);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_iget_mouse_press_ PMPE_IGET_MOUSE_PRESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_iget_mouse_press_ pmpe_iget_mouse_press__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_iget_mouse_press_ pmpe_iget_mouse_press
#else
#define mpe_iget_mouse_press_ pmpe_iget_mouse_press_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_iget_mouse_press_ MPE_IGET_MOUSE_PRESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_iget_mouse_press_ mpe_iget_mouse_press__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_iget_mouse_press_ mpe_iget_mouse_press
#endif
#endif

 void mpe_iget_mouse_press_( graph, x, y, button, wasPressed, __ierr )
MPE_XGraph*graph;
int *x, *y, *button, *wasPressed;
int *__ierr;
{
*__ierr = MPE_Iget_mouse_press(*graph,x,y,button,wasPressed);
}
