#if ! defined( _SLOG )
#define _SLOG

#include <stdio.h>
#include "fbuf.h"

/* the high-level API for reading and writing SLOG files */

#define VERSION_MainID    0
#define VERSION_SubID     7

/*
     Define the basic data types used, SLOG_xxx,
     and their printing formats, fmt_xxx, used in fprintf().
*/
typedef  int                 SLOG_int32 ;
#define  fmt_i32             "%d"

typedef  unsigned char       SLOG_bit   ;
#define  fmt_bit             "%hu"
typedef  unsigned char       SLOG_uint8 ;
#define  fmt_ui8             "%hu"
typedef  unsigned short      SLOG_uint16;
#define  fmt_ui16            "%hu"
typedef  unsigned int        SLOG_uint32;
#define  fmt_ui32            "%u"
typedef  unsigned long long  SLOG_uint64;
#define  fmt_ui64            "%llu"
typedef  double              SLOG_time  ;
#define  fmt_time            "%21.17f"
typedef  SLOG_uint64         SLOG_fptr  ;
#define  fmt_fptr            fmt_ui64
typedef  double              SLOG_statbintype;
#define  fmt_sbin            "%13.2f"

/*
     Define the components and their corresponding print format 
     in the __internal__ Interval Record
*/
typedef  SLOG_uint16         SLOG_bytesize_t;
#define  fmt_sz_t            fmt_ui16
typedef  SLOG_uint16         SLOG_rectype_t;
#define  fmt_rtype_t         fmt_ui16
typedef  SLOG_uint16         SLOG_intvltype_t;
#define  fmt_itype_t         fmt_ui16
typedef  SLOG_time           SLOG_starttime_t;
#define  fmt_stime_t         fmt_time
typedef  SLOG_time           SLOG_duration_t;
#define  fmt_dura_t          fmt_time
typedef  SLOG_bit            SLOG_bebit_t;
#define  fmt_bebit_t         fmt_bit
typedef  SLOG_bit            SLOG_bebits_t;
#define  fmt_bebits_t        fmt_bit
typedef  SLOG_uint16         SLOG_nodeID_t; 
#define  fmt_nodeID_t        fmt_ui16
typedef  SLOG_uint8          SLOG_cpuID_t; 
#define  fmt_cpuID_t         fmt_ui8
typedef  SLOG_uint32         SLOG_threadID_t;
#define  fmt_thID_t          fmt_ui32
typedef  SLOG_fptr           SLOG_where_t;
#define  fmt_where_t         fmt_fptr

typedef  SLOG_uint16         SLOG_N_assocs_t;
#define  fmt_Nassocs_t       fmt_ui16
typedef  SLOG_uint32         SLOG_assoc_t;
#define  fmt_assoc_t         fmt_ui32

typedef  SLOG_uint16         SLOG_N_vtrargs_t;
#define  fmt_Nvtrargs_t      fmt_ui16
typedef  SLOG_uint16         SLOG_N_args_t;
#define  fmt_Nargs_t         fmt_ui16
typedef  SLOG_uint32         SLOG_arg_t;
#define  fmt_arg_t           fmt_ui32

/*
     Define the components and their corresponding print format 
     in the __internal__ Thread Table
*/
typedef  SLOG_uint32         SLOG_OSprocessID_t;
#define  fmt_OSprocID_t      fmt_ui32
typedef  SLOG_uint32         SLOG_OSthreadID_t;
#define  fmt_OSthID_t        fmt_ui32
typedef  SLOG_uint32         SLOG_appID_t;
#define  fmt_appID_t         fmt_ui32

#define SLOG_fptr_NULL           ( (SLOG_fptr) 0 )
#define SLOG_time_NULL           -1.0
#define SLOG_MAX_TYPES           40  /* Number of types in SLOG_typesz       */
#define SLOG_STRING_LEN          21

/*  default value for some internal data structures  */
#define SLOG_BUFFER_UNIT         256 /* byte size for frame buffer           */
#define SLOG_NDirEntry           128 /* Number of entries in Frame Directory */
#define SLOG_FRAME_RESERVED_SIZE 0   /* byte size for frame reserved         */
#define SLOG_PSTAT_STATSIZE      128 /* default size for Preview Statistics  */
#define SLOG_PSTAT_STATSIZE_MAX  512 /* Maximum size for Preview Statistics  */

/*  High level API and internal subroutine return error code  */
#define SLOG_SUCCESS       1
#define SLOG_FAIL          -1
#define SLOG_EOF           -2
#define SLOG_TRUE          1
#define SLOG_FALSE         0

/*  
    The defined RecType for various record types
    "DYNAMIC" means _variable_size_, determined _dynamically_at_runtime_
    "STATIC"  means _fixed_size_, determined _statically_by_record_def_table
    "DIAG"    means _Diagonal_in_TimeLines_Labels_space,
              i.e. Start TimeLine label === End TimeLine label
    "OFFDIAG" means _Off-Diagonal_in_TimeLines_Labels_space
              i.e. Start TimeLine label =/= End TimeLine label

    Assume Rectype is an 8 bit unsigned integer, 
      0 <= rtype <=  63 :     DIAG, DYNAMIC
     64 <= rtype <= 127 :     DIAG, STATIC
    128 <= rtype <= 191 : OFF-DIAG, DYNAMIC
    192 <= rtype <= 255 : OFF-DIAG, STATIC
*/
#define SLOG_RECTYPE_DYNAMIC_DIAG          0        /* variable size state */
#define SLOG_RECTYPE_STATIC_DIAG          64        /* fixed size state    */
#define SLOG_RECTYPE_DYNAMIC_OFFDIAG     128        /* variable size arrow */
#define SLOG_RECTYPE_STATIC_OFFDIAG      192        /* fixed size arrow    */

/*  Define User acroym for user convenience  */
#define  SLOG                     SLOG_STREAM *
#define  SLOG_Irec                SLOG_intvlrec_t *

/* ***  GLOBAL DATA TYPES  *** */

/* File descriptor for the output stream */
#define  outfile  stdout 
/* File descriptor for the error stream */
#define  errfile  stderr

enum SLOG_DataType { 
     bit, ui8, ui16, ui32, ui64, ts, fptr, sbin,
     sz_t, rtype_t, itype_t, stime_t, dura_t,
     bebit_t, bebits_t, nodeID_t, cpuID_t, thID_t, taskID_t, where_t, 
     Nassocs_t, assoc_t, Nargs_t, arg_t,
     Header, Profile, Preview, ThreadTable, RecDefs,
     FrameDirHdr, FrameDirEntry, FrameHdr, min_IntvlRec
};

/*  Initialized in SLOG_InitGlobalData()  */
extern SLOG_uint32 SLOG_typesz[ SLOG_MAX_TYPES ];

/*   A global STATIC data structure   */
typedef struct {
    SLOG_rectype_t         MinRectype4VarDiagRec;
    SLOG_rectype_t         MaxRectype4VarDiagRec;
    SLOG_rectype_t         MinRectype4FixDiagRec;
    SLOG_rectype_t         MaxRectype4FixDiagRec;
    SLOG_rectype_t         MinRectype4VarOffdRec;
    SLOG_rectype_t         MaxRectype4VarOffdRec;
    SLOG_rectype_t         MinRectype4FixOffdRec;
    SLOG_rectype_t         MaxRectype4FixOffdRec;
} SLOG_global_t;



typedef struct {
    SLOG_fptr              file_loc;
    filebuf_t             *fbuf;
    SLOG_uint32            version[2];
    SLOG_uint32            frame_bytesize;
    SLOG_uint32            frame_reserved_size;
    SLOG_uint32            max_Ndirframe;
    SLOG_uint16            IsIncreasingStarttime;
    SLOG_uint16            IsIncreasingEndtime;
    SLOG_uint16            HasReserveSpaceBeenUsed;
    SLOG_fptr              fptr2statistics;
    SLOG_fptr              fptr2preview;
    SLOG_fptr              fptr2profile;
    SLOG_fptr              fptr2threadtable;
    SLOG_fptr              fptr2recdefs;
    SLOG_fptr              fptr2framedata;
} SLOG_hdr_t;

typedef struct {
    SLOG_uint32            Nchar;
    char                  *filename;
    SLOG_uint32            Nbytes;
    char                  *data;
} SLOG_pview_t;

typedef struct {
    SLOG_rectype_t         rectype;
    SLOG_intvltype_t       intvltype;
    char                   label[ SLOG_STRING_LEN ];
    SLOG_uint32            Nbin;
    SLOG_statbintype      *bins;                 /* bins[ Nbin ]  */
} SLOG_statset_t;

/*
    The starttime and endtime here are times collected right after the 1st pass
    So they may be different from those found in Frame Directory
*/
typedef struct {
    SLOG_time              starttime;            /* stime used in Write API  */
    SLOG_time              endtime;              /* etime used in Write API  */
    SLOG_time              bin_width_in_time;    /* width sued in Write API  */
    SLOG_uint32            Nset;
    SLOG_uint32            Nbin;
    SLOG_statset_t       **sets;                 /* sets[ Nset ]  */
    SLOG_int32            *seq_idx_vals;         /* seq_idx_vals[ Nset ]   */
} SLOG_pstat_t;

typedef struct {
    SLOG_intvltype_t       intvltype;
    char                   classtype[ SLOG_STRING_LEN ];
    char                   label[ SLOG_STRING_LEN ];
    char                   color[ SLOG_STRING_LEN ];
    int                    used;                 /* Write API only */
} SLOG_intvlinfo_t;

typedef struct {
    SLOG_uint32            capacity;
    SLOG_uint32            increment;
    SLOG_uint32            Nentries;
    SLOG_intvlinfo_t      *entries;              /* entries[ Nentries ]  */
    SLOG_fptr              file_loc;             /* fptr to entries[ii]  */
} SLOG_prof_t;

/*   The def'n of thread table entry   */
typedef struct {
    SLOG_nodeID_t          node_id;
    SLOG_threadID_t        thread_id;
    SLOG_OSprocessID_t     OSprocess_id;
    SLOG_OSthreadID_t      OSthread_id;
    SLOG_appID_t           app_id;
} SLOG_threadinfo_t;

/*   The def'n of the thread table entry  */
typedef struct {
    SLOG_uint32            capacity;
    SLOG_uint32            increment;
    SLOG_uint32            Nentries;
    SLOG_threadinfo_t     *entries;              /* entries[ Nentries ]  */
} SLOG_threadinfo_table_t;

/*   The def'n of the Fixed Record definition  */
typedef struct {
    SLOG_intvltype_t       intvltype;
    SLOG_bebits_t          bebits[2];
    SLOG_N_assocs_t        Nassocs;
    SLOG_N_args_t          Nargs;
    int                    used;                 /* Write API only */
} SLOG_recdef_t;

/*   The def'n of the Fixed Record definition Table   */
typedef struct {
    SLOG_uint32            capacity;
    SLOG_uint32            increment;
    SLOG_uint32            Nentries;
    SLOG_recdef_t         *entries;              /* entries[ Nentries ]  */
    SLOG_fptr              file_loc;             /* fptr to entries[ii]  */
} SLOG_recdefs_table_t;


/*   The def'n of the Entry of Frame Directory   */
typedef struct {
    SLOG_fptr              fptr2framehdr;
    SLOG_time              starttime;
    SLOG_time              endtime;
} SLOG_dir_entry_t;


/*   The def'n of the Frame Directory   */
typedef struct {
    SLOG_fptr              prevdir;
    SLOG_fptr              nextdir;
    SLOG_uint32            Nframe;
    SLOG_dir_entry_t      *entries;           /*  entries[ SLOG_NDirEntry ]  */
} SLOG_dir_t;


/*   The def'n of the Frame Header   */
typedef struct {
    SLOG_uint32            Nbytes_cur;        /* rec's Within frame     */
    SLOG_uint32            Nrec_cur;
    SLOG_uint32            Nbytes_inc;        /* Incoming rec's         */
    SLOG_uint32            Nrec_inc;
    SLOG_uint32            Nbytes_pas;        /* Passing Through rec's  */
    SLOG_uint32            Nrec_pas;
    SLOG_uint32            Nbytes_out;        /* Outgoing rec's         */
    SLOG_uint32            Nrec_out;
} SLOG_framehdr_t;

/*
     The def'n of a Space ID tag in support primitive assocication for 
     a Send() and Recv() pair
*/
typedef struct {
    SLOG_nodeID_t        node;
    SLOG_cpuID_t         cpu;         /* local CPU ID per node     */
    SLOG_threadID_t      thread;      /* local thread ID per node  */
} SLOG_tasklabel_t;

/*   The def'n of an association pair, this is used to support association   */
typedef struct {
    /*
    SLOG_intvlrec_t   *irec;
    */
    void              *irec;             /* pointer to the associated irec */
    SLOG_assoc_t       fptr;             /* file pointer of associated irec */
} SLOG_assocpair_t;

/*   The def'n of a MPI call arguments vector   */
typedef struct {
    SLOG_N_args_t     size;
    SLOG_arg_t       *values;            /*  values[ size ]  */
} SLOG_vtrarg_t;

typedef struct SLOG_vtrarg_lptr_ {
           SLOG_vtrarg_t     *vtr;
    struct SLOG_vtrarg_lptr_ *prev;
    struct SLOG_vtrarg_lptr_ *next;
} SLOG_vtrarg_lptr_t;

/*   The def'n of the Interval Record ( run time )   */
typedef struct {
    SLOG_bytesize_t      bytesize;          /* bytesize in file */
    SLOG_rectype_t       rectype;
    SLOG_intvltype_t     intvltype;
    SLOG_bebits_t        bebits[2];
    SLOG_starttime_t     starttime;
    SLOG_duration_t      duration;
    SLOG_tasklabel_t     origID;            /*   origin    task label */
    SLOG_tasklabel_t     destID;            /* destination task label */
    SLOG_where_t         where;
    SLOG_N_assocs_t      N_assocs;
    SLOG_assoc_t        *assocs;            /* assocs[ N_assocs ] */
    SLOG_vtrarg_lptr_t  *vhead;
    SLOG_vtrarg_lptr_t  *vtail;
    SLOG_vtrarg_lptr_t  *vptr;
} SLOG_intvlrec_t;


/*   The def'n of Interval Record Bi-directional pointer type  */
typedef struct SLOG_intvlrec_lptr_ {
           SLOG_intvlrec_t       *irec;
    struct SLOG_intvlrec_lptr_   *prev;
    struct SLOG_intvlrec_lptr_   *next;
} SLOG_intvlrec_lptr_t;


/*   The def'n of Interval Record Bi-directional Linked List   */
typedef struct {
    SLOG_uint32            Nbytes_in_file;
    SLOG_uint32            count_irec;
    SLOG_intvlrec_lptr_t  *lhead;
    SLOG_intvlrec_lptr_t  *ltail;
    SLOG_intvlrec_lptr_t  *lptr;
} SLOG_intvlrec_blist_t;

/*   Infomation about the status of Irec Reservation in the frame   */
typedef struct {
    SLOG_int32     Nrec_eff;      /* effective No. of Irec carried over     */
    SLOG_int32     Nbytes_eff;    /* effective Nbytes of Irec carried over  */
    SLOG_int32     Nrec_tot;      /* total No. of Irec reserved in frame    */
    SLOG_int32     Nbytes_tot;    /* total Nbytes of Irec reserved in frame */
} SLOG_reserve_info_t;

/*   The def'n of the SLOG_STREAM   */
typedef struct {
    int            HasIrec2IOStreamBeenUsed;
    FILE          *fd;            /* File descriptor of the slogfile      */
    filebuf_t     *fbuf;          /* File Buffer pointer of the slogfile  */
    SLOG_fptr      file_dir_hdr;  /* beginning FD pointer in slog file    */
    SLOG_fptr      file_dir_cur;  /* current entry in FD in slog file     */
    SLOG_fptr      file_loc;      /* Current position in the slog file    */
    SLOG_time                  prev_starttime;    /* chk incr=startime    */
    SLOG_time                  prev_endtime;      /* chk incr=endtime     */
    SLOG_reserve_info_t       *reserve;           /* Frame Reserve Info   */
    SLOG_hdr_t                *hdr;               /* Header               */
    SLOG_pstat_t              *pstat;             /* Preview Statistics   */
    SLOG_pview_t              *pview;             /* Preview              */
    SLOG_prof_t               *prof;              /* Display Profile      */
    SLOG_threadinfo_table_t   *thread_tab;        /* Thread Table         */
    SLOG_recdefs_table_t      *rec_defs;          /* Record Def'n Table   */
    SLOG_dir_t                 frame_dir;         /* Directory            */
    SLOG_dir_entry_t           frame_dir_entry;   /* Current Frame entry  */
    SLOG_framehdr_t            frame_hdr;         /* Frame Header         */
    SLOG_intvlrec_blist_t     *cur_bbuf;          /* rec within Frame     */
    SLOG_intvlrec_blist_t     *inc_bbuf;          /* incoming rec         */
    SLOG_intvlrec_blist_t     *pas_bbuf;          /* passing through rec  */
    SLOG_intvlrec_blist_t     *out_bbuf;          /* outgoing rec         */
    SLOG_intvlrec_blist_t     *tmp_bbuf;          /* temporary rec        */
    SLOG_intvlrec_blist_t     *ptr2bbuf;          /* pointer to Bbuf read */
} SLOG_STREAM;



/*  High level SLOG (Common) API for both __Read__ and __Write__  */

SLOG_intvlrec_t *SLOG_Irec_Create( void );

void SLOG_Irec_Free( SLOG_intvlrec_t *irec );

void SLOG_Irec_CopyVal( const SLOG_intvlrec_t *src, SLOG_intvlrec_t *dest );

SLOG_intvlrec_t *SLOG_Irec_Copy( const SLOG_intvlrec_t *src );

int SLOG_Irec_Print( const SLOG_intvlrec_t *irec, FILE *outfd );

int SLOG_Irec_IsEmpty( const SLOG_intvlrec_t *irec );

int SLOG_Irec_IsConsistent( const SLOG_intvlrec_t *irec );

int SLOG_Irec_IsEqualTo( const SLOG_intvlrec_t *irec1,
                         const SLOG_intvlrec_t *irec2 );

	/*  Low level SLOG_Irec Methods  */

int SLOG_Irec_DepositToFbuf( const SLOG_intvlrec_t *irec, filebuf_t *fbuf );

SLOG_intvlrec_t *
SLOG_Irec_WithdrawFromFbuf( const SLOG_recdefs_table_t *slog_recdefs,
                                  filebuf_t            *fbuf );

int SLOG_Irec_AddVtrArgs(       SLOG_intvlrec_t *irec,
                          const SLOG_vtrarg_t   *varg );

const SLOG_vtrarg_t *SLOG_Irec_GetVtrArgs( SLOG_intvlrec_t  *irec );

void SLOG_Irec_DelAllVtrArgs( SLOG_intvlrec_t  *irec );



/*  High level SLOG __Write__ API  */

SLOG_STREAM *SLOG_OpenOutputStream( const char *filename );

int SLOG_Irec_ReserveSpace(       SLOG_STREAM      *slog,
                            const SLOG_rectype_t    rectype,
                            const SLOG_intvltype_t  intvltype,
                            const SLOG_bebit_t      bebit_0,
                            const SLOG_bebit_t      bebit_1,
                            const SLOG_starttime_t  starttime );

int SLOG_Irec_SetMinRec(       SLOG_intvlrec_t  *intvlrec,
                         const SLOG_rectype_t    rectype,
                         const SLOG_intvltype_t  intvltype,
                         const SLOG_bebit_t      bebit_0,
                         const SLOG_bebit_t      bebit_1,
                         const SLOG_starttime_t  starttime,
                         const SLOG_duration_t   duration,
                         const SLOG_nodeID_t     orig_node_id,
                         const SLOG_cpuID_t      orig_cpu_id,
                         const SLOG_threadID_t   orig_thread_id,
                         const SLOG_where_t      where,
                         ... );

int SLOG_Irec_SetDestTaskLabel(       SLOG_intvlrec_t  *intvlrec,
                                const SLOG_nodeID_t     node_id,
                                const SLOG_cpuID_t      cpu_id,
                                const SLOG_threadID_t   thread_id );

int SLOG_Irec_SetAssocs(       SLOG_intvlrec_t  *intvlrec,
                         const SLOG_STREAM      *slog,
                         const SLOG_N_assocs_t   N_assocs,
                         const SLOG_assoc_t     *assocs  );

int SLOG_Irec_SetArgs(       SLOG_intvlrec_t  *intvlrec,
                       const SLOG_STREAM      *slog,
                       const SLOG_N_args_t     N_args, 
                       const SLOG_arg_t       *args );

int SLOG_Irec_ToOutputStream(       SLOG_STREAM      *slog,
                              const SLOG_intvlrec_t  *irec );

int SLOG_CloseOutputStream( SLOG_STREAM   *slog );

int SLOG_STM_2ndPass( SLOG_STREAM *slog );



/*  High level SLOG __Read__ API  */

SLOG_STREAM *SLOG_OpenInputStream( const char *filename );

const SLOG_intvlrec_t *SLOG_Irec_FromInputStream( SLOG_STREAM *slog,
                                                  int *irc );

int SLOG_Irec_GetMinRec( const SLOG_intvlrec_t   *intvlrec,
                               SLOG_rectype_t    *rectype,
                               SLOG_intvltype_t  *intvltype,
                               SLOG_bebit_t      *bebit_0,
                               SLOG_bebit_t      *bebit_1,
                               SLOG_starttime_t  *starttime,
                               SLOG_duration_t   *duration,
                               SLOG_nodeID_t     *orig_node_id,
                               SLOG_cpuID_t      *orig_cpu_id,
                               SLOG_threadID_t   *orig_thread_id,
                               SLOG_where_t      *where,
                         ... );

int SLOG_Irec_GetAssocs( const SLOG_intvlrec_t  *intvlrec,
                         const SLOG_N_assocs_t   size_assocs,
                               SLOG_assoc_t     *assocs );

int SLOG_Irec_GetArgs(       SLOG_intvlrec_t  *intvlrec,
                       const SLOG_N_args_t     size_args,
                             SLOG_arg_t       *args );

int SLOG_CloseInputStream( SLOG_STREAM *slog );

#include "slog_header.h"
#include "slog_preview.h"
#include "slog_pstat.h"
#include "slog_ttable.h"
#include "slog_profile.h"
#include "slog_recdefs.h"

#endif  /* if ! defined( _SLOG ) */
