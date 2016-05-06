#include "mpid.h"
#include "mpiddev.h"
#include "mpimem.h"
#include "reqalloc.h"
#include "flow.h"
#include "chpackflow.h"

/* NonBlocking Rendezvous */

/* Prototype definitions */
int MPID_CH_Rndvn_send ANSI_ARGS(( void *, int, int, int, int, int, 
					  MPID_Msgrep_t ));
int MPID_CH_Rndvn_isend ANSI_ARGS(( void *, int, int, int, int, int, 
				    MPID_Msgrep_t, MPIR_SHANDLE * ));
int MPID_CH_Rndvn_irecv ANSI_ARGS(( MPIR_RHANDLE *, int, void * ));
int MPID_CH_Rndvn_save ANSI_ARGS(( MPIR_RHANDLE *, int, void *));

int MPID_CH_Rndvn_unxrecv_start ANSI_ARGS(( MPIR_RHANDLE *, void * ));
int MPID_CH_Rndvn_unxrecv_end ANSI_ARGS(( MPIR_RHANDLE * ));
int MPID_CH_Rndvn_unxrecv_test_end ANSI_ARGS(( MPIR_RHANDLE * ));
int MPID_CH_Rndvn_ok_to_send  ANSI_ARGS(( MPID_Aint, MPID_RNDV_T, int ));
int MPID_CH_Rndvn_ack ANSI_ARGS(( void *, int ));
int MPID_CH_Rndvn_send_test ANSI_ARGS(( MPIR_SHANDLE * ));
int MPID_CH_Rndvn_send_wait ANSI_ARGS(( MPIR_SHANDLE * ));
int MPID_CH_Rndvn_send_test_ack ANSI_ARGS(( MPIR_SHANDLE * ));
int MPID_CH_Rndvn_send_wait_ack ANSI_ARGS(( MPIR_SHANDLE * ));
void MPID_CH_Rndvn_delete ANSI_ARGS(( MPID_Protocol * ));

/* Globals for this protocol */
/* This should be state in the protocol/device ?? */
static int CurTag    = 1024;
static int TagsInUse = 0;

/*
 * Definitions of the actual functions
 */

/*
 * This is really the same as the blocking version, since the 
 * nonblocking operations occur only in the data transmission.
 */
int MPID_CH_Rndvn_isend( buf, len, src_lrank, tag, context_id, dest,
			 msgrep, shandle )
void          *buf;
int           len, tag, context_id, src_lrank, dest;
MPID_Msgrep_t msgrep;
MPIR_SHANDLE  *shandle;
{
    MPID_PKT_REQUEST_SEND_T  pkt;
    
    DEBUG_PRINT_MSG("S Starting Rndvn_isend");
#ifdef MPID_PACK_CONTROL
    while (!MPID_PACKET_CHECK_OK(dest)) {  /* begin while !ok loop */
	/* Wait for a protocol ACK packet */
#ifdef MPID_DEBUG_ALL
	if (MPID_DebugFlag || MPID_DebugFlow)
	    FPRINTF(MPID_DEBUG_FILE,
	 "[%d] S Waiting for a protocol ACK packet (in rndvb isend) from %d\n",
		    MPID_MyWorldRank, dest);
#endif
	MPID_DeviceCheck( MPID_BLOCKING );
    }  /* end while !ok loop */

    MPID_PACKET_ADD_SENT(MPID_MyWorldRank, dest )
#endif

    pkt.mode	   = MPID_PKT_REQUEST_SEND;
    pkt.context_id = context_id;
    pkt.lrank	   = src_lrank;
    pkt.to         = dest;
    pkt.src        = MPID_MyWorldRank;
    pkt.seqnum     = sizeof(MPID_PKT_REQUEST_SEND_T);
    pkt.tag	   = tag;
    pkt.len	   = len;
    MPID_DO_HETERO(pkt.msgrep = (int)msgrep);

    /* We save the address of the send handle in the packet; the receiver
       will return this to us */
    MPID_AINT_SET(pkt.send_id,shandle);
	
    /* Store info in the request for completing the message */
    shandle->is_complete     = 0;
    shandle->start	     = buf;
    shandle->bytes_as_contig = len;
    /* Set the test/wait functions */
    shandle->wait	     = MPID_CH_Rndvn_send_wait_ack;
    shandle->test            = MPID_CH_Rndvn_send_test_ack;
    /* Store partners rank in request in case message is cancelled */
    shandle->partner         = dest;
    /* shandle->finish must NOT be set here; it must be cleared/set
       when the request is created */
    DEBUG_PRINT_BASIC_SEND_PKT("S Sending rndv message",&pkt)
    MPID_PKT_PACK( &pkt, sizeof(pkt), dest );
    MPID_DRAIN_INCOMING_FOR_TINY(1);
    MPID_n_pending++;
    MPID_SendControlBlock( &pkt, sizeof(pkt), dest );

    return MPI_SUCCESS;
}

int MPID_CH_Rndvn_send( buf, len, src_lrank, tag, context_id, dest,
			 msgrep )
void          *buf;
int           len, tag, context_id, src_lrank, dest;
MPID_Msgrep_t msgrep;
{
    MPIR_SHANDLE shandle;

    DEBUG_INIT_STRUCT(&shandle,sizeof(shandle));
    MPIR_SET_COOKIE((&shandle),MPIR_REQUEST_COOKIE);
    MPID_SendInit( &shandle );
    shandle.finish = 0;
    MPID_CH_Rndvn_isend( buf, len, src_lrank, tag, context_id, dest,
			 msgrep, &shandle );
    DEBUG_TEST_FCN(shandle.wait,"req->wait");
    shandle.wait( &shandle );
    return MPI_SUCCESS;
}

/*
 * This is the routine called when a packet of type MPID_PKT_REQUEST_SEND is
 * seen and the receive has been posted.  Note the use of a nonblocking
 * receiver BEFORE sending the ack.
 */
int MPID_CH_Rndvn_irecv( rhandle, from, in_pkt )
MPIR_RHANDLE *rhandle;
int          from;
void         *in_pkt;
{
    MPID_PKT_REQUEST_SEND_T *pkt = (MPID_PKT_REQUEST_SEND_T *)in_pkt;
    int    msglen, err = MPI_SUCCESS;
#if defined(MPID_RNDV_SELF)
    MPIR_SHANDLE *shandle;
#endif
    MPID_RNDV_T rtag;

    msglen = pkt->len;

#ifdef MPID_PACK_CONTROL
    if (MPID_PACKET_RCVD_GET(pkt->src)) {
	MPID_SendProtoAck(pkt->to, pkt->src);
    }
    MPID_PACKET_ADD_RCVD(pkt->to, pkt->src);
#endif

    DEBUG_PRINT_MSG("R Starting rndvn irecv");

    /* Check for truncation */
    MPID_CHK_MSGLEN(rhandle,msglen,err)
    /* Note that if we truncate, We really must receive the message in two 
       parts; the part that we can store, and the part that we discard.
       This case is not yet handled. */
    rhandle->s.count	  = msglen;
    rhandle->s.MPI_TAG	  = pkt->tag;
    rhandle->s.MPI_SOURCE = pkt->lrank;
    rhandle->s.MPI_ERROR  = err;
    rhandle->send_id     = pkt->send_id;

#if defined(MPID_RNDV_SELF)
    if (from == MPID_MyWorldRank) {
	DEBUG_PRINT_MSG("R Starting a receive transfer from self");
	MPID_AINT_GET(shandle,pkt->send_id);
#ifdef MPIR_HAS_COOKIES
	if (shandle->cookie != MPIR_REQUEST_COOKIE) {
	    FPRINTF( stderr, "shandle is %lx\n", (long)shandle );
	    FPRINTF( stderr, "shandle cookie is %lx\n", shandle->cookie );
	    MPID_Print_shandle( stderr, shandle );
	    MPID_Abort( (struct MPIR_COMMUNICATOR *)0, 1, "MPI internal", 
			"Bad address in Rendezvous send (irecv-self)" );
	}
#endif	
	/* Copy directly from the shandle */
	MEMCPY( rhandle->buf, shandle->start, shandle->bytes_as_contig );

	shandle->is_complete = 1;
	if (shandle->finish) 
	    (shandle->finish)( shandle );
	MPID_n_pending--;

	/* Update all of the rhandle information */
	rhandle->wait	 = 0;
	rhandle->test	 = 0;
	rhandle->push	 = 0;

	rhandle->is_complete = 1;
	if (rhandle->finish) 
	    (rhandle->finish)( rhandle );
	return err;
    }
#endif

#ifdef MPID_PACK_CONTROL
    while (!MPID_PACKET_CHECK_OK(from)) {  /* begin while !ok loop */
	/* Wait for a protocol ACK packet */
#ifdef MPID_DEBUG_ALL
	if (MPID_DebugFlag || MPID_DebugFlow)
	    FPRINTF(MPID_DEBUG_FILE,
	 "[%d] S Waiting for a protocol ACK packet (in rndvb isend) from %d\n",
		    MPID_MyWorldRank, from);
#endif
	MPID_DeviceCheck( MPID_BLOCKING );
    }  /* end while !ok loop */

    MPID_PACKET_ADD_SENT(pkt->to, from )
#endif

    MPID_CreateRecvTransfer( 0, 0, from, &rtag );
    DEBUG_PRINT_MSG("Starting a nonblocking receive transfer");
    MPID_StartNBRecvTransfer( rhandle->buf, rhandle->len, from, rtag, rhandle, 
			      rhandle->rid );
    MPID_CH_Rndvn_ok_to_send( rhandle->send_id, rtag, from );
    rhandle->recv_handle = rtag;
    rhandle->wait	 = MPID_CH_Rndvn_unxrecv_end;
    rhandle->test	 = MPID_CH_Rndvn_unxrecv_test_end;
    rhandle->push	 = 0;
    /* Must NOT zero finish in case it has already been set */
    rhandle->from    = from;
    rhandle->is_complete = 0;
    
    return err;
}

/* Save an unexpected message in rhandle.  This is the same as
   MPID_CH_Rndvb_save except for the "push" function */
int MPID_CH_Rndvn_save( rhandle, from, in_pkt )
MPIR_RHANDLE *rhandle;
int          from;
void         *in_pkt;
{
    MPID_PKT_REQUEST_SEND_T   *pkt = (MPID_PKT_REQUEST_SEND_T *)in_pkt;

    DEBUG_PRINT_MSG("Saving info on unexpected message");
#ifdef MPID_PACK_CONTROL
    if (MPID_PACKET_RCVD_GET(pkt->src)) {
	MPID_SendProtoAck(pkt->to, pkt->src);
    }
    MPID_PACKET_ADD_RCVD(pkt->to, pkt->src);
#endif

#if defined(MPID_RNDV_SELF)
    if (from == MPID_MyWorldRank) {
	return MPID_CH_Rndvb_save_self( rhandle, from, in_pkt );
    }
#endif
    rhandle->s.MPI_TAG	  = pkt->tag;
    rhandle->s.MPI_SOURCE = pkt->lrank;
    rhandle->s.MPI_ERROR  = 0;
    rhandle->s.count      = pkt->len;
    rhandle->is_complete  = 0;
    rhandle->from         = from;
    rhandle->partner      = pkt->to;
    rhandle->send_id      = pkt->send_id;
    MPID_DO_HETERO(rhandle->msgrep = (MPID_Msgrep_t)pkt->msgrep );
    /* Need to set the push etc routine to complete this transfer */
    rhandle->push         = MPID_CH_Rndvn_unxrecv_start;
    return 0;
}

/*
 * This is an internal routine to return an OK TO SEND packet.
 * It is the same as the Rndvb version
 */
int MPID_CH_Rndvn_ok_to_send( send_id, rtag, from )
MPID_Aint   send_id;
MPID_RNDV_T rtag;
int         from;
{
    MPID_PKT_OK_TO_SEND_T pkt;

    pkt.mode   = MPID_PKT_OK_TO_SEND;
    pkt.lrank  = MPID_MyWorldRank;
    pkt.to     = from;
    pkt.src    = MPID_MyWorldRank;
    pkt.seqnum = sizeof(MPID_PKT_OK_TO_SEND_T);
    MPID_AINT_SET(pkt.send_id,send_id);
    pkt.recv_handle = rtag;
    DEBUG_PRINT_BASIC_SEND_PKT("S Ok send", &pkt);
    MPID_PKT_PACK( &pkt, sizeof(pkt), from );
    MPID_SendControlBlock( &pkt, sizeof(pkt), from );
    return MPI_SUCCESS;
}

/* 
 * This routine is called when it is time to receive an unexpected
 * message.  Note that we start a nonblocking receive FIRST.
 */
int MPID_CH_Rndvn_unxrecv_start( rhandle, in_runex )
MPIR_RHANDLE *rhandle;
void         *in_runex;
{
    MPIR_RHANDLE *runex = (MPIR_RHANDLE *)in_runex;
    MPID_RNDV_T rtag;

#ifdef MPID_PACK_CONTROL
    while (!MPID_PACKET_CHECK_OK(runex->from)) {  /* begin while !ok loop */
	/* Wait for a protocol ACK packet */
#ifdef MPID_DEBUG_ALL
	if (MPID_DebugFlag || MPID_DebugFlow)
	    FPRINTF(MPID_DEBUG_FILE,
	 "[%d] S Waiting for a protocol ACK packet (in rndvb isend) from %d\n",
		    MPID_MyWorldRank, runex->from);
#endif
	MPID_DeviceCheck( MPID_BLOCKING );
    }  /* end while !ok loop */

    MPID_PACKET_ADD_SENT(runex->partner, runex->from )
#endif

    /* Send a request back to the sender, then do the receive */
    MPID_CreateRecvTransfer( 0, 0, from, &rtag );
    DEBUG_PRINT_MSG("Starting a nonblocking receive transfer for unxpted");
    MPID_StartNBRecvTransfer( rhandle->buf, rhandle->len, from, rtag, rhandle, 
			      rhandle->rid );
    MPID_CH_Rndvn_ok_to_send( runex->send_id, rtag, runex->from );
    /* Now, we can either wait for the message to arrive here or
       wait until later (by testing for it in the "end" routine).
       If we wait for it here, we could deadlock if, for example,
       our "partner" is also starting the receive of an unexpected 
       message.
       
       Thus, we save the message tag and set the wait/test functions
       appropriately.
     */
    rhandle->s		 = runex->s;
    rhandle->recv_handle = rtag;
    rhandle->wait	 = MPID_CH_Rndvn_unxrecv_end;
    rhandle->test	 = MPID_CH_Rndvn_unxrecv_test_end;
    /* Must NOT set finish, since it may have been set elsewhere */
    rhandle->push	 = 0;
    rhandle->from        = runex->from;

    MPID_RecvFree( runex );

    return 0;
}

/* 
   This is the wait routine for a rendezvous message that was unexpected.
   A request for the message has already been sent and the receive 
   transfer has been started.
 */
int MPID_CH_Rndvn_unxrecv_end( rhandle )
MPIR_RHANDLE *rhandle;
{

#if !defined(MPID_RNDV_SELF)
    MPID_DeviceCheck( MPID_NOTBLOCKING ); 
#endif

    /* This is a blocking transfer */
    DEBUG_PRINT_MSG("Ending a receive transfer");

    while (!MPID_TestNBRecvTransfer(rhandle)) {
        MPID_DeviceCheck( MPID_NOTBLOCKING );
    }

    MPID_EndNBRecvTransfer( rhandle, rhandle->recv_handle, rhandle->rid );
    DEBUG_PRINT_MSG("Completed receive transfer");
    rhandle->is_complete = 1;
    if (rhandle->finish) 
	(rhandle->finish)( rhandle );

    return MPI_SUCCESS;
}

/* 
   This is the test routine for a rendezvous message that was unexpected.
   A request for the message has already been sent, and the receive has been
   started.
 */
int MPID_CH_Rndvn_unxrecv_test_end( rhandle )
MPIR_RHANDLE *rhandle;
{
    if (MPID_TestNBRecvTransfer( rhandle )) {
	/* Note that a successful test completes (!) */
	DEBUG_PRINT_MSG("Completed receive transfer");
	rhandle->is_complete = 1;
	if (rhandle->finish) 
	    (rhandle->finish)( rhandle );
    }

    return MPI_SUCCESS;
}

/* 
 * This is the routine that is called when an "ok to send" packet is
 * received.  
 */
int MPID_CH_Rndvn_ack( in_pkt, from_grank )
void  *in_pkt;
int   from_grank;
{
    MPID_PKT_OK_TO_SEND_T *pkt = (MPID_PKT_OK_TO_SEND_T *)in_pkt;
    MPIR_SHANDLE *shandle=0;

    DEBUG_PRINT_MSG("R Starting Rndvb_ack");
#ifdef MPID_PACK_CONTROL
    if (MPID_PACKET_RCVD_GET(pkt->src)) {
	MPID_SendProtoAck(pkt->to, pkt->src);
    }
    MPID_PACKET_ADD_RCVD(pkt->to, pkt->src);
#endif

    MPID_AINT_GET(shandle,pkt->send_id);
#ifdef MPIR_HAS_COOKIES
    if (shandle->cookie != MPIR_REQUEST_COOKIE) {
	FPRINTF( stderr, "shandle is %lx\n", (long)shandle );
	FPRINTF( stderr, "shandle cookie is %lx\n", shandle->cookie );
	MPID_Print_shandle( stderr, shandle );
	MPID_Abort( (struct MPIR_COMMUNICATOR *)0, 1, "MPI internal", 
		    "Bad address in Rendezvous send" );
    }
#endif	

#ifdef MPID_DEBUG_ALL
    if (MPID_DebugFlag) {
	FPRINTF( MPID_DEBUG_FILE, "[%d]S for ", MPID_MyWorldRank );
	MPID_Print_shandle( MPID_DEBUG_FILE, shandle );
    }
#endif

    DEBUG_PRINT_MSG("Sending data on channel with nonblocking send");
    MPID_n_pending--;
    MPID_StartNBSendTransfer( shandle->start, shandle->bytes_as_contig, 
			      from_grank, pkt->recv_handle, shandle->sid );
    /* saving recv_handle needed ONLY for tracing */
    shandle->recv_handle = pkt->recv_handle;
    shandle->is_complete = 0;
    shandle->wait	 = MPID_CH_Rndvn_send_wait;
    shandle->test	 = MPID_CH_Rndvn_send_test;
    /* If the ref count is 0, we should just forget about the request, 
       as in the shared memory case.  For this, we'll need a request
       free operation in the interface */
    return MPI_SUCCESS;
}

int MPID_CH_Rndvn_send_wait( shandle )
MPIR_SHANDLE *shandle;
{
    DEBUG_PRINT_MSG("Ending send transfer");
    MPID_EndNBSendTransfer( shandle, shandle->recv_handle, shandle->sid );
    shandle->is_complete = 1;
    if (shandle->finish) 
	(shandle->finish)( shandle );
    return 0;
}

int MPID_CH_Rndvn_send_test( shandle )
MPIR_SHANDLE *shandle;
{
    DEBUG_PRINT_MSG("Testing for end send transfer" );
    if (MPID_TestNBSendTransfer( shandle->sid )) {
	shandle->is_complete = 1;
	if (shandle->finish) 
	    (shandle->finish)( shandle );
    }
    return 0;
}

/* These wait for the "ack" and then change the wait routine on the
   handle */
int MPID_CH_Rndvn_send_wait_ack( shandle )
MPIR_SHANDLE *shandle;
{
    DEBUG_PRINT_MSG("Waiting for Rndvn ack");
    while (!shandle->is_complete && 
	   shandle->wait == MPID_CH_Rndvn_send_wait_ack) {
	MPID_DeviceCheck( MPID_BLOCKING );
    }
    if (!shandle->is_complete) {
	DEBUG_TEST_FCN(shandle->wait,"shandle->wait");
	return (shandle->wait)( shandle );
    }
    return 0;
}

int MPID_CH_Rndvn_send_test_ack( shandle )
MPIR_SHANDLE *shandle;
{
    DEBUG_PRINT_MSG("Testing for Rndvn ack" );
    if (!shandle->is_complete &&
	shandle->test == MPID_CH_Rndvn_send_test_ack) {
	MPID_DeviceCheck( MPID_NOTBLOCKING );
    }

    return 0;
}

/* 
 * CancelSend 
 * This is fairly hard.  We need to send a "please_cancel_send", 
 * which, if the message is found in the unexpected queue, removes it.
 * However, if the message is being received at the "same" moment, the
 * ok_to_send and cancel_send messages could cross.  To handle this, the
 * receiver must ack the cancel_send message (making the success of the
 * cancel non-local).  There are even more complex protocols, but we won't
 * bother.
 * 
 * Don't forget to update MPID_n_pending as needed.
 */

void MPID_CH_Rndvn_delete( p )
MPID_Protocol *p;
{
    FREE( p );
}

/*
 * The only routing really visable outside this file; it defines the
 * Blocking Rendezvous protocol.
 */
MPID_Protocol *MPID_CH_Rndvn_setup()
{
    MPID_Protocol *p;

    p = (MPID_Protocol *) MALLOC( sizeof(MPID_Protocol) );
    if (!p) return 0;
    p->send	   = MPID_CH_Rndvn_send;
    p->recv	   = 0;
    p->isend	   = MPID_CH_Rndvn_isend;
    p->wait_send   = 0;
    p->push_send   = 0;
    p->cancel_send = 0;
    p->irecv	   = MPID_CH_Rndvn_irecv;
    p->wait_recv   = 0;
    p->push_recv   = 0;
    p->cancel_recv = 0;
    p->do_ack      = MPID_CH_Rndvn_ack;
    p->unex        = MPID_CH_Rndvn_save;
    p->delete      = MPID_CH_Rndvn_delete;

    return p;
}
