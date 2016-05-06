#ifndef MPID_Wtime

/* Special clock for P4 */

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#define MPID_Wtime(t) *(t) = p4_usclock()

#define MPID_Wtick(t) MPID_CH_Wtick(t)
extern double p4_usclock ANSI_ARGS((void));
void MPID_CH_Wtick ANSI_ARGS(( double * ));
#endif
