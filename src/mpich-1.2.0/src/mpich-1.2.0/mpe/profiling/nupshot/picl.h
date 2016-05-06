/*
   Old-style Picl trace file handling routines for Upshot

   Ed Karrels
   Argonne National Laboratory
*/

/*
   Format:

   Picl-1 tracefiles can be in either 'verbose' or 'compact' form.
   When in verbose form, lots of extra strings are strewn throughout.

   Each record type description will be shown first in verbose form,
   then in compact form, with the numeric (compact) form of the record type
   in parentheses.

   Every line starts with either (verbose vs. compact):
   <recordName> clock <sec> <usec> node <node>
   <recordNo> <sec> <usec> <node>

   Then there may be recordtype-specific data, and optional
   comments.  For the recordtype-specific data:

   trace_start(1):
      event <e> compstats <comp> commstats <comm>
      <e> <comp> <comm>
      Not used by Upshot

   open(2): none
      Marks a call to open0()
   load(3): none
      Marks a call to load0(), node=-1 means all processes
   send(4):
      to <procno> type <msg_type> lth <msg_length>
      <procno> <msg_type> <msg_length>
      Marks the sending of a message

   recv(6):
      from <procno> type <msg_type> lth <msg_length>
      <procno> <msg_type> <msg_length>
      Marks the immediate receiving of a message

   recv_blocking(7):
      type <msg_type>
      <msg_type>
      Marks the start of a wait for a message of type <msg_type>

   recv_waking(8):
      from <procno> type <msg_type> lth <msg_length>
      <procno> <msg_type> <msg_length>
      Marks the receive of a message that the process has been waiting for.

   message(9): none
      Marks a call to message0().

   sync(10): none
      Marks the start of a call to sync0().

   compstats(11):
      busy <busy_sec> <busy_usec> idle <idle_sec> <idle_usec>
      <busy_sec> <busy_usec> <idle_sec> <idle_usec>
      For operations that will take time, a compstat record is generated
      before and after the call, so the difference in idle time may
      be used to compute time spent in the call.

   commstats(12):
      received <n_recvd> volume <recv_vol> sent <n_sent> volume <send_vol> \
         probed <n_probed>
      <n_recvd> <recv_vol> <n_sent> <send_vol> <n_probed>
      Shows stats on communications done so far.  Is this guaranteed to
      be in the logfile somewhere?  It is not used by Upshot.

   close(13): none
      Marks a call to close(0).

   trace_level(14): none, not used by Upshot

   trace_mark(15):
      type <type_no>
      <type_no>
      Marks a user-defined event.

   trace_message(16): none, not used by Upshot

   trace_stop(17): none, not used by Upshot

   trace_flush(18): none, not used by Upshot

   tract_exit(19): none, not used by Upshot

   block_begin(20):
      block_type <type> location_type <loc> parameter_type <param>
      <type> <loc> <param>
      Marks the start of a collective operation or user-defined
      computation state.
   
   block_end(21):
      block_type <type> location_type <loc> parameter_type <param>
      <type> <loc> <param>
      Marks the end of a block started by block_begin

   trace_marks(22): not used by Upshot

*/



#ifndef PICL_H
#define PICL_H

#include "log.h"

#ifdef __STDC__
int Picl_Open( logFile *log );
#else
int Picl_Open();
#endif
   
#endif
/* PICL_H */
