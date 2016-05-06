#include <stdio.h>

#include "mpi.h"
#include "mpptest.h"
extern int __NUMNODES, __MYPROCID;

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* 
   Test of single process memcpy.
   ctx is ignored for this test.
*/
double memcpy_rate(reps,len,ctx)
int      reps,len;
PairData *ctx;
{
  double elapsed_time;
  int  i,msg_id,myproc;
  char *sbuffer,*rbuffer;
  double t0, t1;

  sbuffer = (char *)malloc(len);
  rbuffer = (char *)malloc(len);

  myproc       = __MYPROCID;
  elapsed_time = 0;
  *(&t0)=MPI_Wtime();
  for(i=0;i<reps;i++){
      memcpy( rbuffer, sbuffer, len );
  }
  *(&t1)=MPI_Wtime();
  elapsed_time = *(&t1 )-*(&t0);

  free(sbuffer);
  free(rbuffer);
  return(elapsed_time);
}
