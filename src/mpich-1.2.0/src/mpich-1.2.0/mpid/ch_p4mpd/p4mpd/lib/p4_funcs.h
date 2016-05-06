/* to import correctly for c++ users */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

double p4_usclock ANSI_ARGS((void));
P4VOID init_usclock ANSI_ARGS((void));
char *p4_shmalloc ANSI_ARGS((int));
P4VOID p4_set_avail_buff ANSI_ARGS((int,int));
P4BOOL p4_am_i_cluster_master ANSI_ARGS((void));
int p4_askfor ANSI_ARGS((p4_askfor_monitor_t *, int, int (*)(P4VOID *), 
			 P4VOID *, P4VOID (*)(void) ));
int p4_askfor_init ANSI_ARGS((p4_askfor_monitor_t *));
P4VOID p4_barrier ANSI_ARGS((p4_barrier_monitor_t *, int));
int p4_barrier_init ANSI_ARGS((p4_barrier_monitor_t *));
int p4_broadcastx ANSI_ARGS((int, void *, int, int));
int p4_clock ANSI_ARGS(( void ));
int p4_create ANSI_ARGS(( int (*)(void) ));
int p4_create_procgroup ANSI_ARGS((void));
int p4_startup ANSI_ARGS((struct p4_procgroup *));
#if defined(DELTA)  ||  defined(NCUBE)
P4VOID p4_dprintf ANSI_ARGS((int,int,int,int,int,int,int,int,int,int));
#if defined(P4_DPRINTFL)
P4VOID p4_dprintfl ANSI_ARGS((int,int,int,int,int,int,int,int,int,int,int));
#endif
#else
#if defined(USE_STDARG)
P4VOID p4_dprintf ANSI_ARGS((char *, ...));
#if defined(P4_DPRINTFL)
P4VOID p4_dprintfl ANSI_ARGS((int, char *, ... ));
#endif
#else
P4VOID p4_dprintf ANSI_ARGS(());
#if defined(P4_DPRINTFL)
P4VOID p4_dprintfl ANSI_ARGS(());
#endif
#endif /* USE_STDARG */
#endif /* DELTA ect */
P4VOID p4_error ANSI_ARGS((char *, int));
P4VOID p4_set_hard_errors ANSI_ARGS((int));
P4VOID p4_global_barrier ANSI_ARGS((int));
P4VOID p4_get_cluster_masters ANSI_ARGS((int *, int *));
P4VOID p4_get_cluster_ids ANSI_ARGS((int *, int *));
int p4_get_my_cluster_id ANSI_ARGS((void));
int p4_get_my_id ANSI_ARGS((void));
int p4_get_my_id_from_proc ANSI_ARGS((void));
int p4_getsub_init ANSI_ARGS((p4_getsub_monitor_t *));
P4VOID p4_getsubs ANSI_ARGS((p4_getsub_monitor_t *, int *, int, int, int));
int p4_global_op ANSI_ARGS((int, void *, int, int, 
			    P4VOID (*)(char *, char *,int), int));
int p4_initenv ANSI_ARGS((int *, char **));
P4VOID p4_post_init ANSI_ARGS(( void ));
P4VOID p4_int_absmax_op ANSI_ARGS((char *, char *, int));
P4VOID p4_int_absmin_op ANSI_ARGS((char *, char *, int));
P4VOID p4_int_max_op ANSI_ARGS((char *, char *, int));
P4VOID p4_int_min_op ANSI_ARGS((char *, char *, int));
P4VOID p4_int_mult_op ANSI_ARGS((char *, char *, int));
P4VOID p4_int_sum_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_absmax_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_absmin_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_max_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_min_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_mult_op ANSI_ARGS((char *, char *, int));
P4VOID p4_dbl_sum_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_sum_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_absmax_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_absmin_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_max_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_min_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_mult_op ANSI_ARGS((char *, char *, int));
P4VOID p4_flt_sum_op ANSI_ARGS((char *, char *, int));
P4VOID p4_mcontinue ANSI_ARGS((p4_monitor_t *, int));
P4VOID p4_mdelay ANSI_ARGS((p4_monitor_t *, int));
P4VOID p4_menter ANSI_ARGS((p4_monitor_t *));
P4BOOL p4_messages_available ANSI_ARGS((int *, int *));
P4BOOL p4_any_messages_available ANSI_ARGS((void));
P4VOID p4_mexit ANSI_ARGS((p4_monitor_t *));
int p4_moninit ANSI_ARGS((p4_monitor_t *, int));
P4VOID p4_msg_free ANSI_ARGS((char *));
char *p4_msg_alloc ANSI_ARGS((int));
int p4_num_cluster_ids ANSI_ARGS((void));
int p4_num_total_ids ANSI_ARGS((void));
int p4_num_total_slaves ANSI_ARGS((void));
P4VOID p4_probend ANSI_ARGS((p4_askfor_monitor_t *, int));
P4VOID p4_progend ANSI_ARGS((p4_askfor_monitor_t *));
int p4_recv ANSI_ARGS((int *, int *, char **, int *));
int p4_get_dbg_level ANSI_ARGS((void));
P4VOID p4_set_dbg_level ANSI_ARGS((int));
P4VOID p4_shfree ANSI_ARGS((P4VOID *));
int p4_soft_errors ANSI_ARGS((int));
P4VOID p4_update ANSI_ARGS((p4_askfor_monitor_t *, int (*)(void *), 
			    P4VOID * ));
char *p4_version ANSI_ARGS((void));
char *p4_machine_type ANSI_ARGS((void));
int p4_wait_for_end ANSI_ARGS((void));
int p4_proc_info ANSI_ARGS(( int, char **, char ** ));
P4VOID p4_print_avail_buffs ANSI_ARGS((void));
struct p4_procgroup *p4_alloc_procgroup ANSI_ARGS((void));
/* The p4 send routines are actually macros that use this routine */
int send_message ANSI_ARGS((int, int, int, char *, int, int, P4BOOL, P4BOOL));

#ifdef __cplusplus
};
#endif

