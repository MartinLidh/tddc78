/* myprof.c */
/* Custom Fortran interface file */
/* These have been edited because they require special string processing */
/* See mpe_prof.c for what these are interfacing to */

/* 
 * If not building for MPICH, then MPIR_ERROR and the mpi_iargc_/mpir_getarg_
 * calls need to be replaced.
 */

#ifdef MPI_BUILD_PROFILING
#undef MPI_BUILD_PROFILING
#endif
#include "mpeconf.h"
#include "mpi.h"
#include "mpe.h"

#ifndef MPICH_NAME
/* If we aren't running MPICH, just use fprintf for errors */
#include <stdio.h>
#define MPIR_ERROR(comm,errcode,str) (fprintf( stderr, "%s\n", str ),errcode)
/* Also avoid Fortran arguments */
#define mpir_iargc_() 0
#define mpir_getarg_( idx, str, ln ) strncpy(str,"Unknown",ln)
#else
/* Make sure that we get the correct Fortran form */

#ifdef FORTRANCAPS
#define mpir_iargc_ MPIR_IARGC
#define mpir_getarg_ MPIR_GETARG
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpir_iargc_ mpir_iargc__
#define mpir_getarg_ mpir_getarg__
#elif !defined(FORTRANUNDERSCORE)
#define mpir_iargc_ mpir_iargc
#define mpir_getarg_ mpir_getarg
#endif

#endif


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

/* 
   Include a definition of MALLOC and FREE to allow the use of MPICH
   memory debug code 
*/
#if defined(MPIR_MEMDEBUG)
/* Enable memory tracing.  This requires MPICH's mpid/util/tr2.c codes */
#define MALLOC(a)    MPID_trmalloc((unsigned)(a),__LINE__,__FILE__)
#define FREE(a)      MPID_trfree(a,__LINE__,__FILE__)

#else
#define MALLOC(a) malloc(a)
#define FREE(a)   free(a)
#define MPID_trvalid(a)
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif

#ifndef DEBUG_ALL
#define DEBUG_ALL
#endif

#ifdef FORTRANCAPS
#define mpi_init_ MPI_INIT
#define mpi_bsend_init_ MPI_BSEND_INIT
#define mpi_bsend_ MPI_BSEND
#define mpi_buffer_attach_ MPI_BUFFER_ATTACH
#define mpi_buffer_detach_ MPI_BUFFER_DETACH
#define mpi_cancel_ MPI_CANCEL
#define mpi_request_free_ MPI_REQUEST_FREE
#define mpi_recv_init_ MPI_RECV_INIT
#define mpi_send_init_ MPI_SEND_INIT
#define mpi_get_count_ MPI_GET_COUNT
#define mpi_get_elements_ MPI_GET_ELEMENTS
#define mpi_ibsend_ MPI_IBSEND
#define mpi_iprobe_ MPI_IPROBE
#define mpi_irecv_ MPI_IRECV
#define mpi_irsend_ MPI_IRSEND
#define mpi_isend_ MPI_ISEND
#define mpi_issend_ MPI_ISSEND
#define mpi_pack_size_ MPI_PACK_SIZE
#define mpi_pack_ MPI_PACK
#define mpi_probe_ MPI_PROBE
#define mpi_recv_ MPI_RECV
#define mpi_rsend_init_ MPI_RSEND_INIT
#define mpi_rsend_ MPI_RSEND
#define mpi_send_ MPI_SEND
#define mpi_sendrecv_ MPI_SENDRECV
#define mpi_sendrecv_replace_ MPI_SENDRECV_REPLACE
#define mpi_ssend_init_ MPI_SSEND_INIT
#define mpi_ssend_ MPI_SSEND
#define mpi_startall_ MPI_STARTALL
#define mpi_start_ MPI_START
#define mpi_testall_ MPI_TESTALL
#define mpi_testany_ MPI_TESTANY
#define mpi_test_canceled_ MPI_TESTCANCEL
#define mpi_test_ MPI_TEST
#define mpi_testsome_ MPI_TESTSOME
#define mpi_type_commit_ MPI_TYPE_COMMIT
#define mpi_type_contiguous_ MPI_TYPE_CONTIGUOUS
#define mpi_type_extent_ MPI_TYPE_EXTENT
#define mpi_type_free_ MPI_TYPE_FREE
#define mpi_type_hindexed_ MPI_TYPE_HINDEXED
#define mpi_type_hvector_ MPI_TYPE_HVECTOR
#define mpi_type_indexed_ MPI_TYPE_INDEXED
#define mpi_type_lb_ MPI_TYPE_LB
#define mpi_type_size_ MPI_TYPE_SIZE
#define mpi_type_struct_ MPI_TYPE_STRUCT
#define mpi_type_ub_ MPI_TYPE_UB
#define mpi_type_vector_ MPI_TYPE_VECTOR
#define mpi_unpack_ MPI_UNPACK
#define mpi_waitall_ MPI_WAITALL
#define mpi_waitany_ MPI_WAITANY
#define mpi_wait_ MPI_WAIT
#define mpi_waitsome_ MPI_WAITSOME
#define mpi_allgather_ MPI_ALLGATHER
#define mpi_allgatherv_ MPI_ALLGATHERV
#define mpi_allreduce_ MPI_ALLREDUCE
#define mpi_alltoall_ MPI_ALLTOALL
#define mpi_alltoallv_ MPI_ALLTOALLV
#define mpi_barrier_ MPI_BARRIER
#define mpi_bcast_ MPI_BCAST
#define mpi_gather_ MPI_GATHER
#define mpi_gatherv_ MPI_GATHERV
#define mpi_op_create_ MPI_OP_CREATE
#define mpi_op_free_ MPI_OP_FREE
#define mpi_reduce_scatter_ MPI_REDUCE_SCATTER
#define mpi_reduce_ MPI_REDUCE
#define mpi_scan_ MPI_SCAN
#define mpi_scatter_ MPI_SCATTER
#define mpi_scatterv_ MPI_SCATTERV
#define mpi_finalize_ MPI_FINALIZE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpi_init_ mpi_init__
#define mpi_bsend_init_ mpi_bsend_init__
#define mpi_bsend_ mpi_bsend__
#define mpi_buffer_attach_ mpi_buffer_attach__
#define mpi_buffer_detach_ mpi_buffer_detach__
#define mpi_cancel_ mpi_cancel__
#define mpi_request_free_ mpi_request_free__
#define mpi_recv_init_ mpi_recv_init__
#define mpi_send_init_ mpi_send_init__
#define mpi_get_count_ mpi_get_count__
#define mpi_get_elements_ mpi_get_elements__
#define mpi_ibsend_ mpi_ibsend__
#define mpi_iprobe_ mpi_iprobe__
#define mpi_irecv_ mpi_irecv__
#define mpi_irsend_ mpi_irsend__
#define mpi_isend_ mpi_isend__
#define mpi_issend_ mpi_issend__
#define mpi_pack_size_ mpi_pack_size__
#define mpi_pack_ mpi_pack__
#define mpi_probe_ mpi_probe__
#define mpi_recv_ mpi_recv__
#define mpi_rsend_init_ mpi_rsend_init__
#define mpi_rsend_ mpi_rsend__
#define mpi_send_ mpi_send__
#define mpi_sendrecv_ mpi_sendrecv__
#define mpi_sendrecv_replace_ mpi_sendrecv_replace__
#define mpi_ssend_init_ mpi_ssend_init__
#define mpi_ssend_ mpi_ssend__
#define mpi_startall_ mpi_startall__
#define mpi_start_ mpi_start__
#define mpi_testall_ mpi_testall__
#define mpi_testany_ mpi_testany__
#define mpi_test_cancelled_ mpi_test_cancelled__
#define mpi_test_ mpi_test__
#define mpi_testsome_ mpi_testsome__
#define mpi_type_commit_ mpi_type_commit__
#define mpi_type_contiguous_ mpi_type_contiguous__
#define mpi_type_extent_ mpi_type_extent__
#define mpi_type_free_ mpi_type_free__
#define mpi_type_hindexed_ mpi_type_hindexed__
#define mpi_type_hvector_ mpi_type_hvector__
#define mpi_type_indexed_ mpi_type_indexed__
#define mpi_type_lb_ mpi_type_lb__
#define mpi_type_size_ mpi_type_size__
#define mpi_type_struct_ mpi_type_struct__
#define mpi_type_ub_ mpi_type_ub__
#define mpi_type_vector_ mpi_type_vector__
#define mpi_unpack_ mpi_unpack__
#define mpi_waitall_ mpi_waitall__
#define mpi_waitany_ mpi_waitany__
#define mpi_wait_ mpi_wait__
#define mpi_waitsome_ mpi_waitsome__
#define mpi_allgather_ mpi_allgather__
#define mpi_allgatherv_ mpi_allgatherv__
#define mpi_allreduce_ mpi_allreduce__
#define mpi_alltoall_ mpi_alltoall__
#define mpi_alltoallv_ mpi_alltoallv__
#define mpi_barrier_ mpi_barrier__
#define mpi_bcast_ mpi_bcast__
#define mpi_gather_ mpi_gather__
#define mpi_gatherv_ mpi_gatherv__
#define mpi_op_create_ mpi_op_create__
#define mpi_op_free_ mpi_op_free__
#define mpi_reduce_scatter_ mpi_reduce_scatter__
#define mpi_reduce_ mpi_reduce__
#define mpi_scan_ mpi_scan__
#define mpi_scatter_ mpi_scatter__
#define mpi_scatterv_ mpi_scatterv__
#define mpi_finalize_ mpi_finalize__
#elif defined(FORTRANNOUNDERSCORE)
#define mpi_init_ mpi_init
#define mpi_bsend_ mpi_bsend
#define mpi_bsend_init_ mpi_bsend_init
#define mpi_buffer_attach_ mpi_buffer_attach
#define mpi_buffer_detach_ mpi_buffer_detach
#define mpi_cancel_ mpi_cancel
#define mpi_request_free_ mpi_request_free
#define mpi_recv_init_ mpi_recv_init
#define mpi_send_init_ mpi_send_init
#define mpi_get_count_ mpi_get_count
#define mpi_get_elements_ mpi_get_elements
#define mpi_ibsend_ mpi_ibsend
#define mpi_iprobe_ mpi_iprobe
#define mpi_irecv_ mpi_irecv
#define mpi_irsend_ mpi_irsend
#define mpi_isend_ mpi_isend
#define mpi_issend_ mpi_issend
#define mpi_pack_size_ mpi_pack_size
#define mpi_pack_ mpi_pack
#define mpi_probe_ mpi_probe
#define mpi_recv_ mpi_recv
#define mpi_rsend_init_ mpi_rsend_init
#define mpi_rsend_ mpi_rsend
#define mpi_send_ mpi_send
#define mpi_sendrecv_ mpi_sendrecv
#define mpi_sendrecv_replace_ mpi_sendrecv_replace
#define mpi_ssend_init_ mpi_ssend_init
#define mpi_ssend_ mpi_ssend
#define mpi_startall_ mpi_startall
#define mpi_start_ mpi_start
#define mpi_testall_ mpi_testall
#define mpi_testany_ mpi_testany
#define mpi_test_cancelled_ mpi_test_cancelled
#define mpi_test_ mpi_test
#define mpi_testsome_ mpi_testsome
#define mpi_type_commit_ mpi_type_commit
#define mpi_type_contiguous_ mpi_type_contiguous
#define mpi_type_extent_ mpi_type_extent
#define mpi_type_free_ mpi_type_free
#define mpi_type_hindexed_ mpi_type_hindexed
#define mpi_type_hvector_ mpi_type_hvector
#define mpi_type_indexed_ mpi_type_indexed
#define mpi_type_lb_ mpi_type_lb
#define mpi_type_size_ mpi_type_size
#define mpi_type_struct_ mpi_type_struct
#define mpi_type_ub_ mpi_type_ub
#define mpi_type_vector_ mpi_type_vector
#define mpi_unpack_ mpi_unpack
#define mpi_waitall_ mpi_waitall
#define mpi_waitany_ mpi_waitany
#define mpi_wait_ mpi_wait
#define mpi_waitsome_ mpi_waitsome
#define mpi_allgather_ mpi_allgather
#define mpi_allgatherv_ mpi_allgatherv
#define mpi_allreduce_ mpi_allreduce
#define mpi_alltoall_ mpi_alltoall
#define mpi_alltoallv_ mpi_alltoallv
#define mpi_barrier_ mpi_barrier
#define mpi_bcast_ mpi_bcast
#define mpi_gather_ mpi_gather
#define mpi_gatherv_ mpi_gatherv
#define mpi_op_create_ mpi_op_create
#define mpi_op_free_ mpi_op_free
#define mpi_reduce_scatter_ mpi_reduce_scatter
#define mpi_reduce_ mpi_reduce
#define mpi_scan_ mpi_scan
#define mpi_scatter_ mpi_scatter
#define mpi_scatterv_ mpi_scatterv
#define mpi_finalize_ mpi_finalize
#endif

/*
 * Define prototypes to keep the compiler happy
 */
void mpi_init_ ANSI_ARGS(( int * ));
void mpi_bsend_init_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
				MPI_Comm *, MPI_Request *, int *));
void mpi_bsend_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
			   MPI_Comm *, int *));
void mpi_buffer_attach_ ANSI_ARGS((void *, int *, int *));
void mpi_buffer_detach_ ANSI_ARGS((void *, int *, int *));
void mpi_cancel_ ANSI_ARGS((MPI_Request *, int *));
void mpi_request_free_ ANSI_ARGS((MPI_Request *, int *));
void mpi_recv_init_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
			       MPI_Comm *, MPI_Request *, int *));
void mpi_send_init_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
			       MPI_Comm *, MPI_Request *, int *));
void mpi_get_count_ ANSI_ARGS((MPI_Status *, MPI_Datatype *, int *, int *));
void mpi_get_elements_ ANSI_ARGS((MPI_Status *, MPI_Datatype *, int *, int *));
void mpi_ibsend_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *, 
			    MPI_Comm *, MPI_Request *, int *));
void mpi_iprobe_ ANSI_ARGS((int *, int *, MPI_Comm *, int *, MPI_Status *, 
			    int *));
void mpi_irecv_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *, 
			    MPI_Comm *, MPI_Request *, int * ));
void mpi_irsend_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
			    MPI_Comm *, MPI_Request *, int *));
void mpi_isend_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, int *, 
			    MPI_Comm *, MPI_Request *, int * ));
void mpi_issend_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
			    MPI_Comm *, MPI_Request *, int *));
void mpi_pack_size_ ANSI_ARGS((int *, MPI_Datatype *, MPI_Comm *, int *, 
			       int *));
void mpi_pack_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *, int *,
			  MPI_Comm *, int *));
void mpi_probe_ ANSI_ARGS((int *, int *, MPI_Comm *, MPI_Status *, int *));
void mpi_recv_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, int *, 
			   MPI_Comm *, MPI_Status *, int * ));
void mpi_rsend_init_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
				MPI_Comm *, MPI_Request *, int *));
void mpi_rsend_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *, 
			   MPI_Comm *, int *));
void mpi_send_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, int *, 
			   MPI_Comm *, int * ));
void mpi_sendrecv_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, int *,
			       void *, int *, MPI_Datatype *, int *, int *,
			       MPI_Comm *, MPI_Status *, int * ));
void mpi_sendrecv_replace_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, 
				      int *, int *, int *, MPI_Comm *, 
				      MPI_Status *, int *));
void mpi_ssend_init_ ANSI_ARGS((void *, int *, MPI_Datatype *, int *, int *,
				MPI_Comm *, MPI_Request *, int *));
void mpi_ssend_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, int *,
			    MPI_Comm *, int * ));
void mpi_startall_ ANSI_ARGS((int *, MPI_Request *, int *));
void mpi_start_ ANSI_ARGS((MPI_Request *, int *));
void mpi_testall_ ANSI_ARGS((int *, MPI_Request *, int *, MPI_Status *, 
			     int *));
void mpi_testany_ ANSI_ARGS((int *, MPI_Request *, int *, int *, MPI_Status *,
			     int *));
void mpi_test_cancelled_ ANSI_ARGS((MPI_Status *, int *, int *));
void mpi_test_ ANSI_ARGS(( MPI_Request *, int *, MPI_Status *, int * ));
void mpi_testsome_ ANSI_ARGS((int *, MPI_Request *, int *, int *, MPI_Status *,
			      int *));
void mpi_type_commit_ ANSI_ARGS((MPI_Datatype *, int *));
void mpi_type_contiguous_ ANSI_ARGS((int *, MPI_Datatype *, MPI_Datatype *,
				     int *));
void mpi_type_extent_ ANSI_ARGS((MPI_Datatype *, MPI_Aint *, int *));
void mpi_type_free_ ANSI_ARGS((MPI_Datatype *, int *));
void mpi_type_hindexed_ ANSI_ARGS((int *, int *, MPI_Aint *, MPI_Datatype *,
				   MPI_Datatype *, int *));
void mpi_type_hvector_ ANSI_ARGS((int *, int *, MPI_Aint *, MPI_Datatype *,
				  MPI_Datatype *, int *));
void mpi_type_indexed_ ANSI_ARGS((int *, int *, int *, MPI_Datatype *,
				  MPI_Datatype *, int *));
void mpi_type_lb_ ANSI_ARGS((MPI_Datatype *, MPI_Aint *, int *));
void mpi_type_size_ ANSI_ARGS((MPI_Datatype *, int *, int *));
void mpi_type_struct_ ANSI_ARGS((int *, int *, MPI_Aint *, MPI_Datatype *,
				 MPI_Datatype *, int *));
void mpi_type_ub_ ANSI_ARGS((MPI_Datatype *, MPI_Aint *, int *));
void mpi_type_vector_ ANSI_ARGS((int *, int *, int *, MPI_Datatype *,
				 MPI_Datatype *, int *));
void mpi_unpack_ ANSI_ARGS((void *, int *, int *, void *, int *, 
			    MPI_Datatype *, MPI_Comm *, int *));
void mpi_waitall_ ANSI_ARGS(( int *, MPI_Request *, MPI_Status *, int * ));
void mpi_waitany_ ANSI_ARGS(( int *, MPI_Request *, int *, MPI_Status *, 
			      int * ));
void mpi_wait_ ANSI_ARGS(( MPI_Request *, MPI_Status *, int * ));
void mpi_waitsome_ ANSI_ARGS((int *, MPI_Request *, int *, int *,
			      MPI_Status *, int *));
void mpi_allgather_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *,
			       MPI_Datatype *, MPI_Comm *, int *));
void mpi_allgatherv_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *,
			       int *, MPI_Datatype *, MPI_Comm *, int *));
void mpi_allreduce_ ANSI_ARGS(( void *, void *, int *, MPI_Datatype *, 
				MPI_Op *, MPI_Comm *, int * ));
void mpi_alltoall_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *,
			      MPI_Datatype *, MPI_Comm *, int *));
void mpi_alltoallv_ ANSI_ARGS((void *, int *, int *, MPI_Datatype *, void *, 
			       int *, int *, MPI_Datatype *, MPI_Comm *,
			       int *));
void mpi_barrier_ ANSI_ARGS(( MPI_Comm *, int * ));
void mpi_bcast_ ANSI_ARGS(( void *, int *, MPI_Datatype *, int *, 
			    MPI_Comm *, int * ));
void mpi_gather_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *, 
			   MPI_Datatype *, int *, MPI_Comm *, int *));
void mpi_gatherv_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *,  
			   int *, MPI_Datatype *, int *, MPI_Comm *, int *));
void mpi_op_create_ ANSI_ARGS((MPI_User_function *, int *, MPI_Op *, int *));
void mpi_op_free_ ANSI_ARGS((MPI_Op *, int *));
void mpi_reduce_scatter_ ANSI_ARGS((void *, void *, int *, MPI_Datatype *,
				    MPI_Op *, MPI_Comm *, int *));
void mpi_reduce_ ANSI_ARGS(( void *, void *, int *, MPI_Datatype *, MPI_Op *, 
			     int *, MPI_Comm *, int * ));
void mpi_scan_ ANSI_ARGS((void *, void *, int *, MPI_Datatype *, MPI_Op *,
			  MPI_Comm *, int *));
void mpi_scatter_ ANSI_ARGS((void *, int *, MPI_Datatype *, void *, int *,
			     MPI_Datatype *, int *, MPI_Comm *, int *));
void mpi_scatterv_ ANSI_ARGS((void *, int *, int *, MPI_Datatype *, void *, 
			      int *, MPI_Datatype *, int *, MPI_Comm *, 
			      int *));
void mpi_finalize_ ANSI_ARGS(( int * ));

void mpi_init_( ierr )
int *ierr; 
{
    int Argc;
    int i, argsize = 1024;
    char **Argv, *p;
    int  ArgcSave;           /* Save the argument count */
    char **ArgvSave;         /* Save the pointer to the argument vector */

/* Recover the args with the Fortran routines iargc_ and getarg_ */
    ArgcSave	= Argc = mpir_iargc_() + 1; 
    ArgvSave	= Argv = (char **)MALLOC( Argc * sizeof(char *) );    
    if (!Argv) {
	*ierr = MPIR_ERROR( (MPI_Comm)0, MPI_ERR_OTHER, 
			    "Out of space in MPI_INIT" );
	return;
    }
    for (i=0; i<Argc; i++) {
	ArgvSave[i] = Argv[i] = (char *)MALLOC( argsize + 1 );
	if (!Argv[i]) {
	    *ierr = MPIR_ERROR( (MPI_Comm)0, MPI_ERR_OTHER, 
				"Out of space in MPI_INIT" );
	    return;
        }
	mpir_getarg_( &i, Argv[i], argsize );

	/* Trim trailing blanks */
	p = Argv[i] + argsize - 1;
	while (p > Argv[i]) {
	    if (*p != ' ') {
		p[1] = '\0';
		break;
	    }
	    p--;
	}
    }

    *ierr = MPI_Init( &Argc, &Argv );
    
    /* Recover space */
    for (i=0; i<ArgcSave; i++) {
	FREE( ArgvSave[i] );
    }
    FREE( ArgvSave );
}

 void mpi_bsend_init_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int	     *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
*__ierr = MPI_Bsend_init(buf,*count,*datatype,*dest,*tag,*comm,request);
}

 void mpi_bsend_( buf, count, datatype, dest, tag, comm, __ierr )
void         *buf;
int          *count;
int          *dest;
int          *tag;
MPI_Datatype *datatype;
MPI_Comm     *comm;
int *__ierr;
{
*__ierr = MPI_Bsend(buf,*count,*datatype,*dest,*tag,*comm);
}

 void mpi_buffer_attach_( buffer, size, __ierr )
void *buffer;
int  *size;
int  *__ierr;
{
*__ierr = MPI_Buffer_attach(buffer, *size);
}

void mpi_buffer_detach_( buffer, size, __ierr )
void *buffer;
int  *size;
int  *__ierr;
{
  *__ierr = MPI_Buffer_detach(buffer,size);
}

void mpi_cancel_( request, __ierr )
MPI_Request *request;
int *__ierr;
{
    *__ierr = MPI_Cancel(request); 
}

void mpi_request_free_( request, __ierr )
MPI_Request *request;
int *__ierr;
{
    *__ierr = MPI_Request_free( request );
}

void mpi_recv_init_( buf, count, datatype, source, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *source;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Recv_init(buf, *count, *datatype, *source, *tag, *comm,
			    request);
}

void mpi_send_init_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Send_init(buf, *count, *datatype, *dest, *tag, *comm,
			    request);
}

void mpi_get_count_( status, datatype, count, __ierr )
MPI_Status   *status;
MPI_Datatype *datatype;
int          *count;
int *__ierr;
{
    *__ierr = MPI_Get_count(status, *datatype, count);
}

void mpi_get_elements_ ( status, datatype, elements, __ierr )
MPI_Status   *status;
MPI_Datatype *datatype;
int          *elements;
int *__ierr;
{
    *__ierr = MPI_Get_elements(status, *datatype, elements);
}

void mpi_ibsend_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Ibsend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

void mpi_iprobe_( source, tag, comm, flag, status, __ierr )
int        *source;
int        *tag;
MPI_Comm   *comm;
int        *flag;
MPI_Status *status;
int *__ierr;
{
    *__ierr = MPI_Iprobe(*source, *tag, *comm, flag, status);
}

void mpi_irecv_( buf, count, datatype, source, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *source;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{

    *__ierr = MPI_Irecv(buf, *count, *datatype, *source, *tag, *comm, request);
}

void mpi_irsend_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Irsend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

void mpi_isend_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Isend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

void mpi_issend_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Issend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

void mpi_pack_size_ ( incount, datatype, comm, size, __ierr )
int          *incount;
MPI_Datatype *datatype;
MPI_Comm     *comm;
int          *size;
int *__ierr;
{
    *__ierr = MPI_Pack_size(*incount, *datatype, *comm, size);
}

void mpi_pack_ ( inbuf, incount, type, outbuf, outcount, position, comm, 
		 __ierr )
void         *inbuf;
int          *incount;
MPI_Datatype *type;
void         *outbuf;
int          *outcount;
int          *position;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Pack(inbuf, *incount, *type, outbuf, *outcount, position,
                       *comm);
}

void mpi_probe_( source, tag, comm, status, __ierr )
int        *source;
int        *tag;
MPI_Comm   *comm;
MPI_Status *status;
int *__ierr;
{
    *__ierr = MPI_Probe(*source, *tag, *comm, status);
}

 void mpi_recv_( buf, count, datatype, source, tag, comm, status, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *source;
int          *tag;
MPI_Comm     *comm;
MPI_Status   *status;
int *__ierr;
{
    *__ierr = MPI_Recv(buf, *count, *datatype, *source, *tag, *comm, status);
}

void mpi_rsend_init_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Rsend_init(buf, *count, *datatype, *dest, *tag, *comm,
			     request);
}

void mpi_rsend_( buf, count, datatype, dest, tag, comm, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm *comm;
int *__ierr;
{
    *__ierr = MPI_Rsend(buf, *count, *datatype, *dest, *tag, *comm);
}

void mpi_send_( buf, count, datatype, dest, tag, comm, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Send(buf, *count, *datatype, *dest, *tag, *comm);
}

void mpi_sendrecv_( sendbuf, sendcount, sendtype, dest, sendtag, 
                    recvbuf, recvcount, recvtype, source, recvtag, 
                    comm, status, __ierr )
void         *sendbuf;
int          *sendcount;
MPI_Datatype *sendtype;
int          *dest;
int          *sendtag;
void         *recvbuf;
int          *recvcount;
MPI_Datatype *recvtype;
int          *source;
int          *recvtag;
MPI_Comm     *comm;
MPI_Status   *status;
int *__ierr;
{
    *__ierr = MPI_Sendrecv(sendbuf, *sendcount, *sendtype,
		           *dest,*sendtag,recvbuf,*recvcount,
		           *recvtype,*source,*recvtag,*comm,status);
}

void mpi_sendrecv_replace_( buf, count, datatype, dest, sendtag, source, 
			    recvtag, comm, status, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *sendtag;
int          *source;
int          *recvtag;
MPI_Comm     *comm;
MPI_Status   *status;
int *__ierr;
{
    *__ierr = MPI_Sendrecv_replace(buf, *count, *datatype, *dest, *sendtag,
				   *source, *recvtag, *comm, status );
}

void mpi_ssend_init_( buf, count, datatype, dest, tag, comm, request, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
MPI_Request  *request;
int *__ierr;
{
    *__ierr = MPI_Ssend_init(buf, *count, *datatype, *dest, *tag, *comm,
			     request);
} 

 void mpi_ssend_( buf, count, datatype, dest, tag, comm, __ierr )
void         *buf;
int          *count;
MPI_Datatype *datatype;
int          *dest;
int          *tag;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Ssend(buf, *count, *datatype, *dest, *tag, *comm);
}

void mpi_startall_( count, array_of_requests, __ierr )
int *count;
MPI_Request array_of_requests[];
int *__ierr;

{ 
     *__ierr = MPI_Startall(*count, array_of_requests);
}

void mpi_start_( request, __ierr )
MPI_Request *request;
int *__ierr;
{
    *__ierr = MPI_Start( request );
}

void mpi_testall_( count, array_of_requests, flag, array_of_statuses, __ierr )
int *count;
MPI_Request array_of_requests[];
int *flag;
MPI_Status array_of_statuses[];
int *__ierr;
{
    *__ierr = MPI_Testall(*count, array_of_requests, flag, array_of_statuses);
}

void mpi_testany_( count, array_of_requests, index, flag, status, __ierr )
int         *count;
MPI_Request array_of_requests[];
int         *index; 
int         *flag;
MPI_Status  *status;
int *__ierr;
{
    *__ierr = MPI_Testany(*count, array_of_requests, index, flag, status);
}

void mpi_test_cancelled_( status, flag, __ierr )
MPI_Status *status;
int        *flag;
int *__ierr;
{
    *__ierr = MPI_Test_cancelled(status, flag);
}

void mpi_test_ ( request, flag, status, __ierr )
MPI_Request  *request;
int          *flag;
MPI_Status   *status;
int *__ierr;
{
*__ierr = MPI_Test(request, flag, status);
}

void mpi_testsome_( incount, array_of_requests, outcount, array_of_indices, 
		    array_of_statuses, __ierr )
int         *incount;
MPI_Request array_of_requests[];
int         *outcount;
int         array_of_indices[];
MPI_Status  array_of_statuses[];
int *__ierr;
{
    *__ierr = MPI_Testsome(*incount, array_of_requests, outcount,
			   array_of_indices, array_of_statuses);
}

void mpi_type_commit_ ( datatype, __ierr )
MPI_Datatype *datatype;
int *__ierr;
{
    *__ierr = MPI_Type_commit( datatype );
}

void mpi_type_contiguous_( count, old_type, newtype, __ierr )
int          *count;
MPI_Datatype *old_type;
MPI_Datatype *newtype;
int *__ierr;
{
    *__ierr = MPI_Type_contiguous(*count, *old_type, newtype);
}

void mpi_type_extent_( datatype, extent, __ierr )
MPI_Datatype *datatype;
MPI_Aint     *extent;
int *__ierr;
{
    *__ierr = MPI_Type_extent(*datatype, extent);
}

void mpi_type_free_ ( datatype, __ierr )
MPI_Datatype *datatype;
int *__ierr;
{
    *__ierr = MPI_Type_free(datatype);
}

void mpi_type_hindexed_( count, blocklens, indices, old_type, newtype, __ierr )
int          *count;
int          blocklens[];
MPI_Aint     indices[];
MPI_Datatype *old_type;
MPI_Datatype *newtype;
int *__ierr;
{
    *__ierr = MPI_Type_hindexed(*count, blocklens, indices, *old_type, 
				newtype);
}

void mpi_type_hvector_( count, blocklen, stride, old_type, newtype, __ierr )
int          *count;
int          *blocklen;
MPI_Aint     *stride;
MPI_Datatype *old_type;
MPI_Datatype *newtype;
int *__ierr;
{
    *__ierr = MPI_Type_hvector(*count, *blocklen, *stride, *old_type, newtype);
}

void mpi_type_indexed_( count, blocklens, indices, old_type, newtype, __ierr )
int          *count;
int          blocklens[];
int          indices[];
MPI_Datatype *old_type;
MPI_Datatype *newtype;
int *__ierr;
{
    *__ierr = MPI_Type_indexed(*count, blocklens, indices, *old_type, newtype);
}

void mpi_type_lb_ ( datatype, displacement, __ierr )
MPI_Datatype *datatype;
MPI_Aint     *displacement;
int *__ierr;
{
    *__ierr = MPI_Type_lb(*datatype, displacement);
}

void mpi_type_size_ ( datatype, size, __ierr )
MPI_Datatype *datatype;
int          *size;
int *__ierr;
{
    *__ierr = MPI_Type_size(*datatype, size);
}

void mpi_type_struct_( count, blocklens, indices, old_types, newtype, __ierr )
int          *count;
int          blocklens[];
MPI_Aint     indices[];      
MPI_Datatype old_types[];
MPI_Datatype *newtype;
int     *__ierr;
{
    *__ierr = MPI_Type_struct(*count, blocklens, indices, old_types, newtype);
}

void mpi_type_ub_ ( datatype, displacement, __ierr )
MPI_Datatype *datatype;
MPI_Aint     *displacement;
int *__ierr;
{
    *__ierr = MPI_Type_ub(*datatype, displacement);
}

void mpi_type_vector_( count, blocklen, stride, old_type, newtype, __ierr )
int          *count;
int          *blocklen;
int          *stride;
MPI_Datatype *old_type;
MPI_Datatype *newtype;
int *__ierr;
{
    *__ierr = MPI_Type_vector(*count, *blocklen, *stride, *old_type, newtype);
}

void mpi_unpack_ ( inbuf, insize, position, outbuf, outcount, type, comm, 
		   __ierr )
void         *inbuf;
int          *insize;
int          *position;
void         *outbuf;
int          *outcount;
MPI_Datatype *type;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Unpack(inbuf, *insize, position, outbuf, *outcount, *type,
			 *comm);
}

void mpi_waitall_(count, array_of_requests, array_of_statuses, __ierr )
int         *count;
MPI_Request array_of_requests[];
MPI_Status  array_of_statuses[];
int *__ierr;
{
    *__ierr = MPI_Waitall(*count, array_of_requests, array_of_statuses);
}

void mpi_waitany_(count, array_of_requests, index, status, __ierr )
int         *count;
MPI_Request array_of_requests[];
int         *index;
MPI_Status  *status;
int *__ierr;
{
    *__ierr = MPI_Waitany(*count, array_of_requests, index, status);
}

void mpi_wait_ ( request, status, __ierr )
MPI_Request *request;
MPI_Status  *status;
int *__ierr;
{
*__ierr = MPI_Wait(request, status);
}

void mpi_waitsome_( incount, array_of_requests, outcount, array_of_indices, 
    array_of_statuses, __ierr )
int         *incount;
MPI_Request array_of_requests[];
int         *outcount;
int         array_of_indices[];
MPI_Status array_of_statuses[];
int *__ierr;
{
    *__ierr = MPI_Waitsome(*incount, array_of_requests, outcount,
			   array_of_indices, array_of_statuses);
}

void mpi_allgather_ ( sendbuf, sendcount, sendtype, recvbuf, recvcount, 
		      recvtype, comm, __ierr )
void         *sendbuf;
int          *sendcount;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcount;
MPI_Datatype *recvtype;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Allgather(sendbuf, *sendcount, *sendtype, recvbuf,
			    *recvcount, *recvtype, *comm);
}


void mpi_allgatherv_ ( sendbuf, sendcount,  sendtype, recvbuf, recvcounts, 
		       displs,   recvtype, comm, __ierr )
void         *sendbuf;
int          *sendcount;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcounts;
int          *displs;
MPI_Datatype *recvtype;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Allgatherv(sendbuf, *sendcount, *sendtype, recvbuf, 
			     recvcounts, displs, *recvtype, *comm);
}


void mpi_allreduce_ ( sendbuf, recvbuf, count, datatype, op, comm, __ierr )
void         *sendbuf;
void         *recvbuf;
int          *count;
MPI_Datatype *datatype;
MPI_Op       *op;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Allreduce(sendbuf, recvbuf, *count, *datatype, *op, *comm);
}


void mpi_alltoall_( sendbuf, sendcount, sendtype, recvbuf, recvcnt, recvtype, 
		    comm, __ierr )
void         *sendbuf;
int          *sendcount;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcnt;
MPI_Datatype *recvtype;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Alltoall(sendbuf, *sendcount, *sendtype, recvbuf, *recvcnt,
			   *recvtype, *comm);
}


void mpi_alltoallv_ ( sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts,
		      rdispls, recvtype, comm, __ierr )
void         *sendbuf;
int          *sendcnts;
int          *sdispls;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcnts;
int          *rdispls; 
MPI_Datatype *recvtype;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Alltoallv(sendbuf, sendcnts, sdispls, *sendtype, recvbuf,
			    recvcnts, rdispls, *recvtype, *comm);
}

void mpi_barrier_ ( comm, __ierr )
MPI_Comm *comm;
int *__ierr;
{
    *__ierr = MPI_Barrier(*comm);
}

void mpi_bcast_ ( buffer, count, datatype, root, comm, __ierr )
void         *buffer;
int          *count;
MPI_Datatype *datatype;
int          *root;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Bcast(buffer, *count, *datatype, *root, *comm);
}

void mpi_gather_ ( sendbuf, sendcnt, sendtype, recvbuf, recvcount, recvtype, 
		   root, comm, __ierr )
void         *sendbuf;
int          *sendcnt;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcount;
MPI_Datatype *recvtype;
int          *root;
MPI_Comm *comm;
int *__ierr;
{
    *__ierr = MPI_Gather(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcount,
			 *recvtype, *root, *comm);
}


void mpi_gatherv_ ( sendbuf, sendcnt,  sendtype, recvbuf, recvcnts, displs, 
		    recvtype, root, comm, __ierr )
void         *sendbuf;
int          *sendcnt;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcnts;
int          *displs;
MPI_Datatype *recvtype;
int          *root;
MPI_Comm *comm;
int *__ierr;
{
    *__ierr = MPI_Gatherv(sendbuf, *sendcnt, *sendtype, recvbuf, recvcnts, 
			  displs, *recvtype, *root, *comm);
}


void mpi_op_create_( function, commute, op, __ierr )
MPI_User_function *function;
int               *commute;
MPI_Op            *op;
int *__ierr;
{
    *__ierr = MPI_Op_create(function, *commute, op);
}

void mpi_op_free_( op, __ierr )
MPI_Op *op;
int *__ierr;
{
    *__ierr = MPI_Op_free(op);
}

void mpi_reduce_scatter_ ( sendbuf, recvbuf, recvcnts, datatype, op, comm, 
			   __ierr )
void         *sendbuf;
void         *recvbuf;
int          *recvcnts;
MPI_Datatype *datatype;
MPI_Op       *op;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, *datatype, *op,
                                 *comm);
}

 void mpi_reduce_ ( sendbuf, recvbuf, count, datatype, op, root, comm, __ierr )
void         *sendbuf;
void         *recvbuf;
int          *count;
MPI_Datatype *datatype;
MPI_Op       *op;
int          *root;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Reduce(sendbuf, recvbuf, *count, *datatype, *op, *root,
			 *comm);
}


void mpi_scan_ ( sendbuf, recvbuf, count, datatype, op, comm, __ierr )
void         *sendbuf;
void         *recvbuf;
int          *count;
MPI_Datatype *datatype;
MPI_Op       *op;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Scan(sendbuf, recvbuf, *count, *datatype, *op, *comm);
}

void mpi_scatter_ ( sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, 
                    root, comm, __ierr )
void         *sendbuf;
int          *sendcnt;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcnt;
MPI_Datatype *recvtype;
int          *root;
MPI_Comm     *comm;
int *__ierr;
{
    *__ierr = MPI_Scatter(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcnt, 
			  *recvtype, *root, *comm);
}

void mpi_scatterv_ ( sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt,
		     recvtype, root, comm, __ierr )
void         *sendbuf;
int          *sendcnts;
int          *displs;
MPI_Datatype *sendtype;
void         *recvbuf;
int          *recvcnt;
MPI_Datatype *recvtype;
int          *root;
MPI_Comm *comm;
int *__ierr;
{
    *__ierr = MPI_Scatterv(sendbuf, sendcnts, displs, *sendtype, recvbuf,
                           *recvcnt, *recvtype, *root, *comm);
}

void mpi_finalize_( ierr )
int *ierr; 
{
    *ierr = MPI_Finalize();
}
