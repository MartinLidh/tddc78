/*
 *  $Id: adi2probe.c,v 1.1.1.1 1997/09/17 20:39:29 gropp Exp $
 *
 *  (C) 1995 by Argonne National Laboratory and Mississipi State University.
 *      All rights reserved.  See COPYRIGHT in top-level directory.
 */

#include "mpid.h"
#include "mpiddev.h"
#include "mpid_debug.h"
#include "../util/queue.h"

void MPID_Iprobe( comm_ptr, tag, context_id, src_lrank, found, error_code, 
		  status )
struct MPIR_COMMUNICATOR *comm_ptr;
int        tag, context_id, src_lrank, *found, *error_code;
MPI_Status *status;
{
    MPIR_RHANDLE *rhandle;

    DEBUG_PRINT_MSG("Entering Iprobe");
    DEBUG_PRINT_ARGS("Iprobe");
/* At this time, we check to see if the message has already been received */
    MPID_Search_unexpected_queue( src_lrank, tag, context_id, 0, &rhandle );
    if (!rhandle) {
	/* If nothing there, check for incoming messages.... */
	/* MPID_check_incoming should return whether it found anything when
	   called in the non-blocking mode... */
	MPID_DeviceCheck( MPID_NOTBLOCKING );
	MPID_Search_unexpected_queue( src_lrank, tag, context_id, 0, 
				      &rhandle );
    }
    if (rhandle) {
	*found  = 1;
	*status = rhandle->s;
	DEBUG_PRINT_MSG(" Iprobe found msg");
    } 
    else {
	*found = 0;
	DEBUG_PRINT_MSG(" Iprobe did not find msg");
    }
    DEBUG_PRINT_MSG("Exiting Iprobe");
}

void MPID_Probe( comm_ptr, tag, context_id, src_lrank, error_code, status )
struct MPIR_COMMUNICATOR *comm_ptr;
int        tag, src_lrank, context_id, *error_code;
MPI_Status *status;
{
    int found;

    *error_code = 0;
    DEBUG_PRINT_MSG("Entering Probe");
    while (1) {
	MPID_Iprobe( comm_ptr, tag, context_id, src_lrank, &found, error_code, 
		     status );
	if (found || *error_code) break;
	/* Wait for a message */
	MPID_DeviceCheck( MPID_BLOCKING );
    }
    DEBUG_PRINT_MSG("Exiting Probe");
}
