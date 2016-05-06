#ifndef MPID_Wtime

/* Special features of timer for MPL (allows access to global clock) */

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#ifdef MPID_TB2_TIMER
#define MPID_Wtime(t) *(t)= MPID_GTime()
#define MPID_Wtick(t) MPID_CH_Wtick( t )
#define MPID_Wtime_is_global MPID_Time_is_global

extern double MPID_GTime ANSI_ARGS((void));
extern int MPID_Time_is_global ANSI_ARGS((void));
extern void MPID_CH_Wtick ANSI_ARGS((double *));
#else
#define MPID_Wtime(t) MPID_CH_Wtime(t)
#define MPID_Wtick(t) MPID_CH_Wtick( t )
#define MPID_Wtime_is_global 0

extern double MPID_CH_Wtime ANSI_ARGS((double *));
extern void MPID_CH_Wtick ANSI_ARGS((double *));
#endif

#endif
