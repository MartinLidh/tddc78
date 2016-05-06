#if defined(HAVE_MPICHCONF_H) && !defined(MPICHCONF_INC)
/* This includes the definitions found by configure, and can be found in
   the library directory (lib/$ARCH/$COMM) corresponding to this configuration
 */
#define MPICHCONF_INC
#include "mpichconf.h"
#endif

#include "mpid_time.h"
/* 
   This returns a value that is correct but not the best value that
   could be returned.
   It makes several separate stabs at computing the tickvalue.
*/
void MPID_CH_Wtick( tick )
double *tick;
{
    static double tickval = -1.0;
    double t1, t2;
    int    cnt;
    int    icnt;

    if (tickval < 0.0) {
	tickval = 1.0e6;
	for (icnt=0; icnt<10; icnt++) {
	    cnt = 1000;
	    MPID_Wtime( &t1 );
	    while (cnt--) {
		MPID_Wtime( &t2 );
		if (t2 > t1) break;
		}
	    if (cnt && t2 > t1 && t2 - t1 < tickval)
		tickval = t2 - t1;
	}
    }
    *tick = tickval;
}
