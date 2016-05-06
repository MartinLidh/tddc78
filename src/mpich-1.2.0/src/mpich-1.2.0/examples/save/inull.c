/*
 *  ovltest.c:
 *
 * simulates communication pattern of a typical program with
 *    overlapping computation and communication
 *   
 *  n processes in a chain, each with local values a[1], a[2] 
 *   and two ghost points a[0], a[3]
 *
 *  usage:
 *            ovltest
 */

#include <stdio.h>
#include <mpi.h>

int main( argc, argv )
int argc;
char *argv[];
{
   int             a[4];
   int             nproc;
   int             period;
   int             me, right, left;
   MPI_Comm        grid;
   MPI_Status      recv_status, send_status[2];
   MPI_Request     recv_req[2], send_req[2];
   int             index, it;

   /* start up */
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nproc);

   /* set up processor chain */
   period = 0;
   MPI_Cart_create(MPI_COMM_WORLD, 1, &nproc, &period, 1, &grid);
   MPI_Comm_rank(grid, &me);
   MPI_Cart_shift(grid, 0, 1, &left, &right);

  /* initialize local matrix */
  /* globally: a[i] = i, i = 1 .. 2*nproc */
  /* locally : a[i] = 2*rank+i, i=1,2 */
  a[0] = -1;
  a[1] = 2*me + 1; 
  a[2] = 2*me + 2; 
  a[3] = -1;

   /* 
    * "exchange and sweep", overlapping communication and computation  
    */

   /* start all receives and sends */
   MPI_Irecv(&a[0], 1, MPI_INT, left,  1, grid, &recv_req[0]);
   MPI_Irecv(&a[3], 1, MPI_INT, right, 0, grid, &recv_req[1]);
   MPI_Isend(&a[1], 1, MPI_INT, left, 0, grid, &send_req[0]);
   MPI_Isend(&a[2], 1, MPI_INT, right,  1, grid, &send_req[1]);

   /* do some computation on "inner points" */

   /* wait for the receives and compute corresponding edge points */ 
   for (it=0; it<=1; it++)
   {
      MPI_Waitany(2, recv_req, &index, &recv_status);

      /* which receive returned ? */
      printf("Receive no. %d on process %d completed\n", index, me);

      /* do some computations with edge points corresponding to index */
      /* what, if index == MPI_UNDEFINED ? */
   }
 
   /* finally finish the sends */
   MPI_Waitall(2, send_req, send_status);


   /* dump result */
   printf("Array on Process %d: %d %d %d %d\n", me, a[0], a[1], a[2], a[3]);
   
   /* clean up */
   MPI_Comm_free(&grid);
   MPI_Finalize();
   return 0;
}
