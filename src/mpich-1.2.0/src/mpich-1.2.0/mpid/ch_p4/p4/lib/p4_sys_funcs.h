#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

char *xx_malloc ANSI_ARGS((int, int));
char *MD_shmalloc ANSI_ARGS((int));
char *print_conn_type ANSI_ARGS((int));
char *xx_shmalloc ANSI_ARGS((unsigned));
int MD_clock ANSI_ARGS((void));
P4VOID get_qualified_hostname ANSI_ARGS((char *));
P4VOID MD_set_reference_time ANSI_ARGS((void));
P4VOID MD_initenv ANSI_ARGS((void));
P4VOID MD_initmem ANSI_ARGS((int));
P4VOID MD_malloc_hint ANSI_ARGS((int,int));
P4VOID MD_shfree ANSI_ARGS((char *));
/* P4VOID MD_start_cube_slaves ANSI_ARGS(( )); */
int bm_start ANSI_ARGS((int *, char **))	;
/* P4VOID compute_conntab ANSI_ARGS(( )); */
#ifdef THREAD_LISTENER
int request_connection ANSI_ARGS((int));
#else
P4VOID request_connection ANSI_ARGS((int));
#endif
int create_bm_processes ANSI_ARGS((struct p4_procgroup *));
P4VOID net_slave_info ANSI_ARGS((struct p4_procgroup_entry *, char *, 
				 int, int))	;
int create_remote_processes ANSI_ARGS((struct p4_procgroup *));
P4VOID create_rm_processes ANSI_ARGS((int, int))	;
P4VOID dump_conntab ANSI_ARGS(( int ));
P4VOID dump_global ANSI_ARGS((int))	;
P4VOID dump_listener ANSI_ARGS((int))	;
P4VOID dump_local ANSI_ARGS((int))	;
P4VOID dump_procgroup ANSI_ARGS((struct p4_procgroup *, int))	;
P4VOID dump_tmsg ANSI_ARGS((struct p4_msg *));
int p4_has_timedout ANSI_ARGS(( int ));
int establish_connection ANSI_ARGS((int));
int p4_establish_all_conns ANSI_ARGS((void));
/* P4VOID exec_pgm ANSI_ARGS(( )); */
P4VOID put_execer_port ANSI_ARGS((int));
int get_execer_port ANSI_ARGS((char *));
int fork_p4 ANSI_ARGS((void));
P4VOID free_p4_msg ANSI_ARGS((struct p4_msg *));
P4VOID free_quel ANSI_ARGS((struct p4_queued_msg *));
P4VOID get_inet_addr ANSI_ARGS((struct in_addr *));
P4VOID get_inet_addr_str ANSI_ARGS((char *));
void p4_print_sock_params( int skt );
#if !defined(CRAY)
P4VOID dump_sockaddr ANSI_ARGS(( char *, struct sockaddr_in *));
P4VOID dump_sockinfo ANSI_ARGS((char *, int));
#else
#endif
P4VOID get_pipe ANSI_ARGS((int *, int *))	;
int getswport ANSI_ARGS((char *));
P4VOID handle_connection_interrupt ANSI_ARGS((int));
P4BOOL shmem_msgs_available ANSI_ARGS((void));
P4BOOL socket_msgs_available ANSI_ARGS((void));
P4BOOL MD_tcmp_msgs_available ANSI_ARGS((int *, int *));
P4BOOL MD_i860_msgs_available ANSI_ARGS((void));
P4BOOL MD_CM5_msgs_available ANSI_ARGS((void));
P4BOOL MD_NCUBE_msgs_available ANSI_ARGS((void));
P4BOOL MD_euih_msgs_available ANSI_ARGS((void));
int MD_i860_send ANSI_ARGS((struct p4_msg *));
int MD_CM5_send ANSI_ARGS((struct p4_msg *));
int MD_NCUBE_send ANSI_ARGS((struct p4_msg *));
int MD_eui_send ANSI_ARGS((struct p4_msg *));
int MD_euih_send ANSI_ARGS((struct p4_msg *));
struct p4_msg *MD_i860_recv ANSI_ARGS((void));
struct p4_msg *MD_CM5_recv ANSI_ARGS((void));
struct p4_msg *MD_NCUBE_recv ANSI_ARGS((void));
struct p4_msg *MD_eui_recv ANSI_ARGS((void));
struct p4_msg *MD_euih_recv ANSI_ARGS((void));
P4BOOL in_same_cluster ANSI_ARGS((int,int));
P4VOID p4_cluster_shmem_sync ANSI_ARGS((P4VOID **));
/* P4VOID **cluster_shmem; */
P4VOID init_avail_buffs ANSI_ARGS((void));
P4VOID free_avail_buffs ANSI_ARGS((void));
P4VOID initialize_msg_queue ANSI_ARGS((struct p4_msg_queue *));
int install_in_proctable ANSI_ARGS((int,int,int,char *,char *,int,char *,int));
/* P4VOID kill_server ANSI_ARGS(( )); */
P4VOID listener ANSI_ARGS((void));
P4VOID thread_listener ANSI_ARGS((void));
struct listener_data *alloc_listener_info ANSI_ARGS((void));
struct local_data *alloc_local_bm ANSI_ARGS((void));
struct local_data *alloc_local_listener ANSI_ARGS((void));
struct local_data *alloc_local_rm ANSI_ARGS((void));
struct local_data *alloc_local_slave ANSI_ARGS((void));
int myhost ANSI_ARGS((void));
int net_accept ANSI_ARGS((int))	;
/* int net_conn_to_addr_listener ANSI_ARGS(( ))	; */
int net_conn_to_listener ANSI_ARGS((char *, int, int ))	;
/* int net_conn_to_named_listener ANSI_ARGS(( ))	; */
int net_create_slave ANSI_ARGS((int, int, char *, char *, char *));
int net_recv ANSI_ARGS(( int, void *, int))	;
int net_recv_timeout ANSI_ARGS(( int, void *, int, int))	;
int net_send ANSI_ARGS(( int, void *, int, int ))	;
P4VOID net_setup_anon_listener ANSI_ARGS((int, int *, int *))	;
P4VOID net_setup_listener ANSI_ARGS((int, int, int *))	;
/* P4VOID net_setup_named_listener ANSI_ARGS(( ))	; */
P4VOID net_set_sockbuf_size ANSI_ARGS(( int, int ));
void get_sock_info_by_hostname ANSI_ARGS(( char *, struct sockaddr_in ** ));
int num_in_mon_queue ANSI_ARGS((p4_monitor_t *,int));
P4VOID alloc_global ANSI_ARGS((void));
struct p4_msg *alloc_p4_msg ANSI_ARGS((int));
struct p4_msg *get_tmsg ANSI_ARGS((int, int, int, char *, int, int, int, int));
struct p4_msg *recv_message ANSI_ARGS((int *, int *));
struct p4_msg *search_p4_queue ANSI_ARGS((int, int, P4BOOL));
struct p4_msg *shmem_recv ANSI_ARGS((void));
struct p4_msg *socket_recv ANSI_ARGS((int));
struct p4_msg *socket_recv_on_fd ANSI_ARGS((int));
struct p4_queued_msg *alloc_quel ANSI_ARGS((void));
P4VOID free_avail_quels ANSI_ARGS((void));
P4VOID process_args ANSI_ARGS((int *, char **))	;
P4BOOL process_connect_request ANSI_ARGS((int))	;
/* int process_connection ANSI_ARGS(( )); */
P4BOOL process_slave_message ANSI_ARGS((int))	;
struct p4_procgroup *alloc_procgroup ANSI_ARGS((void));
struct p4_procgroup *read_procgroup ANSI_ARGS((void))	;
P4VOID procgroup_to_proctable ANSI_ARGS((struct p4_procgroup *));
P4VOID queue_p4_message ANSI_ARGS((struct p4_msg *, struct p4_msg_queue *));
/* P4VOID reaper ANSI_ARGS(( )); */
P4VOID receive_proc_table ANSI_ARGS((int))	;
/* int rm_newline ANSI_ARGS(( )); */
int rm_start ANSI_ARGS((int *, char **));
P4VOID send_ack ANSI_ARGS((int,int))	;
P4VOID sync_with_remotes ANSI_ARGS((void));
P4VOID send_proc_table ANSI_ARGS((void));
P4VOID setup_conntab ANSI_ARGS((void));
int shmem_send ANSI_ARGS((struct p4_msg *));
P4VOID shutdown_p4_socks ANSI_ARGS((void));
#if defined(GP_1000) || defined(TC_2000)
int simple_lock ANSI_ARGS((int *));
int simple_unlock ANSI_ARGS((int *));
P4VOID waitspin ANSI_ARGS((int));
#endif
P4BOOL sock_msg_avail_on_fd ANSI_ARGS((int));
int socket_send ANSI_ARGS((int,int,int,char *,int,int,int));
int socket_close_conn ANSI_ARGS((int));
int start_slave ANSI_ARGS((char *, char *, char *, int, char *, 
			   char *(*)(char *, char *)));
int subtree_broadcast_p4 ANSI_ARGS((int, int, void *,int,int));
P4VOID trap_sig_errs ANSI_ARGS((void));
P4VOID wait_for_ack ANSI_ARGS((int))	;
int xdr_recv ANSI_ARGS((int, struct p4_msg *));
int xdr_send ANSI_ARGS((int, int, int, char *, int, int, int));
int data_representation ANSI_ARGS((char *));
P4BOOL same_data_representation ANSI_ARGS((int, int));
P4VOID xx_init_shmalloc ANSI_ARGS((char *, unsigned));
P4VOID xx_shfree ANSI_ARGS((char *));
P4VOID zap_p4_processes ANSI_ARGS((void));
P4VOID zap_remote_p4_processes ANSI_ARGS((void));
struct p4_msg *MD_tcmp_recv ANSI_ARGS((void));
int MD_tcmp_send ANSI_ARGS((int, int, int, char *, int, int, int));
struct hostent *gethostbyname_p4 ANSI_ARGS(( char *));
char *getpw_ss ANSI_ARGS((char *, char * ));

P4VOID p4_dprint_last ANSI_ARGS(( FILE * ));

#ifdef CHECK_SIGNALS
P4VOID p4_CheckSighandler ANSI_ARGS(( ));
#else
#define p4_CheckSighandler( a )
#endif

#ifdef SYSV_IPC
int init_sysv_semset ANSI_ARGS((int));
P4VOID MD_lock_init ANSI_ARGS((MD_lock_t *));
P4VOID MD_lock ANSI_ARGS((MD_lock_t *));
P4VOID MD_unlock ANSI_ARGS((MD_lock_t *));
P4VOID remove_sysv_ipc ANSI_ARGS((void));
#endif

#if defined(NEEDS_STDLIB_PROTOTYPES)
/* Some gcc installations have out-of-date include files and need these
   definitions to handle the "missing" prototypes.  This is NOT
   autodetected, but is provided and can be selected by using a switch
   on the options line.

   These are from stdlib.h, stdio.h, and unistd.h
 */
extern int fprintf(FILE*,const char*,...);
extern int printf(const char*,...);
extern int fflush(FILE *);
extern int fclose(FILE *);
extern int fscanf(FILE *,char *,...);
extern int sscanf(char *, char *, ... );
extern int pclose (FILE *);

/* String.h */
extern void bzero( void *, size_t );
extern void bcopy( const void *, void *, size_t);
extern int bcmp( const void *, const void *, size_t);


extern int gettimeofday( struct timeval *, struct timezone *);
extern void perror(const char *);
extern int  gethostname( char *, int );
extern int  getdtablesize(void);

/* time.h */
extern time_t   time (time_t *);

/* signal.h */
extern int sigblock (int);
extern int sigsetmask (int);

/* sys/socket.h */
extern int socket( int, int, int );
extern int bind( int, const struct sockaddr *, int);
extern int listen( int, int );
extern int accept( int, struct sockaddr *, int *);
extern int connect( int, const struct sockaddr *, int);
extern int socketpair( int, int, int, int[2] );
extern int setsockopt( int, int, int, const void *, int );
extern int getsockopt(int, int, int, void *, int *);
extern int getsockname(int, struct sockaddr *, int *);
extern int getpeername(int, struct sockaddr *, int *);
extern int recv(int, void *, int, int);
extern int shutdown(int, int);

/* netinet/in.h or arpa/inet.h */
extern char *inet_ntoa(struct in_addr);

/* sys/select.h */
int select (int, fd_set *, fd_set *, fd_set *, struct timeval *);

/* sys/time.h */
extern int setitimer (int, struct itimerval *, struct itimerval *);

/* unistd.h */
extern char *getlogin(void);

#ifdef FOO
/* Unistd.h */
extern int  atoi(const char *);
extern int  unlink(const char *);
extern int  close(int);
extern int  read(int, void *, size_t);
extern int  write(int, const void *, size_t);


extern int  getpid(void);
extern int  getppid(void);
extern int  getuid(void);
extern char *getlogin(void);

extern int setitimer( int, const struct itimerval *, struct itimerval *);
extern int sleep(unsigned int);

extern int execlp( const char *, const char *, ... );
extern int fork (void);

#endif
#endif
