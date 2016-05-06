/*

   Simple stats stuff

   Ed Karrels
   Argonne National Laboratory
*/

#ifndef STATS_H_
#define STATS_H_


typedef struct {
  double sum;
  double sum_sq;
  double max, min;
  int n;
} statData;


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

  /* initialize stats structure */
statData *Stats_Create ANSI_ARGS((void));

  /* add a data point */
int Stats_Add ANSI_ARGS(( statData *stats, double x ));

  /* add an array of data points */
int Stats_AddArray ANSI_ARGS(( statData *stats, double *nums, int n ));

  /* get the # of data points */
int Stats_N ANSI_ARGS(( statData *stats ));

  /* get the sum of the data points */
double Stats_Sum ANSI_ARGS(( statData *stats ));

  /* get the sum of the data points */
double Stats_Min ANSI_ARGS(( statData *stats ));

  /* get the sum of the data points */
double Stats_Max ANSI_ARGS(( statData *stats ));

  /* get the average of the data */
double Stats_Av ANSI_ARGS(( statData *stats ));

  /* get the standard deviation of the data */
double Stats_StdDev ANSI_ARGS(( statData *stats ));

  /* clear the stats data */
int Stats_Reset ANSI_ARGS(( statData *stats ));

  /* close the stats data */
int Stats_Close ANSI_ARGS(( statData *stats ));

#endif  /* #ifndef STATS_H_ */
