#ifndef _MPEGRAPHICS_H_
#define _MPEGRAPHICS_H_

#include <math.h>
#include "tools.h"          /*I "tools.h" I*/
#include "basex11.h"        /*I "basex11.h" I*/
#include "mpi.h"

#define MPE_G_COOKIE 0xfeeddada

struct MPE_XGraph_s {
  int      Cookie;
  MPI_Comm comm;
  XBWindow *xwin;
  int      is_collective;
};

typedef struct MPE_XGraph_s *MPE_XGraph;

#define MPE_INTERNAL

#include "mpe_graphics.h"   /*I "mpe_graphics.h" I*/

#define MPE_XEVT_IDLE_MASK 0
/* normal XEvent mask; what it should be set to during normal processing */
/* Eventually, this should be ExposureMask or more */

#endif
