/*
 *  $Id: chpackflow.c,v 1.5 1999/08/05 20:59:08 swider Exp $
 *
 *  (C) 1996 by Argonne National Laboratory and Mississipi State University.
 *      All rights reserved.  See COPYRIGHT in top-level directory.
 */

#include "mpid.h"
#include "mpiddev.h"
#include "mpimem.h"
#include "chpackflow.h"
#include "flow.h"

MPID_Packets MPID_pack_info;
#ifdef MPID_GET_LAST_PKT
    int total_pack_unacked;
    int expect_ack;
#endif

/* Initialize packet flow struct and arrays */
void MPID_PacketFlowSetup( ) 
{  /* begin MPID_PacketFlowSetup */

    int i;

    for (i=0; i<MPID_MyWorldSize; i++) {  /* begin for i loop */
	MPID_pack_info.pack_sent = (int *)MALLOC(MPID_MyWorldSize * 
						 sizeof(int));
	MPID_pack_info.pack_rcvd = (int *)MALLOC(MPID_MyWorldSize * 
						 sizeof(int));
    }  /* end for i loop */

    for (i=0; i<MPID_MyWorldSize; i++) {  /* begin for i loop */
	MPID_pack_info.pack_sent[i] = 0;
	MPID_pack_info.pack_rcvd[i] = 0;
    }  /* end for i loop */

#ifdef MPID_GET_LAST_PKT
    total_pack_unacked = 0;
    expect_ack = 0;
#endif
}  /* end MPID_PacketFlowSetup */


/* Send Protocol ACK packet */
void MPID_SendProtoAck( me, partner )
int me, partner;

{  /* begin MPID_SendProtoAck */

#ifdef MPID_USE_SHMEM
    MPID_PKT_FLOW_T *ack_pkt;
    ack_pkt = (MPID_PKT_FLOW_T *) MPID_SHMEM_GetSendPkt(0);
#else
    MPID_PKT_FLOW_T ack_pkt;
#endif

    MPID_PACKET_SUB_RCVD(me, partner);
    DEBUG_PRINT_MSG("- Sending protocol ACK packet");
#ifdef MPID_USE_SHMEM
    ack_pkt->mode = MPID_PKT_PROTO_ACK;
    ack_pkt->lrank = me;
    ack_pkt->to = partner;
#else
    ack_pkt.mode = MPID_PKT_PROTO_ACK;
    ack_pkt.lrank = me;
    ack_pkt.to = partner;
    MPID_PKT_PACK( &ack_pkt, sizeof(MPID_PKT_HEAD_T), partner );
#endif

#ifdef MPID_USE_SHMEM
    MPID_SHMEM_SendControl( (MPID_PKT_T *)ack_pkt, sizeof(MPID_PKT_FLOW_T), 
			    partner ); 
#else
    MPID_SendControl( &ack_pkt, sizeof(MPID_PKT_FLOW_T), partner );
#endif

#ifdef MPID_GET_LAST_PKT
    total_pack_unacked++;
#endif
}  /* end MPID_SendProtoAck */


/* Receive Protocol Ack or ACK protocol packet and send an ACK proto
   packet. */
void MPID_RecvProtoAck( in_pkt, partner )
MPID_PKT_T *in_pkt;
int        partner;

{  /* begin MPID_RecvProtoAck */
    
    MPID_PKT_FLOW_T *ack_pkt = (MPID_PKT_FLOW_T *)in_pkt;
    int me = ack_pkt->to;
#ifdef MPID_USE_SHMEM
    MPID_PKT_FLOW_T *new_ack_pkt;
#else
    MPID_PKT_FLOW_T new_ack_pkt;
#endif    

    if (me == partner) {  /* begin if me == partner */

	DEBUG_PRINT_MSG("- Receiving protocol ACK  packet");
        MPID_PACKET_SUB_SENT(me, partner);
#ifdef MPID_USE_SHMEM
       MPID_SHMEM_FreeRecvPkt( (MPID_PKT_T *)in_pkt );
#endif
#ifdef MPID_GET_LAST_PKT
    total_pack_unacked--;
#endif
       return;
    }  /* end if me == partner */
	
    if (ack_pkt->mode == MPID_PKT_PROTO_ACK) {  /* begin if mode */

	DEBUG_PRINT_MSG("- Receiving protocol ACK  packet");
        MPID_PACKET_SUB_SENT(me, partner);

#ifdef MPID_USE_SHMEM
       new_ack_pkt = (MPID_PKT_FLOW_T *) MPID_SHMEM_GetSendPkt(0);
       new_ack_pkt->mode = MPID_PKT_ACK_PROTO; 
       new_ack_pkt->lrank = me;
       new_ack_pkt->to = partner; 
#else
       new_ack_pkt.mode = MPID_PKT_ACK_PROTO; 
       new_ack_pkt.lrank = me;
       new_ack_pkt.to = partner; 
       MPID_PKT_PACK(&new_ack_pkt, sizeof(MPID_PKT_HEAD_T), partner );
#endif
       DEBUG_PRINT_MSG("- Sending ACK PROTO packet");

#ifdef MPID_USE_SHMEM
       MPID_SHMEM_SendControl( (MPID_PKT_T *)new_ack_pkt, 
			       sizeof(MPID_PKT_FLOW_T), partner );
       MPID_SHMEM_FreeRecvPkt( (MPID_PKT_T *)in_pkt );
#else
       MPID_SendControl( &new_ack_pkt, sizeof(MPID_PKT_FLOW_T), partner );
#endif

    }  /* end if mode */

    else if (ack_pkt->mode == MPID_PKT_ACK_PROTO) {  /* begin else if */

	DEBUG_PRINT_MSG("- Receiving ACK protocol packet");	
#ifdef MPID_USE_SHMEM
	MPID_SHMEM_FreeRecvPkt( (MPID_PKT_T *)in_pkt );
#endif

#ifdef MPID_GET_LAST_PKT
    total_pack_unacked--;
#endif
    }  /* end else if */

}  /* end MPID_RecvProtoAck */


#ifdef MPID_GET_LAST_PKT
/* Make sure you receive all unacked packets.  If this is not called,
   MPID_CH_End will hang */
void MPID_FinishRecvPackets( dev )
MPID_Device *dev;

{  /* begin MPID_FinishRecvPackets */


    DEBUG_PRINT_MSG("Entering MPID_FinishRecvPackets");
    
    while (expect_ack > 0) 
	MPID_DeviceCheck( MPID_BLOCKING );

    while (total_pack_unacked > 0) 
	MPID_DeviceCheck( MPID_BLOCKING );
    
    DEBUG_PRINT_MSG("Leaving MPID_FinishRecvPackets");

}  /* end MPID_FinishRecvPackets */
#endif


/* Free memory from packet flow struct and arrays */
void MPID_PackDelete()
{  /* begin MPID_PackDelete */

	FREE( MPID_pack_info.pack_rcvd ); 
	FREE( MPID_pack_info.pack_sent );

}  /* end MPID_PackDelete */
