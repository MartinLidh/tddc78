/*

   Define a bunch of bitmaps

   2x2, black, boxes, dimple3, dllines3, dllines4, dlines3, drlines4,
   drlines4, gray, light_gray, gray3, hlines2, hlines3, hlines4,
   vlines2, vlines3, vlines4, white

*/

#ifndef BITMAPS_H
#define BITMAPS_H

#include "tcl.h"


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

int Bitmap_Init ANSI_ARGS(( Tcl_Interp *interp ));
int Bitmap_Reset ANSI_ARGS(( void ));
char *Bitmap_Get ANSI_ARGS(( void ));


#endif
