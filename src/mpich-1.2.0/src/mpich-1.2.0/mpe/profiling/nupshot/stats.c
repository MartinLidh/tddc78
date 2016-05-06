/*

   Simple stats stuff

   Ed Karrels
   Argonne National Laboratory
*/

#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#include <stdio.h>
#include <math.h>
#include "stats.h"


#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif


statData *Stats_Create() {
  statData *stats;

  stats = (statData*)malloc( sizeof( statData ) );
  if (!stats) {
    fprintf( stderr, "%s:%d Out of memory initializing stats\n",
	     __FILE__, __LINE__ );
    return 0;
  }

  stats->sum = 0;
  stats->sum_sq = 0;
  stats->n = 0;

  return stats;
}

  
int Stats_Add( stats, x )
statData *stats;
double x;
{
  if (!stats->n) {
    stats->max = stats->min = x;
  } else {
    if (x < stats->min) {
      stats->min = x;
    } else if (x > stats->max) {
      stats->max = x;
    }
  }

  stats->sum += x;
  stats->sum_sq += x*x;
  return ++stats->n;
}


int Stats_AddArray( stats, nums, n )
statData *stats;
double *nums;
int n;
{
  if (!stats->n) {
    stats->max = stats->min = *nums;
  }

  stats->n += n;
  for (; n>0; n--,nums++) {
    if (*nums < stats->min) {
      stats->min = *nums;
    } else if (*nums > stats->max) {
      stats->max = *nums;
    }

    stats->sum += *nums;
    stats->sum_sq += *nums * *nums;
  }
  return stats->n;
}


int Stats_N( stats )
statData *stats;
{
  return stats->n;
}


double Stats_Sum( stats )
statData *stats;
{
  return stats->sum;
}


double Stats_Max( stats )
statData *stats;
{
  return stats->max;
}


double Stats_Min( stats )
statData *stats;
{
  return stats->min;
}


double Stats_Av( stats )
statData *stats;
{
  if (!stats->n) return 0;
  return stats->sum / stats->n;
}


double Stats_StdDev( stats )
statData *stats;
{
  double av;

  if (!stats->n) return 0;

  av = stats->sum / stats->n;
  return sqrt( (stats->sum_sq - 2 * stats->sum * av + stats->n * av * av ) /
		stats->n );
}


int Stats_Reset( stats )
statData *stats;
{
  stats->sum = 0;
  stats->sum_sq = 0;
  stats->n = 0;

  return 0;
}


int Stats_Close( stats )
statData *stats;
{
  free( (void*)stats );

  return 0;
}
