#include "p4.h"
#include "p4_sys.h"

typedef long P4_Aint;

#if !defined(NEXT) && !defined(HAVE_STDLIB_H)
/* Note Sun 4.1.3 defines exit as int exit(int), even though DOCUMENTED as
   void exit(int).  Try to use the definition in stdlib.h instead */
extern P4VOID exit ANSI_ARGS((int));
#endif

/*
 * Some systems provide prototypes for the definitions SIG_IGN and SIG_DFL
 * only if some additional defs (like -D_ANSI_SOURCE under FreeBSD) are
 * supplied.  If you really need a completely clean compile, consider
 * adding these defs to the user cflags.
 */

static int interrupt_caught = 0; /* True if an interrupt was caught */

int p4_hard_errors = 1;

#if defined(ENCORE) || defined(SYMMETRY) || defined(TITAN) || \
    defined(SGI)    || defined(GP_1000)  || defined(TC_2000) || defined(SUN_SOLARIS)
#define P4_HANDLER_TYPE int
#else
#define P4_HANDLER_TYPE P4VOID
#endif
#if defined(__STDC__) 
#define HANDLER_ARG int
#else
#define HANDLER_ARG
#endif

#if defined(HAVE_FOUR_ARG_SIGS)
#define HANDLER_ARGS int,int,int,int
#else
#define HANDLER_ARGS int
#endif

static P4_HANDLER_TYPE (*prev_sigint_handler) ANSI_ARGS((HANDLER_ARGS)) = NULL;
static P4_HANDLER_TYPE (*prev_sigsegv_handler) ANSI_ARGS((HANDLER_ARGS)) = NULL;
static P4_HANDLER_TYPE (*prev_sigbus_handler) ANSI_ARGS((HANDLER_ARGS)) = NULL;
static P4_HANDLER_TYPE (*prev_sigfpe_handler) ANSI_ARGS((HANDLER_ARGS)) = NULL;
static P4_HANDLER_TYPE (*prev_err_handler) ANSI_ARGS((HANDLER_ARGS)) = NULL;
static int err_sig;
#if defined(HAVE_FOUR_ARG_SIGS)
static int                err_code;
static struct sigcontext *err_scp;
static char              *err_addr;
#endif

int p4_soft_errors(onoff)
int onoff;
{
    int old;

    if (!p4_local)
	p4_error("p4_soft_errors: p4_local must be allocated first", 0);

    old = p4_local->soft_errors;
    p4_local->soft_errors = onoff;
    return old;
}

P4VOID p4_error(string, value)
char *string;
int value;
{
    char job_filename[64];
    static int in_p4_error = 0;

    if (in_p4_error) {
	/* Recursive call - emergency stop */
	exit(1);
    }
    in_p4_error = 1;

    /* If the following line generates a warning about prototypes,
       see the comment at the head of the file */
    SIGNAL_P4(SIGINT,SIG_IGN);
    fflush(stdout);
#ifdef USE_PTHREADS
    printf("%s: %u:  p4_error: %s: %d\n",whoami_p4,pthread_self(),
	   string,value);
#else
    printf("%s:  p4_error: %s: %d\n",whoami_p4,string,value);
#endif
    if (value < 0)
        perror("    p4_error: latest msg from perror");
    fflush(stdout);

#ifdef USE_PRINT_LAST_ON_ERROR
	p4_dprint_last( stderr );
#endif

    /* Send interrupt to all known processes */
    zap_p4_processes();

    /* Send kill-clients message to all known listeners */
#ifdef FOOGLE
    /* Still being debugged */
    if (p4_get_my_id() != p4_global->listener_pid)   /* if I am not the listener */
	zap_remote_p4_processes();
#endif
    /* shutdown(sock,2), close(sock) all sockets */
#   ifdef CAN_DO_SOCKET_MSGS
    shutdown_p4_socks();
#   endif

#   ifdef SYSV_IPC
    remove_sysv_ipc();
#   endif

#   if defined(SGI)  &&  defined(VENDOR_IPC)
    unlink(p4_sgi_shared_arena_filename);
#   endif

    if (execer_starting_remotes  &&  execer_mynodenum == 0)
    {
	strcpy(job_filename,"/tmp/p4_");
	strcat(job_filename,execer_jobname);
	unlink(job_filename);
    }

    if (interrupt_caught && value != SIGINT)
    {
	switch (value)
	{
	  case SIGSEGV:
	    prev_err_handler = prev_sigsegv_handler;
	    break;
	  case SIGBUS:
	    prev_err_handler = prev_sigbus_handler;
	    break;
	  case SIGFPE:
	    prev_err_handler = prev_sigfpe_handler;
	    break;
	  default:
	    printf("p4_error: unidentified err handler (signal %d)\n", value );
	    prev_err_handler = NULL;
	    break;
	}
	if (prev_err_handler == (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))) NULL)
	{
	    /* return to default handling of the interrupt by the OS */
	    SIGNAL_P4(value,SIG_DFL); 
#           if defined(NEXT)  ||  defined(KSR)
            kill(getpid(),value);
#           endif
	    /* This is really a fatal error, so ensure that we don't get 
	       any farther */
	    exit( 1 );
	    return;
	}
	else
	{
#if defined(HAVE_FOUR_ARG_SIGS)
	    (*prev_err_handler) (err_sig, err_code, err_scp, err_addr);
#else
	    (*prev_err_handler) (err_sig);
#endif
	}
    }
    else
    {

#       if defined(SP1_EUI)
	mpc_stopall(value);
#       endif
	exit(1);
    }
}

/* static P4_HANDLER_TYPE sig_err_handler(sig, code, scp, addr) */

#if defined(HAVE_FOUR_ARG_SIGS)
static P4VOID sig_err_handler(sig, code, scp, addr)
int sig, code;
struct sigcontext *scp;
char *addr;
#else
static P4VOID sig_err_handler(sig)
int sig;
#endif
{
    interrupt_caught = 1;
    err_sig = sig;
#if defined(HAVE_FOUR_ARG_SIGS)
    err_code = code;
    err_scp = scp;
    err_addr = addr;
#endif
    p4_dprintfl(90,"sig_err_handler: sig = %d\n", sig);
    if (sig == SIGSEGV)
	p4_error("interrupt SIGSEGV", sig);
    else if (sig == SIGBUS)
	p4_error("interrupt SIGBUS", sig);
    else if (sig == SIGFPE)
	p4_error("interrupt SIGFPE", sig);
    else if (sig == SIGINT) {
#if defined(USE_PRINT_LAST_ON_SIGINT) && ! defined(USE_PRINT_LAST_ON_ERROR)
	p4_dprint_last( stderr );
#endif
	p4_error("interrupt SIGINT", sig);
    }
    else
	p4_error("interrupt SIGx", sig);
    /* return( (P4_HANDLER_TYPE) NULL); */

    interrupt_caught = 0;
}


/*
  Trap signals so that we can propagate error conditions and tidy up 
  shared system resources in a manner not possible just by killing procs
*/
P4VOID trap_sig_errs()
{
    P4_HANDLER_TYPE (*rc) ANSI_ARGS((HANDLER_ARGS));

/*     rc = (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))  */
	SIGNAL_WITH_OLD_P4(SIGINT, sig_err_handler,
                           rc= (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))));
    if (rc == (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))) -1)
	p4_error("trap_sig_errs: SIGNAL_P4 failed", SIGINT);
    if (((P4_Aint) rc > 1)  &&  ((P4_Aint) rc != (P4_Aint) sig_err_handler))
	prev_sigint_handler = rc;

/* we can not handle sigsegv on symmetry and balance because they use 
 * it for shmem stuff 
*/

#ifdef CAN_HANDLE_SIGSEGV
/*     rc = (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS)))  */
	SIGNAL_WITH_OLD_P4(SIGSEGV, sig_err_handler, 
                           rc= (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))));
    if ((P4_Aint) rc == -1)
	p4_error("trap_sig_errs: SIGNAL_P4 failed", SIGSEGV);
    if (((P4_Aint) rc > 1)  &&  ((P4_Aint) rc != (P4_Aint) sig_err_handler))
	prev_sigsegv_handler = rc;
#endif

/*    rc = (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS)))  */
	SIGNAL_WITH_OLD_P4(SIGBUS, sig_err_handler,
                 rc= (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))));
    if ((P4_Aint) rc == -1)
	p4_error("trap_sig_errs: SIGNAL_P4 failed", SIGBUS);
    if (((P4_Aint) rc > 1)  &&  ((P4_Aint) rc != (P4_Aint) sig_err_handler))
	prev_sigbus_handler = rc;
/*    rc = (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))) */
	SIGNAL_WITH_OLD_P4(SIGFPE, sig_err_handler,
                           rc= (P4_HANDLER_TYPE (*) ANSI_ARGS((HANDLER_ARGS))));
    if ((P4_Aint) rc == -1)
	p4_error("trap_sig_errs: SIGNAL_P4 failed", SIGFPE);
    if (((P4_Aint) rc > 1)  &&  ((P4_Aint) rc != (P4_Aint) sig_err_handler))
	prev_sigfpe_handler = rc;
}

P4VOID p4_set_hard_errors( flag )
int flag;
{
    p4_hard_errors = flag;
}
