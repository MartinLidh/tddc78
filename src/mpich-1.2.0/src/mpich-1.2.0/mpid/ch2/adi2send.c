/*
 *  $Id: adi2send.c,v 1.1.1.1 1997/09/17 20:39:22 gropp Exp $
 *
 *  (C) 1995 by Argonne National Laboratory and Mississipi State University.
 *      All rights reserved.  See COPYRIGHT in top-level directory.
 */

#include "mpid.h"
#include "mpiddev.h"
/* flow.h includs the optional flow control for eager delivery */
#include "flow.h"

/***************************************************************************/
/*
 * Multi-protocol, Multi-device support for 2nd generation ADI.
 * We start with support for blocking, contiguous sends.
 * Note the 'msgrep' field; this gives a hook for heterogeneous systems
 * which can be ignored on homogeneous systems.
 */
/***************************************************************************/

void MPID_SendContig( comm_ptr, buf, len, src_lrank, tag, context_id, 
			      dest_grank, msgrep, error_code )
struct MPIR_COMMUNICATOR *comm_ptr;
void     *buf;
int      len, src_lrank, tag, context_id, dest_grank, *error_code;
MPID_Msgrep_t msgrep;
{
    MPID_Device *dev = MPID_devset->dev[dest_grank];
    int (*fcn) ANSI_ARGS(( void *, int, int, int, int, int, MPID_Msgrep_t ));

    /* The one error test that makes sense here */
    if (buf == 0 && len > 0) {
	*error_code = MPI_ERR_BUFFER;
	return;
    }
    /* Choose the function based on the message length in bytes */
    if (len < dev->long_len)
	fcn = dev->short_msg->send;
    else if (len < dev->vlong_len && MPID_FLOW_MEM_OK(len,dest_grank)) 
	fcn = dev->long_msg->send;
    else
	fcn = dev->vlong_msg->send;
    DEBUG_TEST_FCN(fcn,"dev->proto->send");
    *error_code = (*(fcn))( buf, len, src_lrank, tag, context_id, dest_grank, 
			    msgrep );
}

void MPID_IsendContig( comm_ptr, buf, len, src_lrank, tag, context_id, 
		       dest_grank, msgrep, request, error_code )
struct MPIR_COMMUNICATOR *   comm_ptr;
void        *buf;
int         len, src_lrank, tag, context_id, dest_grank, *error_code;
MPID_Msgrep_t msgrep;
MPI_Request request;
{
    MPID_Device *dev = MPID_devset->dev[dest_grank];
    int (*fcn) ANSI_ARGS(( void *, int, int, int, int, int, MPID_Msgrep_t, 
			   MPIR_SHANDLE * ));

    /* The one error test that makes sense here */
    if (buf == 0 && len > 0) {
	*error_code = MPI_ERR_BUFFER;
	return;
    }

    /* Just in case; make sure that finish is 0 */
    request->shandle.finish = 0;

    /* Choose the function based on the message length in bytes */
    if (len < dev->long_len)
	fcn = dev->short_msg->isend;
    else if (len < dev->vlong_len && MPID_FLOW_MEM_OK(len,dest_grank)) 
	fcn = dev->long_msg->isend;
    else
	fcn = dev->vlong_msg->isend;
    DEBUG_TEST_FCN(fcn,"dev->proto->isend");
    *error_code = (*(fcn))( buf, len, src_lrank, tag, context_id, dest_grank, 
			    msgrep, (MPIR_SHANDLE *)request );
}


/* Bsend is just a test for short send */
void MPID_BsendContig( comm_ptr, buf, len, src_lrank, tag, context_id, 
			 dest_grank, msgrep, error_code )
struct MPIR_COMMUNICATOR *comm_ptr;
void     *buf;
int      len, src_lrank, tag, context_id, dest_grank, *error_code;
MPID_Msgrep_t msgrep;
{
    MPID_Device *dev = MPID_devset->dev[dest_grank];
    int rc;

    if (len < dev->long_len) {
	DEBUG_TEST_FCN(dev->short_msg->send,"dev->short->send");
	rc = (*dev->short_msg->send)( buf, len, src_lrank, tag, context_id, 
				      dest_grank, msgrep );
    }
    else
	rc = MPIR_ERR_MAY_BLOCK;
    *error_code = rc;
}

int MPID_SendIcomplete( request, error_code )
MPI_Request request;
int         *error_code;
{
    MPIR_SHANDLE *shandle = &request->shandle;
    int lerr;

    if (shandle->is_complete) {
	if (shandle->finish) 
	    (shandle->finish)( shandle );
	return 1;
    }
    if (shandle->test) 
	*error_code = 
	    (*shandle->test)( shandle );
    else {
	/* The most common case is a check device loop */
	MPID_Device *dev;
	dev = MPID_devset->dev_list;
	while (dev) {
	    lerr = (*dev->check_device)( dev, MPID_NOTBLOCKING );
	    if (lerr > 0) {
		*error_code = lerr;
		return 0;
	    }
	    dev = dev->next;
	}
    }
    if (shandle->is_complete && shandle->finish) 
	(shandle->finish)( shandle );
    return shandle->is_complete;
}

void MPID_SendComplete( request, error_code )
MPI_Request request;
int         *error_code;
{
    MPIR_SHANDLE *shandle = &request->shandle;
    int          lerr;

    /* The 'while' is at the top in case the 'wait' routine is changed
       by one of the steps.  This happens, for example, in the Rendezvous
       Protocol */
    while (!shandle->is_complete) {
	if (shandle->wait) 
	    *error_code = 
		(*shandle->wait)( shandle );
	else {
	    /* The most common case is a check device loop until it is
	       complete. */
	    MPID_Device *dev;

	    if (MPID_devset->ndev_list == 1) {
		dev = MPID_devset->dev_list;
		if (!shandle->is_complete) {
		    lerr = (*dev->check_device)( dev, MPID_BLOCKING );
		    if (lerr > 0) {
			*error_code = lerr;
			return;
		    }
		}
	    }
	    else {
		if (!shandle->is_complete) {
		    dev = MPID_devset->dev_list;
		    while (dev) {
			lerr = (*dev->check_device)( dev, MPID_NOTBLOCKING );
			if (lerr > 0) {
			    *error_code = lerr;
			    return;
			}
			dev = dev->next;
		    }
		}
	    }
	}
    }
    if (shandle->finish) 
	(shandle->finish)( shandle );
}

