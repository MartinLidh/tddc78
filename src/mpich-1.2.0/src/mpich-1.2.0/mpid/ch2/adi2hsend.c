/*
 *  $Id: adi2hsend.c,v 1.2 1997/10/06 18:30:34 gropp Exp $
 *
 *  (C) 1996 by Argonne National Laboratory and Mississipi State University.
 *      All rights reserved.  See COPYRIGHT in top-level directory.
 */

#include "mpid.h"
#include "mpiddev.h"
#include "mpimem.h"

/***************************************************************************/
/*
 * Multi-protocol, Multi-device support for 2nd generation ADI.
 * This file has support for noncontiguous sends for systems that do not 
 * have native support for complex datatypes.
 */
/***************************************************************************/

void MPID_SendDatatype( comm_ptr, buf, count, dtype_ptr, src_lrank, tag, 
			context_id, dest_grank, error_code )
struct MPIR_COMMUNICATOR *comm_ptr;
struct MPIR_DATATYPE     *dtype_ptr;
void         *buf;
int          count, src_lrank, tag, context_id, dest_grank, *error_code;
{
    int             len, contig_size;
    void            *mybuf;
    MPID_Msgrep_t   msgrep = MPID_MSGREP_RECEIVER;
    MPID_Msg_pack_t msgact = MPID_MSG_OK;

    /*
     * Alogrithm:
     * First, see if we can just send the data (contiguous or, for
     * heterogeneous, packed).
     * Otherwise, 
     * Create a local buffer, use SendContig, and then free the buffer.
     */

    contig_size = MPIR_GET_DTYPE_SIZE(datatype,dtype_ptr);

    MPID_DO_HETERO(MPID_Msg_rep( comm_ptr, dest_grank, dtype_ptr, 
				 &msgrep, &msgact ));
    
    if (contig_size > 0
	MPID_DO_HETERO(&& msgact == MPID_MSG_OK)) {
	/* Just drop through into the contiguous send routine 
	   For packed data, the representation format is that in the
	   communicator.
	 */
	len = contig_size * count;
	MPID_SendContig( comm_ptr, buf, len, src_lrank, tag, context_id, 
			 dest_grank, msgrep, error_code );
	return;
    }

    mybuf = 0;
    MPID_PackMessage( buf, count, dtype_ptr, comm_ptr, dest_grank, msgrep, 
		      msgact, (void **)&mybuf, &len, error_code );
    if (*error_code) return;

    MPID_SendContig( comm_ptr, mybuf, len, src_lrank, tag, context_id, 
		     dest_grank, msgrep, error_code );
    if (mybuf) {
	FREE( mybuf );
    }
}

/*
 * Noncontiguous datatype isend
 * This is a simple implementation.  Note that in the rendezvous case, the
 * "pack" could be deferred until the "ok to send" message arrives.  To
 * implement this, the individual "send" routines would have to know how to
 * handle general datatypes.  We'll leave that for later.
 */
void MPID_IsendDatatype( comm_ptr, buf, count, dtype_ptr, src_lrank, tag, 
			 context_id, dest_grank, request, error_code )
struct MPIR_COMMUNICATOR *comm_ptr;
struct MPIR_DATATYPE     *dtype_ptr;
void         *buf;
int          count, src_lrank, tag, context_id, dest_grank, *error_code;
MPI_Request  request;
{
    int             len, contig_size;
    char            *mybuf;
    MPID_Msgrep_t   msgrep = MPID_MSGREP_RECEIVER;
    MPID_Msg_pack_t msgact = MPID_MSG_OK;

    /*
     * Alogrithm:
     * First, see if we can just send the data (contiguous or, for
     * heterogeneous, packed).
     * Otherwise, 
     * Create a local buffer, use SendContig, and then free the buffer.
     */

    /* Just in case; make sure that finish is 0 */
    request->shandle.finish = 0;

    contig_size = MPIR_GET_DTYPE_SIZE(datatype,dtype_ptr);
    MPID_DO_HETERO(MPID_Msg_rep( comm_ptr, dest_grank, dtype_ptr, 
				 &msgrep, &msgact ));
    if (contig_size > 0 
	MPID_DO_HETERO(&& msgact == MPID_MSG_OK)) {
	/* Just drop through into the contiguous send routine 
	   For packed data, the representation format is that in the
	   communicator.
	 */
	len = contig_size * count;
	MPID_IsendContig( comm_ptr, buf, len, src_lrank, tag, context_id, 
			  dest_grank, msgrep, request, error_code );
	return;
    }

    mybuf = 0;
    MPID_PackMessage( buf, count, dtype_ptr, comm_ptr, dest_grank, 
		      msgrep, msgact,
		      (void **)&mybuf, &len, error_code );
    if (*error_code) return;

    MPID_IsendContig( comm_ptr, mybuf, len, src_lrank, tag, context_id, 
		      dest_grank, msgrep, request, error_code );
    if (request->shandle.is_complete) {
	if (mybuf) { FREE( mybuf ); }
	}
    else {
	/* PackMessageFree saves allocated buffer in "start" */
	request->shandle.start  = mybuf;
	request->shandle.finish = MPID_PackMessageFree;
    }

    /* Note that, from the users perspective, the message is now complete
       (!) since the data is out of the input buffer (!) */
}
