#include "p4.h"
#include "p4_sys.h"

/* This routine gies us a way to timeout a loop */
#include <sys/time.h>
#ifndef TIMEOUT_VALUE 
#define TIMEOUT_VALUE 300
#endif
static int p4_timeout_value = TIMEOUT_VALUE;

int p4_has_timedout( flag )
int flag;
{
    static time_t start_time;
    time_t curtime;
    curtime = time((time_t)0);
    if (flag) {
	if (curtime - start_time > p4_timeout_value) return 1;
    }
    else
	start_time = curtime;
    return 0;
}

int p4_establish_all_conns()
{
    int myid = p4_get_my_id();
    int i;

    for (i = 0; i < p4_global->num_in_proctable; i++) {
	if ( i > myid ) {
	    if (!in_same_cluster(i,myid)) {
		if (p4_local->conntab[i].type == CONN_REMOTE_NON_EST) {
		    p4_dprintfl(20,"establishing early connection to %d\n",i);
		    establish_connection(i);
		}
	    }
	}
    }
    return 0;
}

/* see p4_sock_list.c for the thread version */
#ifndef THREAD_LISTENER

int establish_connection(dest_id)
int dest_id;
{
    int myid = p4_get_my_id();

    p4_global->dest_id[myid] = dest_id;
    request_connection(dest_id);
    p4_global->dest_id[myid] = (-1);

    if (myid > dest_id)
    {
	/* following should not spin long */
        p4_has_timedout( 0 );
	/* If threaded, we should wait for the message from the thread,
	   rather than spin here */
	p4_dprintfl(70, "waiting for interrupt handler to do its job\n");
	while (p4_local->conntab[dest_id].type != CONN_REMOTE_EST) {
	    p4_dprintfl(111, "waiting in loop for interrupt handler to do its job\n");
	    if (p4_has_timedout( 1 )) {
		p4_error( "Timeout in establishing connection to remote process", 0 );
		}
	    }
	p4_dprintfl(70, "interrupt handler succeeded\n");
    }
    return (P4_TRUE);
}

/*
 * This routine may be called by the thread listener after marking the 
 * connection as opening.  The second argument is used to indicate this
 * case.
 */
P4VOID request_connection(dest_id)
int dest_id;
{
    struct proc_info *my_pi, *dest_pi;
    char *my_host, *dest_host;
    int my_id;
    struct slave_listener_msg msg;
    int connection_fd;
    int dest_listener_con_fd;
    int my_listener, dest_listener;
    int new_listener_port, new_listener_fd;
#   if (defined(HAVE_SIGBLOCK) && defined(HAVE_SIGSETMASK))
    int oldmask;
#   endif
    int num_tries;

#   if (defined(HAVE_SIGBLOCK) && defined(HAVE_SIGSETMASK))
    oldmask = sigblock(sigmask(LISTENER_ATTN_SIGNAL));
#   else
    sighold(LISTENER_ATTN_SIGNAL);
#   endif

    /* Get some initial information */
    my_id = p4_get_my_id();
    my_pi = get_proc_info(my_id);
    my_host = my_pi->host_name;
    my_listener = my_pi->port;

    dest_pi = get_proc_info(dest_id);
    dest_host = dest_pi->host_name;
    dest_listener = dest_pi->port;

    p4_dprintfl(70, "request_connection: my_id=%d my_host=%s my_listener=%d dest_id=%d dest_host=%s dest_listener=%d\n",
	    my_id, my_host, my_listener, dest_id, dest_host, dest_listener);

    /* Have we already connected?? */
    if (p4_local->conntab[dest_id].type == CONN_REMOTE_EST)
    {
	p4_dprintfl(70,"request_connection %d: already connected\n", dest_id);
#   if (defined(HAVE_SIGBLOCK) && defined(HAVE_SIGSETMASK))
	sigsetmask(oldmask);
#       else
        sigrelse(LISTENER_ATTN_SIGNAL);
#       endif
	return;
    }

    p4_dprintfl(70, "enter loop to connect to dest listener %s\n",dest_host);
    /* Connect to dest listener */
    num_tries = 1;
    p4_has_timedout( 0 );
    while((dest_listener_con_fd = net_conn_to_listener(dest_host,dest_listener,1)) == -1) {
	num_tries++;
	if (p4_has_timedout( 1 )) {
	    p4_error( "Timeout in establishing connection to remote process", 0 );
	    }
	}
    p4_dprintfl(70, "conn_to_proc_contd: connected after %d tries, dest_listener_con_fd=%d\n",num_tries, dest_listener_con_fd);


    /* Setup a listener to accept the connection to dest_id */
    net_setup_anon_listener(1, &new_listener_port, &new_listener_fd);

    /* Construct a connection request message */
    msg.type = p4_i_to_n(CONNECTION_REQUEST);
    msg.from = p4_i_to_n(my_id);
    msg.lport = p4_i_to_n(new_listener_port);
    msg.to = p4_i_to_n(dest_id);
    msg.to_pid = p4_i_to_n(dest_pi->unix_id);

    /* Send it to dest_id's listener */
    p4_dprintfl(70, "request_connection: sending CONNECTION_REQUEST to %d on fd=%d size=%d\n",
		dest_id,dest_listener_con_fd,sizeof(msg));
    net_send(dest_listener_con_fd, &msg, sizeof(msg), P4_FALSE);
    p4_dprintfl(70, "request_connection: sent CONNECTION_REQUEST to dest_listener\n");

    if (my_id < dest_id)
    {
	/* Wait for the remote process to connect to me */
	p4_dprintfl(70, "request_connection: waiting for accept from %d on fd=%d, port=%d\n",
		    dest_id, new_listener_fd, new_listener_port);

	/* This needs a timeout? ???  */
	connection_fd = net_accept(new_listener_fd);
	p4_dprintfl(70, "request_connection: accepted from %d on %d\n", dest_id, connection_fd);

	/* Add the connection to the table */
	p4_local->conntab[dest_id].port = connection_fd;
	p4_local->conntab[dest_id].same_data_rep =
	    same_data_representation(p4_local->my_id,dest_id);
	/* Requires write ordering in the thread */
	p4_local->conntab[dest_id].type = CONN_REMOTE_EST;
    }

    close(dest_listener_con_fd);
    /* Now release the listener connections */
    close(new_listener_fd);

#   if (defined(HAVE_SIGBLOCK) && defined(HAVE_SIGSETMASK))
    sigsetmask(oldmask);
#   else
    sigrelse(LISTENER_ATTN_SIGNAL);
#   endif

    p4_dprintfl(70, "request_connection: finished connecting\n");
    return;
}

/* sig isn't used except to match the function prototypes for POSIX
   signal handlers */
P4VOID handle_connection_interrupt( sig )
int sig;
{
    struct slave_listener_msg msg;
    int type;
    int listener_fd;
    int to, to_pid, from, lport;
    int connection_fd;
    struct proc_info *from_pi;
    int myid = p4_get_my_id();
    int num_tries;

    listener_fd = p4_local->listener_fd;
    p4_dprintfl(70, "Inside handle_connection_interrupt, listener_fd=%d\n",
		listener_fd);

    if (net_recv(listener_fd, &msg, sizeof(msg)) == PRECV_EOF)
    {
	p4_dprintf("OOPS: got eof in handle_connection_interrupt\n");
	return;
    }

    type = p4_n_to_i(msg.type);

    if (type == KILL_SLAVE) {
        msg.type = p4_i_to_n(IGNORE_THIS);
        p4_dprintfl(70, "handle_connection_interrupt: sending IGNORE_THIS to my_listener\n");
        /* send msg to listener indicating I made the connection */
        net_send(listener_fd, &msg, sizeof(msg), P4_FALSE);
	p4_dprintfl(99, "handle_connection_interrupt: exiting due to DIE msg\n");
	exit(0);
    }

    if (type != CONNECTION_REQUEST)
    {
	p4_dprintf("handle_connection_interrupt: invalid type %d\n", type);
	return;
    }

    to = p4_n_to_i(msg.to);
    from = p4_n_to_i(msg.from);
    to_pid = p4_n_to_i(msg.to_pid);
    lport = p4_n_to_i(msg.lport);

    p4_dprintfl(70, "handle_connection_interrupt: msg contents: to=%d from=%d to_pid=%d lport=%d\n",
		to, from, to_pid, lport);

    /* If we're already connected, forget about the interrupt. */
    if (p4_local->conntab[from].type != CONN_REMOTE_EST)
    {
	if (myid < from)
	{
	    /* see if I have already started this connection */
	    p4_dprintfl(90,"myid < from, myid = %d, from = %d\n",myid,from);
	    if (p4_global->dest_id[myid] != from)
		request_connection(from);
	}
	else
	{
	    /* Get the information for the process we're connecting to */
	    from_pi = &(p4_global->proctable[from]);

	    /* Connect to the waiting process */
	    p4_dprintfl(70, "connecting to port...\n");
	    num_tries = 1;
	    /* connect to the requesting process, who is listening */
	    p4_dprintfl(70,"handling connection interrupt: connecting to %s\n",from_pi->host_name);
	    p4_has_timedout( 0 );
	    while ((connection_fd = net_conn_to_listener(from_pi->host_name,lport,1)) == -1) {
		num_tries++;
		if (p4_has_timedout( 1 )) {
		    p4_error( "Timeout in establishing connection to remote process", 0 );
		    }
		}

	    p4_dprintfl(70, "handling connection interrupt: connected after %d tries, connection_fd=%d host = %s\n",
			num_tries, connection_fd, from_pi->host_name);

	    /* We're connected, so we can add this connection to the table */
	    p4_local->conntab[from].port = connection_fd;
	    p4_local->conntab[from].same_data_rep =
		same_data_representation(p4_local->my_id,from);
	    /* Note that this requires write ordering in the threads */
	    p4_local->conntab[from].type = CONN_REMOTE_EST;
	    p4_dprintfl(70, "marked as established fd=%d from=%d\n",
			connection_fd, from);
	}
    }
    else
    {
	p4_dprintfl(70,"ignoring interrupt from %d\n",from);
    }

    msg.type = p4_i_to_n(IGNORE_THIS);
    p4_dprintfl(70, "handle_connection_interrupt: sending IGNORE_THIS to my_listener\n");
    /* send msg to listener indicating I made the connection */
    net_send(listener_fd, &msg, sizeof(msg), P4_FALSE);
    p4_dprintfl(70, "handle_connection_interrupt: exiting handling intr from %d\n",from);
    
    /* If the return from this is SIG_DFL, then there is a problem ... */
    SIGNAL_P4(LISTENER_ATTN_SIGNAL, handle_connection_interrupt);
}

#endif /* THREAD_LISTENER */
