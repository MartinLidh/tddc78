#if defined(HAVE_MPICHCONF_H) && !defined(MPICHCONF_INC)
/* This includes the definitions found by configure, and can be found in
   the library directory (lib/$ARCH/$COMM) corresponding to this configuration
 */
#define MPICHCONF_INC
#include "mpichconf.h"
#include "mpich-mpid.h"
#endif

#ifndef MPID_CH_Wtime
#if defined(HAVE_GETTIMEOFDAY) || defined(USE_WIERDGETTIMEOFDAY) || \
    defined(HAVE_BSDGETTIMEOFDAY)
#include <sys/types.h>
#include <sys/time.h>
#endif

void MPID_CH_Wtime( seconds )
double *seconds;
{

#if defined(USE_ALPHA_CYCLE_COUNTER)
/* Code from LinuxJournal #42 (Oct-97), p50; 
   thanks to Dave Covey dnc@gi.alaska.edu
   Untested; we don't have a Linux alpha
   Also 
 */
    unsigned long cc
    asm volatile( "rpcc %0" : "=r"(cc) : : "memory" );
    /* Convert to time.  Scale cc by 1024 incase it would overflow a double;
       consider using long double as well */
    *seconds = 1024.0 * ((double)(cc/1024) / (double)CLOCK_FREQ_HZ);
#elif defined(HAVE_BSDGETTIMEOFDAY)
    struct timeval tp;
    struct timezone tzp;

    BSDgettimeofday(&tp,&tzp);
    *seconds = ((double) tp.tv_sec + .000001 * (double) tp.tv_usec);
#elif defined(USE_WIERDGETTIMEOFDAY)
    /* This is for Solaris, where they decided to change the CALLING
       SEQUENCE OF gettimeofday! (Solaris 2.3 and 2.4 only?) */
    struct timeval tp;

    gettimeofday(&tp);
    *seconds = ((double) tp.tv_sec + .000001 * (double) tp.tv_usec);
#elif defined(HAVE_GETTIMEOFDAY)
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp,&tzp);
    *seconds = ((double) tp.tv_sec + .000001 * (double) tp.tv_usec);
#else
    *seconds = 0;
#endif
}
#endif

