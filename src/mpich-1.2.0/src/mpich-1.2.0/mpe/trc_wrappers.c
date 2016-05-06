#ifdef MPI_BUILD_PROFILING
#undef MPI_BUILD_PROFILING
#endif
#include <stdio.h>
#include "mpeconf.h"
#include "mpi.h"

#if defined(NEEDS_STDLIB_PROTOTYPES)
#include "protofix.h"
#endif

/*
   This was originally built with wrappergen, then modified to 
   print out some of the int values.
 */



int   MPI_Allgather( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm )
void * sendbuf;
int sendcount;
MPI_Datatype sendtype;
void * recvbuf;
int recvcount;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Allgather - prototyping replacement for MPI_Allgather
    Trace the beginning and ending of MPI_Allgather.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Allgather...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Allgather( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm );

  printf( "[%d] Ending MPI_Allgather\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Allgatherv( sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm )
void * sendbuf;
int sendcount;
MPI_Datatype sendtype;
void * recvbuf;
int * recvcounts;
int * displs;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Allgatherv - prototyping replacement for MPI_Allgatherv
    Trace the beginning and ending of MPI_Allgatherv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Allgatherv...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Allgatherv( sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm );

  printf( "[%d] Ending MPI_Allgatherv\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Allreduce( sendbuf, recvbuf, count, datatype, op, comm )
void * sendbuf;
void * recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Allreduce - prototyping replacement for MPI_Allreduce
    Trace the beginning and ending of MPI_Allreduce.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Allreduce...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Allreduce( sendbuf, recvbuf, count, datatype, op, comm );

  printf( "[%d] Ending MPI_Allreduce\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Alltoall( sendbuf, sendcount, sendtype, recvbuf, recvcnt, recvtype, comm )
void * sendbuf;
int sendcount;
MPI_Datatype sendtype;
void * recvbuf;
int recvcnt;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  int  returnVal;
  int llrank;

/*
    MPI_Alltoall - prototyping replacement for MPI_Alltoall
    Trace the beginning and ending of MPI_Alltoall.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Alltoall...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Alltoall( sendbuf, sendcount, sendtype, recvbuf, recvcnt, recvtype, comm );

  printf( "[%d] Ending MPI_Alltoall\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Alltoallv( sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm )
void * sendbuf;
int * sendcnts;
int * sdispls;
MPI_Datatype sendtype;
void * recvbuf;
int * recvcnts;
int * rdispls;
MPI_Datatype recvtype;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Alltoallv - prototyping replacement for MPI_Alltoallv
    Trace the beginning and ending of MPI_Alltoallv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Alltoallv...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Alltoallv( sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm );

  printf( "[%d] Ending MPI_Alltoallv\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Barrier( comm )
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Barrier - prototyping replacement for MPI_Barrier
    Trace the beginning and ending of MPI_Barrier.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Barrier...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Barrier( comm );

  printf( "[%d] Ending MPI_Barrier\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Bcast( buffer, count, datatype, root, comm )
void * buffer;
int count;
MPI_Datatype datatype;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Bcast - prototyping replacement for MPI_Bcast
    Trace the beginning and ending of MPI_Bcast.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Bcast...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Bcast( buffer, count, datatype, root, comm );

  printf( "[%d] Ending MPI_Bcast\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Gather( sendbuf, sendcnt, sendtype, recvbuf, recvcount, recvtype, root, comm )
void * sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void * recvbuf;
int recvcount;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Gather - prototyping replacement for MPI_Gather
    Trace the beginning and ending of MPI_Gather.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Gather...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Gather( sendbuf, sendcnt, sendtype, recvbuf, recvcount, recvtype, root, comm );

  printf( "[%d] Ending MPI_Gather\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Gatherv( sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm )
void * sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void * recvbuf;
int * recvcnts;
int * displs;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Gatherv - prototyping replacement for MPI_Gatherv
    Trace the beginning and ending of MPI_Gatherv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Gatherv...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Gatherv( sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm );

  printf( "[%d] Ending MPI_Gatherv\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Op_create( function, commute, op )
MPI_User_function * function;
int commute;
MPI_Op * op;
{
  int  returnVal;
  int llrank;

/*
    MPI_Op_create - prototyping replacement for MPI_Op_create
    Trace the beginning and ending of MPI_Op_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Op_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Op_create( function, commute, op );

  printf( "[%d] Ending MPI_Op_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Op_free( op )
MPI_Op * op;
{
  int  returnVal;
  int llrank;

/*
    MPI_Op_free - prototyping replacement for MPI_Op_free
    Trace the beginning and ending of MPI_Op_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Op_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Op_free( op );

  printf( "[%d] Ending MPI_Op_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Reduce_scatter( sendbuf, recvbuf, recvcnts, datatype, op, comm )
void * sendbuf;
void * recvbuf;
int * recvcnts;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Reduce_scatter - prototyping replacement for MPI_Reduce_scatter
    Trace the beginning and ending of MPI_Reduce_scatter.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Reduce_scatter...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Reduce_scatter( sendbuf, recvbuf, recvcnts, datatype, op, comm );

  printf( "[%d] Ending MPI_Reduce_scatter\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Reduce( sendbuf, recvbuf, count, datatype, op, root, comm )
void * sendbuf;
void * recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Reduce - prototyping replacement for MPI_Reduce
    Trace the beginning and ending of MPI_Reduce.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Reduce...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Reduce( sendbuf, recvbuf, count, datatype, op, root, comm );

  printf( "[%d] Ending MPI_Reduce\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Scan( sendbuf, recvbuf, count, datatype, op, comm )
void * sendbuf;
void * recvbuf;
int count;
MPI_Datatype datatype;
MPI_Op op;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Scan - prototyping replacement for MPI_Scan
    Trace the beginning and ending of MPI_Scan.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Scan...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Scan( sendbuf, recvbuf, count, datatype, op, comm );

  printf( "[%d] Ending MPI_Scan\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Scatter( sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm )
void * sendbuf;
int sendcnt;
MPI_Datatype sendtype;
void * recvbuf;
int recvcnt;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Scatter - prototyping replacement for MPI_Scatter
    Trace the beginning and ending of MPI_Scatter.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Scatter...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Scatter( sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm );

  printf( "[%d] Ending MPI_Scatter\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Scatterv( sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm )
void * sendbuf;
int * sendcnts;
int * displs;
MPI_Datatype sendtype;
void * recvbuf;
int recvcnt;
MPI_Datatype recvtype;
int root;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Scatterv - prototyping replacement for MPI_Scatterv
    Trace the beginning and ending of MPI_Scatterv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Scatterv...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Scatterv( sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm );

  printf( "[%d] Ending MPI_Scatterv\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Attr_delete( comm, keyval )
MPI_Comm comm;
int keyval;
{
  int   returnVal;
  int llrank;

/*
    MPI_Attr_delete - prototyping replacement for MPI_Attr_delete
    Trace the beginning and ending of MPI_Attr_delete.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Attr_delete...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Attr_delete( comm, keyval );

  printf( "[%d] Ending MPI_Attr_delete\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Attr_get( comm, keyval, attr_value, flag )
MPI_Comm comm;
int keyval;
void * attr_value;
int * flag;
{
  int   returnVal;
  int llrank;

/*
    MPI_Attr_get - prototyping replacement for MPI_Attr_get
    Trace the beginning and ending of MPI_Attr_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Attr_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Attr_get( comm, keyval, attr_value, flag );

  printf( "[%d] Ending MPI_Attr_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Attr_put( comm, keyval, attr_value )
MPI_Comm comm;
int keyval;
void * attr_value;
{
  int   returnVal;
  int llrank;

/*
    MPI_Attr_put - prototyping replacement for MPI_Attr_put
    Trace the beginning and ending of MPI_Attr_put.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Attr_put...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Attr_put( comm, keyval, attr_value );

  printf( "[%d] Ending MPI_Attr_put\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_compare( comm1, comm2, result )
MPI_Comm comm1;
MPI_Comm comm2;
int * result;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_compare - prototyping replacement for MPI_Comm_compare
    Trace the beginning and ending of MPI_Comm_compare.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_compare...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_compare( comm1, comm2, result );

  printf( "[%d] Ending MPI_Comm_compare\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_create( comm, group, comm_out )
MPI_Comm comm;
MPI_Group group;
MPI_Comm * comm_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_create - prototyping replacement for MPI_Comm_create
    Trace the beginning and ending of MPI_Comm_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_create( comm, group, comm_out );

  printf( "[%d] Ending MPI_Comm_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_dup( comm, comm_out )
MPI_Comm comm;
MPI_Comm * comm_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_dup - prototyping replacement for MPI_Comm_dup
    Trace the beginning and ending of MPI_Comm_dup.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_dup...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_dup( comm, comm_out );

  printf( "[%d] Ending MPI_Comm_dup\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_free( comm )
MPI_Comm * comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_free - prototyping replacement for MPI_Comm_free
    Trace the beginning and ending of MPI_Comm_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_free( comm );

  printf( "[%d] Ending MPI_Comm_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_group( comm, group )
MPI_Comm comm;
MPI_Group * group;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_group - prototyping replacement for MPI_Comm_group
    Trace the beginning and ending of MPI_Comm_group.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_group...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_group( comm, group );

  printf( "[%d] Ending MPI_Comm_group\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_rank( comm, rank )
MPI_Comm comm;
int * rank;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_rank - prototyping replacement for MPI_Comm_rank
    Trace the beginning and ending of MPI_Comm_rank.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_rank...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_rank( comm, rank );

  printf( "[%d] Ending MPI_Comm_rank\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_remote_group( comm, group )
MPI_Comm comm;
MPI_Group * group;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_remote_group - prototyping replacement for MPI_Comm_remote_group
    Trace the beginning and ending of MPI_Comm_remote_group.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_remote_group...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_remote_group( comm, group );

  printf( "[%d] Ending MPI_Comm_remote_group\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_remote_size( comm, size )
MPI_Comm comm;
int * size;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_remote_size - prototyping replacement for MPI_Comm_remote_size
    Trace the beginning and ending of MPI_Comm_remote_size.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_remote_size...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_remote_size( comm, size );

  printf( "[%d] Ending MPI_Comm_remote_size\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_size( comm, size )
MPI_Comm comm;
int * size;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_size - prototyping replacement for MPI_Comm_size
    Trace the beginning and ending of MPI_Comm_size.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_size...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_size( comm, size );

  printf( "[%d] Ending MPI_Comm_size\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_split( comm, color, key, comm_out )
MPI_Comm comm;
int color;
int key;
MPI_Comm * comm_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_split - prototyping replacement for MPI_Comm_split
    Trace the beginning and ending of MPI_Comm_split.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_split...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_split( comm, color, key, comm_out );

  printf( "[%d] Ending MPI_Comm_split\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Comm_test_inter( comm, flag )
MPI_Comm comm;
int * flag;
{
  int   returnVal;
  int llrank;

/*
    MPI_Comm_test_inter - prototyping replacement for MPI_Comm_test_inter
    Trace the beginning and ending of MPI_Comm_test_inter.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Comm_test_inter...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Comm_test_inter( comm, flag );

  printf( "[%d] Ending MPI_Comm_test_inter\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_compare( group1, group2, result )
MPI_Group group1;
MPI_Group group2;
int * result;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_compare - prototyping replacement for MPI_Group_compare
    Trace the beginning and ending of MPI_Group_compare.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_compare...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_compare( group1, group2, result );

  printf( "[%d] Ending MPI_Group_compare\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_difference( group1, group2, group_out )
MPI_Group group1;
MPI_Group group2;
MPI_Group * group_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_difference - prototyping replacement for MPI_Group_difference
    Trace the beginning and ending of MPI_Group_difference.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_difference...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_difference( group1, group2, group_out );

  printf( "[%d] Ending MPI_Group_difference\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_excl( group, n, ranks, newgroup )
MPI_Group group;
int n;
int * ranks;
MPI_Group * newgroup;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_excl - prototyping replacement for MPI_Group_excl
    Trace the beginning and ending of MPI_Group_excl.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_excl...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_excl( group, n, ranks, newgroup );

  printf( "[%d] Ending MPI_Group_excl\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_free( group )
MPI_Group * group;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_free - prototyping replacement for MPI_Group_free
    Trace the beginning and ending of MPI_Group_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_free( group );

  printf( "[%d] Ending MPI_Group_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_incl( group, n, ranks, group_out )
MPI_Group group;
int n;
int * ranks;
MPI_Group * group_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_incl - prototyping replacement for MPI_Group_incl
    Trace the beginning and ending of MPI_Group_incl.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_incl...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_incl( group, n, ranks, group_out );

  printf( "[%d] Ending MPI_Group_incl\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_intersection( group1, group2, group_out )
MPI_Group group1;
MPI_Group group2;
MPI_Group * group_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_intersection - prototyping replacement for MPI_Group_intersection
    Trace the beginning and ending of MPI_Group_intersection.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_intersection...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_intersection( group1, group2, group_out );

  printf( "[%d] Ending MPI_Group_intersection\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_rank( group, rank )
MPI_Group group;
int * rank;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_rank - prototyping replacement for MPI_Group_rank
    Trace the beginning and ending of MPI_Group_rank.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_rank...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_rank( group, rank );

  printf( "[%d] Ending MPI_Group_rank\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_range_excl( group, n, ranges, newgroup )
MPI_Group group;
int n;
int ranges[][3];
MPI_Group * newgroup;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_range_excl - prototyping replacement for MPI_Group_range_excl
    Trace the beginning and ending of MPI_Group_range_excl.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_range_excl...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_range_excl( group, n, ranges, newgroup );

  printf( "[%d] Ending MPI_Group_range_excl\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_range_incl( group, n, ranges, newgroup )
MPI_Group group;
int n;
int ranges[][3];
MPI_Group * newgroup;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_range_incl - prototyping replacement for MPI_Group_range_incl
    Trace the beginning and ending of MPI_Group_range_incl.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_range_incl...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_range_incl( group, n, ranges, newgroup );

  printf( "[%d] Ending MPI_Group_range_incl\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_size( group, size )
MPI_Group group;
int * size;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_size - prototyping replacement for MPI_Group_size
    Trace the beginning and ending of MPI_Group_size.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_size...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_size( group, size );

  printf( "[%d] Ending MPI_Group_size\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_translate_ranks( group_a, n, ranks_a, group_b, ranks_b )
MPI_Group group_a;
int n;
int * ranks_a;
MPI_Group group_b;
int * ranks_b;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_translate_ranks - prototyping replacement for MPI_Group_translate_ranks
    Trace the beginning and ending of MPI_Group_translate_ranks.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_translate_ranks...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_translate_ranks( group_a, n, ranks_a, group_b, ranks_b );

  printf( "[%d] Ending MPI_Group_translate_ranks\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Group_union( group1, group2, group_out )
MPI_Group group1;
MPI_Group group2;
MPI_Group * group_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Group_union - prototyping replacement for MPI_Group_union
    Trace the beginning and ending of MPI_Group_union.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Group_union...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Group_union( group1, group2, group_out );

  printf( "[%d] Ending MPI_Group_union\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Intercomm_create( local_comm, local_leader, peer_comm, remote_leader, tag, comm_out )
MPI_Comm local_comm;
int local_leader;
MPI_Comm peer_comm;
int remote_leader;
int tag;
MPI_Comm * comm_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Intercomm_create - prototyping replacement for MPI_Intercomm_create
    Trace the beginning and ending of MPI_Intercomm_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Intercomm_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Intercomm_create( local_comm, local_leader, peer_comm, remote_leader, tag, comm_out );

  printf( "[%d] Ending MPI_Intercomm_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Intercomm_merge( comm, high, comm_out )
MPI_Comm comm;
int high;
MPI_Comm * comm_out;
{
  int   returnVal;
  int llrank;

/*
    MPI_Intercomm_merge - prototyping replacement for MPI_Intercomm_merge
    Trace the beginning and ending of MPI_Intercomm_merge.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Intercomm_merge...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Intercomm_merge( comm, high, comm_out );

  printf( "[%d] Ending MPI_Intercomm_merge\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Keyval_create( copy_fn, delete_fn, keyval, extra_state )
MPI_Copy_function * copy_fn;
MPI_Delete_function * delete_fn;
int * keyval;
void * extra_state;
{
  int   returnVal;
  int llrank;

/*
    MPI_Keyval_create - prototyping replacement for MPI_Keyval_create
    Trace the beginning and ending of MPI_Keyval_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Keyval_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Keyval_create( copy_fn, delete_fn, keyval, extra_state );

  printf( "[%d] Ending MPI_Keyval_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Keyval_free( keyval )
int * keyval;
{
  int   returnVal;
  int llrank;

/*
    MPI_Keyval_free - prototyping replacement for MPI_Keyval_free
    Trace the beginning and ending of MPI_Keyval_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Keyval_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Keyval_free( keyval );

  printf( "[%d] Ending MPI_Keyval_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Abort( comm, errorcode )
MPI_Comm comm;
int errorcode;
{
  int  returnVal;
  int llrank;

/*
    MPI_Abort - prototyping replacement for MPI_Abort
    Trace the beginning and ending of MPI_Abort.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Abort...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Abort( comm, errorcode );

  printf( "[%d] Ending MPI_Abort\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Error_class( errorcode, errorclass )
int errorcode;
int * errorclass;
{
  int  returnVal;
  int llrank;

/*
    MPI_Error_class - prototyping replacement for MPI_Error_class
    Trace the beginning and ending of MPI_Error_class.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Error_class...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Error_class( errorcode, errorclass );

  printf( "[%d] Ending MPI_Error_class\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Errhandler_create( function, errhandler )
MPI_Handler_function * function;
MPI_Errhandler * errhandler;
{
  int  returnVal;
  int llrank;

/*
    MPI_Errhandler_create - prototyping replacement for MPI_Errhandler_create
    Trace the beginning and ending of MPI_Errhandler_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Errhandler_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Errhandler_create( function, errhandler );

  printf( "[%d] Ending MPI_Errhandler_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Errhandler_free( errhandler )
MPI_Errhandler * errhandler;
{
  int  returnVal;
  int llrank;

/*
    MPI_Errhandler_free - prototyping replacement for MPI_Errhandler_free
    Trace the beginning and ending of MPI_Errhandler_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Errhandler_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Errhandler_free( errhandler );

  printf( "[%d] Ending MPI_Errhandler_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Errhandler_get( comm, errhandler )
MPI_Comm comm;
MPI_Errhandler * errhandler;
{
  int  returnVal;
  int llrank;

/*
    MPI_Errhandler_get - prototyping replacement for MPI_Errhandler_get
    Trace the beginning and ending of MPI_Errhandler_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Errhandler_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Errhandler_get( comm, errhandler );

  printf( "[%d] Ending MPI_Errhandler_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Error_string( errorcode, string, resultlen )
int errorcode;
char * string;
int * resultlen;
{
  int  returnVal;
  int llrank;

/*
    MPI_Error_string - prototyping replacement for MPI_Error_string
    Trace the beginning and ending of MPI_Error_string.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Error_string...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Error_string( errorcode, string, resultlen );

  printf( "[%d] Ending MPI_Error_string\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Errhandler_set( comm, errhandler )
MPI_Comm comm;
MPI_Errhandler errhandler;
{
  int  returnVal;
  int llrank;

/*
    MPI_Errhandler_set - prototyping replacement for MPI_Errhandler_set
    Trace the beginning and ending of MPI_Errhandler_set.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Errhandler_set...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Errhandler_set( comm, errhandler );

  printf( "[%d] Ending MPI_Errhandler_set\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Finalize(  )
{
  int  returnVal;
  int llrank;

/*
    MPI_Finalize - prototyping replacement for MPI_Finalize
    Trace the beginning and ending of MPI_Finalize.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Finalize...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Finalize(  );

  printf( "[%d] Ending MPI_Finalize\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Get_processor_name( name, resultlen )
char * name;
int * resultlen;
{
  int  returnVal;
  int llrank;

/*
    MPI_Get_processor_name - prototyping replacement for MPI_Get_processor_name
    Trace the beginning and ending of MPI_Get_processor_name.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Get_processor_name...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Get_processor_name( name, resultlen );

  printf( "[%d] Ending MPI_Get_processor_name\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Init( argc, argv )
int * argc;
char *** argv;
{
  int  returnVal;
  int llrank;

/*
    MPI_Init - prototyping replacement for MPI_Init
    Trace the beginning and ending of MPI_Init.
*/

  
  printf( "Starting MPI_Init...\n" ); fflush( stdout );
  
  returnVal = PMPI_Init( argc, argv );

  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Ending MPI_Init\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Initialized( flag )
int * flag;
{
  int  returnVal;
  int llrank;

/*
    MPI_Initialized - prototyping replacement for MPI_Initialized
    Trace the beginning and ending of MPI_Initialized.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Initialized...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Initialized( flag );

  printf( "[%d] Ending MPI_Initialized\n", llrank ); fflush( stdout );


  return returnVal;
}

#ifdef FOO
/* Don't trace the timer calls */
double  MPI_Wtick(  )
{
  double  returnVal;
  int llrank;

/*
    MPI_Wtick - prototyping replacement for MPI_Wtick
    Trace the beginning and ending of MPI_Wtick.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Wtick...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Wtick(  );

  printf( "[%d] Ending MPI_Wtick\n", llrank ); fflush( stdout );


  return returnVal;
}

double  MPI_Wtime(  )
{
  double  returnVal;
  int llrank;

/*
    MPI_Wtime - prototyping replacement for MPI_Wtime
    Trace the beginning and ending of MPI_Wtime.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Wtime...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Wtime(  );

  printf( "[%d] Ending MPI_Wtime\n", llrank ); fflush( stdout );


  return returnVal;
}
#endif

int  MPI_Address( location, address )
void * location;
MPI_Aint * address;
{
  int  returnVal;
  int llrank;

/*
    MPI_Address - prototyping replacement for MPI_Address
    Trace the beginning and ending of MPI_Address.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Address...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Address( location, address );

  printf( "[%d] Ending MPI_Address\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Bsend( buf, count, datatype, dest, tag, comm )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int  returnVal;
  int llrank;

/*
    MPI_Bsend - prototyping replacement for MPI_Bsend
    Trace the beginning and ending of MPI_Bsend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Bsend with count = %d, dest = %d, tag = %d...\n",
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Bsend( buf, count, datatype, dest, tag, comm );

  printf( "[%d] Ending MPI_Bsend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Bsend_init( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Bsend_init - prototyping replacement for MPI_Bsend_init
    Trace the beginning and ending of MPI_Bsend_init.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Bsend_init with count = %d, dest = %d, tag = %d...\n", 
	 llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Bsend_init( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Bsend_init\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Buffer_attach( buffer, size )
void * buffer;
int size;
{
  int  returnVal;
  int llrank;

/*
    MPI_Buffer_attach - prototyping replacement for MPI_Buffer_attach
    Trace the beginning and ending of MPI_Buffer_attach.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Buffer_attach...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Buffer_attach( buffer, size );

  printf( "[%d] Ending MPI_Buffer_attach\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Buffer_detach( buffer, size )
void * buffer;
int * size;
{
  int  returnVal;
  int llrank;

/*
    MPI_Buffer_detach - prototyping replacement for MPI_Buffer_detach
    Trace the beginning and ending of MPI_Buffer_detach.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Buffer_detach...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Buffer_detach( buffer, size );

  printf( "[%d] Ending MPI_Buffer_detach\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Cancel( request )
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Cancel - prototyping replacement for MPI_Cancel
    Trace the beginning and ending of MPI_Cancel.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cancel...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cancel( request );

  printf( "[%d] Ending MPI_Cancel\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Request_free( request )
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Request_free - prototyping replacement for MPI_Request_free
    Trace the beginning and ending of MPI_Request_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Request_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Request_free( request );

  printf( "[%d] Ending MPI_Request_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Recv_init( buf, count, datatype, source, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Recv_init - prototyping replacement for MPI_Recv_init
    Trace the beginning and ending of MPI_Recv_init.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Recv_init with count = %d, source = %d, tag = %d ...\n", 
	  llrank, count, source, tag ); fflush( stdout );
  
  returnVal = PMPI_Recv_init( buf, count, datatype, source, tag, comm, request );

  printf( "[%d] Ending MPI_Recv_init\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Send_init( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Send_init - prototyping replacement for MPI_Send_init
    Trace the beginning and ending of MPI_Send_init.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Send_init, with count = %d, dest = %d, tag = %d...\n",
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Send_init( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Send_init\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Get_elements( status, datatype, elements )
MPI_Status * status;
MPI_Datatype datatype;
int * elements;
{
  int   returnVal;
  int llrank;

/*
    MPI_Get_elements - prototyping replacement for MPI_Get_elements
    Trace the beginning and ending of MPI_Get_elements.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Get_elements...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Get_elements( status, datatype, elements );

  printf( "[%d] Ending MPI_Get_elements\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Get_count( status, datatype, count )
MPI_Status * status;
MPI_Datatype datatype;
int * count;
{
  int  returnVal;
  int llrank;

/*
    MPI_Get_count - prototyping replacement for MPI_Get_count
    Trace the beginning and ending of MPI_Get_count.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Get_count...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Get_count( status, datatype, count );

  printf( "[%d] Ending MPI_Get_count\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Ibsend( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Ibsend - prototyping replacement for MPI_Ibsend
    Trace the beginning and ending of MPI_Ibsend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Ibsend with count = %d, dest = %d, tag = %d ...\n",
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Ibsend( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Ibsend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Iprobe( source, tag, comm, flag, status )
int source;
int tag;
MPI_Comm comm;
int * flag;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Iprobe - prototyping replacement for MPI_Iprobe
    Trace the beginning and ending of MPI_Iprobe.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Iprobe...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Iprobe( source, tag, comm, flag, status );

  printf( "[%d] Ending MPI_Iprobe\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Irecv( buf, count, datatype, source, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Irecv - prototyping replacement for MPI_Irecv
    Trace the beginning and ending of MPI_Irecv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Irecv with count = %d, source = %d, tag = %d...\n",
	  llrank, count, source, tag ); fflush( stdout );
  
  returnVal = PMPI_Irecv( buf, count, datatype, source, tag, comm, request );

  printf( "[%d] Ending MPI_Irecv\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Irsend( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Irsend - prototyping replacement for MPI_Irsend
    Trace the beginning and ending of MPI_Irsend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Irsend with count = %d, dest = %d, tag = %d...\n",
	 llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Irsend( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Irsend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Isend( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Isend - prototyping replacement for MPI_Isend
    Trace the beginning and ending of MPI_Isend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Isend with count = %d, dest = %d, tag = %d...\n",
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Isend( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Isend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Issend( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Issend - prototyping replacement for MPI_Issend
    Trace the beginning and ending of MPI_Issend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Issend with count = %d, dest = %d, tag = %d ...\n",
	 llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Issend( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Issend\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Pack( inbuf, incount, type, outbuf, outcount, position, comm )
void * inbuf;
int incount;
MPI_Datatype type;
void * outbuf;
int outcount;
int * position;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Pack - prototyping replacement for MPI_Pack
    Trace the beginning and ending of MPI_Pack.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Pack...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Pack( inbuf, incount, type, outbuf, outcount, position, comm );

  printf( "[%d] Ending MPI_Pack\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Pack_size( incount, datatype, comm, size )
int incount;
MPI_Datatype datatype;
MPI_Comm comm;
int * size;
{
  int   returnVal;
  int llrank;

/*
    MPI_Pack_size - prototyping replacement for MPI_Pack_size
    Trace the beginning and ending of MPI_Pack_size.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Pack_size...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Pack_size( incount, datatype, comm, size );

  printf( "[%d] Ending MPI_Pack_size\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Probe( source, tag, comm, status )
int source;
int tag;
MPI_Comm comm;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Probe - prototyping replacement for MPI_Probe
    Trace the beginning and ending of MPI_Probe.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Probe...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Probe( source, tag, comm, status );

  printf( "[%d] Ending MPI_Probe\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Recv( buf, count, datatype, source, tag, comm, status )
void * buf;
int count;
MPI_Datatype datatype;
int source;
int tag;
MPI_Comm comm;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Recv - prototyping replacement for MPI_Recv
    Trace the beginning and ending of MPI_Recv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Recv with count = %d, source = %d, tag = %d...\n",
	 llrank, count, source, tag ); fflush( stdout );
  
  returnVal = PMPI_Recv( buf, count, datatype, source, tag, comm, status );

  printf( "[%d] Ending MPI_Recv from %d with tag %d\n", 
	 llrank, status->MPI_SOURCE, status->MPI_TAG ); 
  fflush( stdout );


  return returnVal;
}

int  MPI_Rsend( buf, count, datatype, dest, tag, comm )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int  returnVal;
  int llrank;

/*
    MPI_Rsend - prototyping replacement for MPI_Rsend
    Trace the beginning and ending of MPI_Rsend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Rsend with count = %d, dest = %d, tag = %d...\n",
	 llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Rsend( buf, count, datatype, dest, tag, comm );

  printf( "[%d] Ending MPI_Rsend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Rsend_init( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Rsend_init - prototyping replacement for MPI_Rsend_init
    Trace the beginning and ending of MPI_Rsend_init.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Rsend_init with count = %d, dest = %d, tag = %d...\n", 
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Rsend_init( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Rsend_init\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Send( buf, count, datatype, dest, tag, comm )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int  returnVal;
  int llrank;

/*
    MPI_Send - prototyping replacement for MPI_Send
    Trace the beginning and ending of MPI_Send.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Send with count = %d, dest = %d, tag = %d...\n", 
	   llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Send( buf, count, datatype, dest, tag, comm );

  printf( "[%d] Ending MPI_Send\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Sendrecv( sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status )
void * sendbuf;
int sendcount;
MPI_Datatype sendtype;
int dest;
int sendtag;
void * recvbuf;
int recvcount;
MPI_Datatype recvtype;
int source;
int recvtag;
MPI_Comm comm;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Sendrecv - prototyping replacement for MPI_Sendrecv
    Trace the beginning and ending of MPI_Sendrecv.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Sendrecv with sendtag %d, recvtag %d, dest %d, source %d ...\n", llrank, sendtag, recvtag, dest, source ); fflush( stdout );
  
  returnVal = PMPI_Sendrecv( sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status );

  printf( "[%d] Ending MPI_Sendrecv\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Sendrecv_replace( buf, count, datatype, dest, sendtag, source, recvtag, comm, status )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int sendtag;
int source;
int recvtag;
MPI_Comm comm;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Sendrecv_replace - prototyping replacement for MPI_Sendrecv_replace
    Trace the beginning and ending of MPI_Sendrecv_replace.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Sendrecv_replace with sendtag %d, recvtag %d, dest %d, source %d ...\n", llrank, sendtag, recvtag, dest, source ); fflush( stdout );
  
  returnVal = PMPI_Sendrecv_replace( buf, count, datatype, dest, sendtag, source, recvtag, comm, status );

  printf( "[%d] Ending MPI_Sendrecv_replace\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Ssend( buf, count, datatype, dest, tag, comm )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
{
  int  returnVal;
  int llrank;

/*
    MPI_Ssend - prototyping replacement for MPI_Ssend
    Trace the beginning and ending of MPI_Ssend.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Ssend with count = %d, dest = %d, tag = %d...\n",
	 llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Ssend( buf, count, datatype, dest, tag, comm );

  printf( "[%d] Ending MPI_Ssend\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Ssend_init( buf, count, datatype, dest, tag, comm, request )
void * buf;
int count;
MPI_Datatype datatype;
int dest;
int tag;
MPI_Comm comm;
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Ssend_init - prototyping replacement for MPI_Ssend_init
    Trace the beginning and ending of MPI_Ssend_init.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Ssend_init with count = %d, dest = %d, tag = %d ...\n", 
	  llrank, count, dest, tag ); fflush( stdout );
  
  returnVal = PMPI_Ssend_init( buf, count, datatype, dest, tag, comm, request );

  printf( "[%d] Ending MPI_Ssend_init\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Start( request )
MPI_Request * request;
{
  int  returnVal;
  int llrank;

/*
    MPI_Start - prototyping replacement for MPI_Start
    Trace the beginning and ending of MPI_Start.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Start...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Start( request );

  printf( "[%d] Ending MPI_Start\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Startall( count, array_of_requests )
int count;
MPI_Request * array_of_requests;
{
  int  returnVal;
  int llrank;

/*
    MPI_Startall - prototyping replacement for MPI_Startall
    Trace the beginning and ending of MPI_Startall.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Startall...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Startall( count, array_of_requests );

  printf( "[%d] Ending MPI_Startall\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Test( request, flag, status )
MPI_Request * request;
int * flag;
MPI_Status * status;
{
  int   returnVal;
  int llrank;

/*
    MPI_Test - prototyping replacement for MPI_Test
    Trace the beginning and ending of MPI_Test.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Test...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Test( request, flag, status );

  printf( "[%d] Ending MPI_Test\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Testall( count, array_of_requests, flag, array_of_statuses )
int count;
MPI_Request * array_of_requests;
int * flag;
MPI_Status * array_of_statuses;
{
  int  returnVal;
  int llrank;

/*
    MPI_Testall - prototyping replacement for MPI_Testall
    Trace the beginning and ending of MPI_Testall.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Testall...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Testall( count, array_of_requests, flag, array_of_statuses );

  printf( "[%d] Ending MPI_Testall\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Testany( count, array_of_requests, index, flag, status )
int count;
MPI_Request * array_of_requests;
int * index;
int * flag;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Testany - prototyping replacement for MPI_Testany
    Trace the beginning and ending of MPI_Testany.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Testany...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Testany( count, array_of_requests, index, flag, status );

  printf( "[%d] Ending MPI_Testany\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Test_cancelled( status, flag )
MPI_Status * status;
int * flag;
{
  int  returnVal;
  int llrank;

/*
    MPI_Test_cancelled - prototyping replacement for MPI_Test_cancelled
    Trace the beginning and ending of MPI_Test_cancelled.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Test_cancelled...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Test_cancelled( status, flag );

  printf( "[%d] Ending MPI_Test_cancelled\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Testsome( incount, array_of_requests, outcount, array_of_indices, array_of_statuses )
int incount;
MPI_Request * array_of_requests;
int * outcount;
int * array_of_indices;
MPI_Status * array_of_statuses;
{
  int  returnVal;
  int llrank;

/*
    MPI_Testsome - prototyping replacement for MPI_Testsome
    Trace the beginning and ending of MPI_Testsome.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Testsome...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Testsome( incount, array_of_requests, outcount, array_of_indices, array_of_statuses );

  printf( "[%d] Ending MPI_Testsome\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Type_commit( datatype )
MPI_Datatype * datatype;
{
  int   returnVal;
  int llrank;

/*
    MPI_Type_commit - prototyping replacement for MPI_Type_commit
    Trace the beginning and ending of MPI_Type_commit.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_commit...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_commit( datatype );

  printf( "[%d] Ending MPI_Type_commit\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_contiguous( count, old_type, newtype )
int count;
MPI_Datatype old_type;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_contiguous - prototyping replacement for MPI_Type_contiguous
    Trace the beginning and ending of MPI_Type_contiguous.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_contiguous...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_contiguous( count, old_type, newtype );

  printf( "[%d] Ending MPI_Type_contiguous\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_extent( datatype, extent )
MPI_Datatype datatype;
MPI_Aint * extent;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_extent - prototyping replacement for MPI_Type_extent
    Trace the beginning and ending of MPI_Type_extent.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_extent...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_extent( datatype, extent );

  printf( "[%d] Ending MPI_Type_extent\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Type_free( datatype )
MPI_Datatype * datatype;
{
  int   returnVal;
  int llrank;

/*
    MPI_Type_free - prototyping replacement for MPI_Type_free
    Trace the beginning and ending of MPI_Type_free.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_free...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_free( datatype );

  printf( "[%d] Ending MPI_Type_free\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_hindexed( count, blocklens, indices, old_type, newtype )
int count;
int * blocklens;
MPI_Aint * indices;
MPI_Datatype old_type;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_hindexed - prototyping replacement for MPI_Type_hindexed
    Trace the beginning and ending of MPI_Type_hindexed.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_hindexed...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_hindexed( count, blocklens, indices, old_type, newtype );

  printf( "[%d] Ending MPI_Type_hindexed\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_hvector( count, blocklen, stride, old_type, newtype )
int count;
int blocklen;
MPI_Aint stride;
MPI_Datatype old_type;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_hvector - prototyping replacement for MPI_Type_hvector
    Trace the beginning and ending of MPI_Type_hvector.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_hvector...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_hvector( count, blocklen, stride, old_type, newtype );

  printf( "[%d] Ending MPI_Type_hvector\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_indexed( count, blocklens, indices, old_type, newtype )
int count;
int * blocklens;
int * indices;
MPI_Datatype old_type;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_indexed - prototyping replacement for MPI_Type_indexed
    Trace the beginning and ending of MPI_Type_indexed.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_indexed...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_indexed( count, blocklens, indices, old_type, newtype );

  printf( "[%d] Ending MPI_Type_indexed\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Type_lb( datatype, displacement )
MPI_Datatype datatype;
MPI_Aint * displacement;
{
  int   returnVal;
  int llrank;

/*
    MPI_Type_lb - prototyping replacement for MPI_Type_lb
    Trace the beginning and ending of MPI_Type_lb.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_lb...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_lb( datatype, displacement );

  printf( "[%d] Ending MPI_Type_lb\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Type_size( datatype, size )
MPI_Datatype datatype;
int          * size;
{
  int   returnVal;
  int llrank;

/*
    MPI_Type_size - prototyping replacement for MPI_Type_size
    Trace the beginning and ending of MPI_Type_size.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_size...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_size( datatype, size );

  printf( "[%d] Ending MPI_Type_size\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_struct( count, blocklens, indices, old_types, newtype )
int count;
int * blocklens;
MPI_Aint * indices;
MPI_Datatype * old_types;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_struct - prototyping replacement for MPI_Type_struct
    Trace the beginning and ending of MPI_Type_struct.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_struct...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_struct( count, blocklens, indices, old_types, newtype );

  printf( "[%d] Ending MPI_Type_struct\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Type_ub( datatype, displacement )
MPI_Datatype datatype;
MPI_Aint * displacement;
{
  int   returnVal;
  int llrank;

/*
    MPI_Type_ub - prototyping replacement for MPI_Type_ub
    Trace the beginning and ending of MPI_Type_ub.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_ub...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_ub( datatype, displacement );

  printf( "[%d] Ending MPI_Type_ub\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Type_vector( count, blocklen, stride, old_type, newtype )
int count;
int blocklen;
int stride;
MPI_Datatype old_type;
MPI_Datatype * newtype;
{
  int  returnVal;
  int llrank;

/*
    MPI_Type_vector - prototyping replacement for MPI_Type_vector
    Trace the beginning and ending of MPI_Type_vector.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Type_vector...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Type_vector( count, blocklen, stride, old_type, newtype );

  printf( "[%d] Ending MPI_Type_vector\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Unpack( inbuf, insize, position, outbuf, outcount, type, comm )
void * inbuf;
int insize;
int * position;
void * outbuf;
int outcount;
MPI_Datatype type;
MPI_Comm comm;
{
  int   returnVal;
  int llrank;

/*
    MPI_Unpack - prototyping replacement for MPI_Unpack
    Trace the beginning and ending of MPI_Unpack.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Unpack...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Unpack( inbuf, insize, position, outbuf, outcount, type, comm );

  printf( "[%d] Ending MPI_Unpack\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Wait( request, status )
MPI_Request * request;
MPI_Status * status;
{
  int   returnVal;
  int llrank;

/*
    MPI_Wait - prototyping replacement for MPI_Wait
    Trace the beginning and ending of MPI_Wait.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Wait...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Wait( request, status );

  printf( "[%d] Ending MPI_Wait\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Waitall( count, array_of_requests, array_of_statuses )
int count;
MPI_Request * array_of_requests;
MPI_Status * array_of_statuses;
{
  int  returnVal;
  int llrank;

/*
    MPI_Waitall - prototyping replacement for MPI_Waitall
    Trace the beginning and ending of MPI_Waitall.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Waitall...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Waitall( count, array_of_requests, array_of_statuses );

  printf( "[%d] Ending MPI_Waitall\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Waitany( count, array_of_requests, index, status )
int count;
MPI_Request * array_of_requests;
int * index;
MPI_Status * status;
{
  int  returnVal;
  int llrank;

/*
    MPI_Waitany - prototyping replacement for MPI_Waitany
    Trace the beginning and ending of MPI_Waitany.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Waitany...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Waitany( count, array_of_requests, index, status );

  printf( "[%d] Ending MPI_Waitany\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Waitsome( incount, array_of_requests, outcount, array_of_indices, array_of_statuses )
int incount;
MPI_Request * array_of_requests;
int * outcount;
int * array_of_indices;
MPI_Status * array_of_statuses;
{
  int  returnVal;
  int llrank;

/*
    MPI_Waitsome - prototyping replacement for MPI_Waitsome
    Trace the beginning and ending of MPI_Waitsome.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Waitsome...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Waitsome( incount, array_of_requests, outcount, array_of_indices, array_of_statuses );

  printf( "[%d] Ending MPI_Waitsome\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_coords( comm, rank, maxdims, coords )
MPI_Comm comm;
int rank;
int maxdims;
int * coords;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_coords - prototyping replacement for MPI_Cart_coords
    Trace the beginning and ending of MPI_Cart_coords.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_coords...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_coords( comm, rank, maxdims, coords );

  printf( "[%d] Ending MPI_Cart_coords\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_create( comm_old, ndims, dims, periods, reorder, comm_cart )
MPI_Comm comm_old;
int ndims;
int * dims;
int * periods;
int reorder;
MPI_Comm * comm_cart;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_create - prototyping replacement for MPI_Cart_create
    Trace the beginning and ending of MPI_Cart_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_create( comm_old, ndims, dims, periods, reorder, comm_cart );

  printf( "[%d] Ending MPI_Cart_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_get( comm, maxdims, dims, periods, coords )
MPI_Comm comm;
int maxdims;
int * dims;
int * periods;
int * coords;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_get - prototyping replacement for MPI_Cart_get
    Trace the beginning and ending of MPI_Cart_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_get( comm, maxdims, dims, periods, coords );

  printf( "[%d] Ending MPI_Cart_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_map( comm_old, ndims, dims, periods, newrank )
MPI_Comm comm_old;
int ndims;
int * dims;
int * periods;
int * newrank;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_map - prototyping replacement for MPI_Cart_map
    Trace the beginning and ending of MPI_Cart_map.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_map...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_map( comm_old, ndims, dims, periods, newrank );

  printf( "[%d] Ending MPI_Cart_map\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_rank( comm, coords, rank )
MPI_Comm comm;
int * coords;
int * rank;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_rank - prototyping replacement for MPI_Cart_rank
    Trace the beginning and ending of MPI_Cart_rank.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_rank...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_rank( comm, coords, rank );

  printf( "[%d] Ending MPI_Cart_rank\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_shift( comm, direction, displ, source, dest )
MPI_Comm comm;
int direction;
int displ;
int * source;
int * dest;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_shift - prototyping replacement for MPI_Cart_shift
    Trace the beginning and ending of MPI_Cart_shift.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_shift...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_shift( comm, direction, displ, source, dest );

  printf( "[%d] Ending MPI_Cart_shift\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cart_sub( comm, remain_dims, comm_new )
MPI_Comm comm;
int * remain_dims;
MPI_Comm * comm_new;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cart_sub - prototyping replacement for MPI_Cart_sub
    Trace the beginning and ending of MPI_Cart_sub.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cart_sub...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cart_sub( comm, remain_dims, comm_new );

  printf( "[%d] Ending MPI_Cart_sub\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Cartdim_get( comm, ndims )
MPI_Comm comm;
int * ndims;
{
  int   returnVal;
  int llrank;

/*
    MPI_Cartdim_get - prototyping replacement for MPI_Cartdim_get
    Trace the beginning and ending of MPI_Cartdim_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Cartdim_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Cartdim_get( comm, ndims );

  printf( "[%d] Ending MPI_Cartdim_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int  MPI_Dims_create( nnodes, ndims, dims )
int nnodes;
int ndims;
int * dims;
{
  int  returnVal;
  int llrank;

/*
    MPI_Dims_create - prototyping replacement for MPI_Dims_create
    Trace the beginning and ending of MPI_Dims_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Dims_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Dims_create( nnodes, ndims, dims );

  printf( "[%d] Ending MPI_Dims_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graph_create( comm_old, nnodes, index, edges, reorder, comm_graph )
MPI_Comm comm_old;
int nnodes;
int * index;
int * edges;
int reorder;
MPI_Comm * comm_graph;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graph_create - prototyping replacement for MPI_Graph_create
    Trace the beginning and ending of MPI_Graph_create.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graph_create...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graph_create( comm_old, nnodes, index, edges, reorder, comm_graph );

  printf( "[%d] Ending MPI_Graph_create\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graph_get( comm, maxindex, maxedges, index, edges )
MPI_Comm comm;
int maxindex;
int maxedges;
int * index;
int * edges;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graph_get - prototyping replacement for MPI_Graph_get
    Trace the beginning and ending of MPI_Graph_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graph_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graph_get( comm, maxindex, maxedges, index, edges );

  printf( "[%d] Ending MPI_Graph_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graph_map( comm_old, nnodes, index, edges, newrank )
MPI_Comm comm_old;
int nnodes;
int * index;
int * edges;
int * newrank;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graph_map - prototyping replacement for MPI_Graph_map
    Trace the beginning and ending of MPI_Graph_map.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graph_map...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graph_map( comm_old, nnodes, index, edges, newrank );

  printf( "[%d] Ending MPI_Graph_map\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graph_neighbors( comm, rank, maxneighbors, neighbors )
MPI_Comm comm;
int rank;
int maxneighbors;
int * neighbors;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graph_neighbors - prototyping replacement for MPI_Graph_neighbors
    Trace the beginning and ending of MPI_Graph_neighbors.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graph_neighbors...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graph_neighbors( comm, rank, maxneighbors, neighbors );

  printf( "[%d] Ending MPI_Graph_neighbors\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graph_neighbors_count( comm, rank, nneighbors )
MPI_Comm comm;
int rank;
int * nneighbors;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graph_neighbors_count - prototyping replacement for MPI_Graph_neighbors_count
    Trace the beginning and ending of MPI_Graph_neighbors_count.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graph_neighbors_count...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graph_neighbors_count( comm, rank, nneighbors );

  printf( "[%d] Ending MPI_Graph_neighbors_count\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Graphdims_get( comm, nnodes, nedges )
MPI_Comm comm;
int * nnodes;
int * nedges;
{
  int   returnVal;
  int llrank;

/*
    MPI_Graphdims_get - prototyping replacement for MPI_Graphdims_get
    Trace the beginning and ending of MPI_Graphdims_get.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Graphdims_get...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Graphdims_get( comm, nnodes, nedges );

  printf( "[%d] Ending MPI_Graphdims_get\n", llrank ); fflush( stdout );


  return returnVal;
}

int   MPI_Topo_test( comm, top_type )
MPI_Comm comm;
int * top_type;
{
  int   returnVal;
  int llrank;

/*
    MPI_Topo_test - prototyping replacement for MPI_Topo_test
    Trace the beginning and ending of MPI_Topo_test.
*/

  
  PMPI_Comm_rank( MPI_COMM_WORLD, &llrank );
  printf( "[%d] Starting MPI_Topo_test...\n", llrank ); fflush( stdout );
  
  returnVal = PMPI_Topo_test( comm, top_type );

  printf( "[%d] Ending MPI_Topo_test\n", llrank ); fflush( stdout );


  return returnVal;
}
