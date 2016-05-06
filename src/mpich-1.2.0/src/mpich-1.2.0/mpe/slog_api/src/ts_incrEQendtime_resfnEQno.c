#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "slog.h"

#define  MaxNdirFrames    64
#define  FrameByteSize    256
#define  SLOG_PREVIEWNAME  "SLOG_Preview.txt"
#define  SLOG_PROFILENAME  "SLOG_Profile.txt"    /*  Not used  */
#define  USER_MARKER 

#define  MaxIdx4VarIrec   4
#define  MaxNumOfProc     4

#if 0 
#define  MaxAssocs  10
#define  MaxArgs    10
#else
#define  MaxAssocs  0
#define  MaxArgs    0
#endif

typedef struct {
    int               IsStartEvent;
    SLOG_Irec         irec;
} tagged_irec;

int event_cmp_fn( const void *event1, const void *event2 );

int event_cmp( const tagged_irec *event1, const tagged_irec *event2 );

SLOG_intvltype_t GetIntvlType( const SLOG_uint32 IdxType,
                               const SLOG_uint32 IdxAssocs,
                               const SLOG_uint32 IdxArgs );

int str_add_num_label( char *str, const char *fix_str, int num );

int main( int argc, char **argv )
{
    const char slog_name[] = "slogfile.data";

    const SLOG_intvltype_t     VarIrec_MinIdx = 0;
    const SLOG_intvltype_t     FixIrec_MinIdx = 1;
    const SLOG_intvltype_t     FixIrec_MaxIdx = 5;
    const SLOG_intvltype_t     MaxIdxType     = FixIrec_MaxIdx
                                              - FixIrec_MinIdx + 1;
    const SLOG_intvltype_t     VarIrec_MaxAssocs = 2 * MaxIdx4VarIrec;
          SLOG                 slog;
          SLOG_Irec            irec;

          int                  MaxIrec = 7;
          int                  ii, jj, count, idx_type, ierr;

          /*  Variables for SLOG Thread Table  */
          SLOG_nodeID_t        node_id;
          SLOG_threadID_t      thread_id;
          SLOG_OSprocessID_t   OSprocess_id;
          SLOG_OSthreadID_t    OSthread_id;
          SLOG_appID_t         app_id;

          /*  Variables for SLOG Display Profile  */
    const char                 fix_classtype[10] = "state";
    const char                 fix_label[10]     = "label";
    const char                 fix_color[10]     = "color";
          char                 classtype[10]     = "state";
          char                 label[10]         = "label";
          char                 color[10]         = "color";
          char                *color_strs[10]    = { "red", "green",
                                                     "blue", "yellow",
                                                     "orange", "gold",
                                                     "RoyalBlue", "navy",
                                                     "pink", "grey" };

          /*  Variables for SLOG Record Definition Table  */
          SLOG_intvltype_t     intvltype;
          SLOG_bebit_t         bebit_0 = 0;
          SLOG_bebit_t         bebit_1 = 1;
          SLOG_N_assocs_t      Nassocs;
          SLOG_N_args_t        Nargs;

          /*  Variables for SLOG Interval Record  */
          SLOG_rectype_t       irec_rectype;
          SLOG_intvltype_t     irec_intvltype;
          SLOG_starttime_t     irec_starttime;
          SLOG_duration_t      irec_duration;
          SLOG_time            irec_endtime;
          SLOG_bebit_t         irec_bebits[2];
          SLOG_nodeID_t        irec_node_id;
          SLOG_cpuID_t         irec_cpu_id;
          SLOG_threadID_t      irec_thread_id;
          SLOG_where_t         irec_where;

          SLOG_N_assocs_t      irec_Nassocs;
          SLOG_assoc_t         irec_assocs[ 100 ];
          SLOG_N_args_t        irec_Nargs;
          SLOG_arg_t           irec_args[ 100 ];
          SLOG_uint32          irec_Nvtrargs = 2;


          /*
              Generate an array of events from a set of interval records.
          */
          tagged_irec         *events;
          SLOG_uint32          Nentries_extra_recdefs;
          SLOG_recdef_t       *recdefs_reserved;
          SLOG_recdef_t       *recdef;
          SLOG_intvlinfo_t    *intvlinfos_reserved;
          SLOG_intvlinfo_t    *intvlinfo;

    /*
        Change the Number of Interval Records created in slogfile based on
        command line argument
    */
    if ( argc > 1 ) {
        printf( "argv[1] = _%s_\n", argv[1] );
        if ( isdigit( (int) argv[1][0] ) )  MaxIrec = atoi( argv[1] );
    }

    /*  MaxIrec = 15;  */

    /*  Initialization  */
    for ( jj = 0; jj < MaxAssocs; jj++ ) irec_assocs[ jj ] = jj+1;
    for ( jj = 0; jj < MaxArgs; jj++ )   irec_args[ jj ] = jj+1;
    irec_starttime = 0.0;
    irec_duration  = 0.0;

    /*  Get a handle to the SLOG_STREAM data structure  */
    slog = SLOG_OpenOutputStream( slog_name );

    /*
        Tell the API the properties of the slog file by using SLOG_SetXXXX()
    */

    /*  Set the maimum number of frames per directory in slog file  */
    SLOG_SetMaxNumOfFramesPerDir( slog, MaxNdirFrames );

    /*  Set the byte size per frame  */
    SLOG_SetFrameByteSize( slog, FrameByteSize );

    /*  Set the reserved space in each frame  */
    SLOG_SetFrameReservedSpace( slog, 3 * SLOG_typesz[ min_IntvlRec ] );

    /*  Set the flag for the time order arrangement of the interval record  */
    /*  SLOG_SetIncreasingStarttimeOrder( slog );  */
    SLOG_SetIncreasingEndtimeOrder( slog ); 

    /*
        Set the SLOG Preview to be read from a given file, filename,
        and then init SLOG's Preview section
    */ 
    SLOG_SetPreviewName( slog, SLOG_PREVIEWNAME );

    /*
        This is an optional call to set the Preview Statistics's array
        size, if this function is NOT called, the default size
        SLOG_PSTAT_STATSIZE_MAX will be used.
    */
    SLOG_SetPreviewStatisticsSize( slog, 20 );

#if defined( USER_MARKER )
    Nentries_extra_recdefs = 2;
    recdefs_reserved    = ( SLOG_recdef_t * )
                          malloc( Nentries_extra_recdefs
                                * sizeof ( SLOG_recdef_t ) );
    intvlinfos_reserved = ( SLOG_intvlinfo_t * )
                          malloc( Nentries_extra_recdefs
                                * sizeof ( SLOG_intvlinfo_t ) );
#else
    Nentries_extra_recdefs = 0;
    recdefs_reserved = NULL;
    intvlinfos_reserved = NULL;
#endif


    /*  Initialize the SLOG Thread Table  */
    if ( SLOG_TTAB_Open( slog ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - SLOG_TTAB_Open() fails!\n" );
        fflush( errfile );
        exit( 1 );
    }

    /*  Add the entries of SLOG Thread Table  */
    for ( ii = 0; ii < MaxIrec; ii++ ) {
        node_id      = ii % MaxNumOfProc;
        thread_id    = ii / MaxNumOfProc;
        OSprocess_id = ii % ( MaxNumOfProc / 2 );
        OSthread_id  = ii % ( MaxNumOfProc * 2 );
        app_id       = node_id;
        ierr = SLOG_TTAB_AddThreadInfo( slog, node_id, thread_id,
                                        OSprocess_id, OSthread_id, app_id );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__"Main() - "
                              "SLOG_TTAB_AddTheadInfo() fails!\n" ); 
            fflush( errfile );
            /*  exit( 1 );  */
        }
    }

    /*  Finalize the Record Definition Table in SLOG file  */
    ierr = SLOG_TTAB_Close( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__"Main() - "
                          "SLOG_TTAB_Close() fails!\n" ); 
        fflush( errfile );
        exit( 1 );
    } 
    
    /*  Set the SLOG Display Profile with a given filename and init SLOG  */
    /*  SLOG_SetProfileName( slog, SLOG_PROFILENAME );  */

    /*  Initialize the SLOG Display Profile Table  */
    if ( SLOG_PROF_Open( slog ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - SLOG_PROF_Open() fails\n " );
        fflush( errfile );
        exit( 1 );
    }

    count = -1;
    /*  Create the Indexes for Variable Interval's display profile  */
    Nargs = 0;    /*  Initialization, does NOT mean anything  */
    for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
        count++;
        intvltype = GetIntvlType( VarIrec_MinIdx, Nassocs, Nargs );
        str_add_num_label( label, fix_label, count );
        /*  str_add_num_label( color, fix_color, count );  */
        strcpy( color, color_strs[ count ] );
        ierr = SLOG_PROF_AddIntvlInfo( slog, intvltype,
                                       classtype, label, color );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":Main() - "
                              "SLOG_PROF_AddIntvlInfo() fails!\n" );
            fflush( errfile );
            exit( 1 );
        }
    }

    /*  Create the Indexes for Fixed Interval Record's display profile  */
    for ( idx_type  = FixIrec_MinIdx;
          idx_type  < FixIrec_MaxIdx;
          idx_type++ ) {
        for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
            for ( Nargs = 0; Nargs <= MaxArgs; Nargs++ ) {
                count++;
                intvltype = GetIntvlType( idx_type, Nassocs, Nargs );
                str_add_num_label( label, fix_label, count );
                /*  str_add_num_label( color, fix_color, count );  */
                strcpy( color, color_strs[ count ] );
                ierr = SLOG_PROF_AddIntvlInfo( slog, intvltype,
                                               classtype, label, color );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__":Main() - "
                                      "SLOG_PROF_AddIntvlInfo() fails!\n" );
                    fflush( errfile );
                    exit( 1 );
                }
            }
        }
    }

#if defined( USER_MARKER )
    ierr = SLOG_PROF_SetExtraNumOfIntvlInfos( slog, Nentries_extra_recdefs );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - "
                          "SLOG_PROF_SetExtraNumOfIntvlInfos(%d) fails!\n",
                          Nentries_extra_recdefs );
        fflush( errfile );
        exit( 1 );
    }
#else
    /*  Finalize the Display Profile Table in SLOG file  */
    ierr = SLOG_PROF_Close( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - "
                          "SLOG_PROF_Close() fails!\n" );
        fflush( errfile );
        exit( 1 );
    }
#endif


    /*  Initialize the SLOG Record Definition Table  */
    if ( SLOG_RDEF_Open( slog ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - SLOG_RDEF_Open() fails\n " );
        fflush( errfile );
        exit( 1 );
    }

    /*  Create the Indexes for Variable Interval Record Definition  */
    Nargs = 0;    /*  Initialization, does NOT mean anything  */
    for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
        intvltype = GetIntvlType( VarIrec_MinIdx, Nassocs, Nargs );
        ierr = SLOG_RDEF_AddRecDef( slog,
                                    intvltype, bebit_0, bebit_1,
                                    Nassocs, Nargs );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":Main() - "
                              "SLOG_RDEF_AddRecDef() fails!\n" ); 
            fflush( errfile );
            exit( 1 );
        }
    }

    /*  Create the Indexes for Fixed Interval Record Definition  */
    for ( idx_type  = FixIrec_MinIdx;
          idx_type  < FixIrec_MaxIdx;
          idx_type++ ) {
        for ( Nassocs = 0; Nassocs <= MaxAssocs; Nassocs++ ) {
            for ( Nargs = 0; Nargs <= MaxArgs; Nargs++ ) {
                intvltype = GetIntvlType( idx_type, Nassocs, Nargs );
                ierr = SLOG_RDEF_AddRecDef( slog,
                                            intvltype, bebit_0, bebit_1,
                                            Nassocs, Nargs );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__":Main() - "
                                      "SLOG_RDEF_AddRecDef() fails!\n" ); 
                    fflush( errfile );
                    exit( 1 );
                }
            }
        }
    }
   
#if defined( USER_MARKER )
    ierr = SLOG_RDEF_SetExtraNumOfRecDefs( slog, Nentries_extra_recdefs );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - "
                          "SLOG_RDEF_SetExtraNumOfRecDefs(%d) fails!\n",
                          Nentries_extra_recdefs );
        fflush( errfile );
        exit( 1 );
    } 
#else
    /*  Finalize the Record Definition Table in SLOG file  */
    ierr = SLOG_RDEF_Close( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":Main() - "
                          "SLOG_RDEF_Close() fails!\n" );
        fflush( errfile );
        exit( 1 );
    } 
#endif

    printf( "Start appending records :\n" );

    events = ( tagged_irec * ) malloc( 2*MaxIrec * sizeof( tagged_irec ) );

/*
    printf( "\t""Append variable records:\n" );
    for ( ii = 0; ii <= VarIrec_MaxAssocs; ii++ ) {
        irec_rectype   = ( SLOG_rectype_t ) 0;
        irec_bebits[0] = ( SLOG_bebit_t ) 0;
        irec_bebits[1] = ( SLOG_bebit_t ) 1;
        irec_starttime = ( SLOG_starttime_t ) ii*0.1;
        irec_duration  = ( SLOG_duration_t ) 0.1;
        irec_node_id   = ( SLOG_nodeID_t ) ii % MaxNumOfProc;
        irec_cpu_id    = ( SLOG_cpuID_t ) 1;
        irec_thread_id = ( SLOG_threadID_t ) ii / MaxNumOfProc;
        irec_where     = ( SLOG_where_t ) 8888;

        irec_Nassocs   = ii % ( MaxIdx4VarIrec + 1 );
        irec_Nargs     = 2;
        irec_intvltype = GetIntvlType( VarIrec_MinIdx,
                                       irec_Nassocs, irec_Nargs );

        irec = SLOG_Irec_Create();
        SLOG_Irec_SetMinRec( irec, irec_rectype, irec_intvltype, 
                             irec_bebits[0], irec_bebits[1],
                             irec_starttime, irec_duration,
                             irec_node_id, irec_cpu_id, irec_thread_id,
                             irec_where );
        if ( irec_Nassocs > 0 )
            SLOG_Irec_SetAssocs( irec, slog, irec_Nassocs, irec_assocs );
        for ( jj = 0; jj < irec_Nvtrargs; jj++ ) {
            if ( irec_Nargs > 0 )
               SLOG_Irec_SetArgs( irec, slog, irec_Nargs, irec_args );
            irec_Nargs++;
        }

        printf( "MAIN: %d, irec = ( "
                fmt_itype_t", "fmt_stime_t", "fmt_dura_t", "
                fmt_Nassocs_t" )\n", 
                ii, irec_intvltype, irec_starttime, irec_duration,
                irec_Nassocs );
        SLOG_Irec_Print( irec, stdout ); fprintf( stdout, "\n" );

        SLOG_Irec_ToOutputStream( slog, irec );
        SLOG_Irec_Free( irec );
    }

    irec_endtime = irec_starttime + irec_duration;
*/

    printf( "\t""Creation of fixed records:\n" );

    for ( ii = 0; ii < MaxIrec; ii++ ) {

        /*
            Set up the default value of the irec's components
        */
        irec_rectype   = ( SLOG_rectype_t ) 100;
        irec_bebits[0] = ( SLOG_bebit_t ) 0;
        irec_bebits[1] = ( SLOG_bebit_t ) 1;
        irec_endtime   = ( SLOG_time ) ( ii + 1 ) * 0.1;
        irec_duration  = ( SLOG_duration_t ) 0.08;
        irec_starttime = ( SLOG_starttime_t ) ( irec_endtime - irec_duration );
        irec_node_id   = ( SLOG_nodeID_t ) ii % MaxNumOfProc;
        irec_cpu_id    = ( SLOG_cpuID_t ) 1;
        irec_thread_id = ( SLOG_threadID_t ) ii / MaxNumOfProc;
        irec_where     = ( SLOG_where_t ) 8888;

        /*
            Make up some starttime and endtime stamps of interval records
            to check various features of the SLOG WRITE API
        */
        if ( ii == 3 ) {
            irec_duration  = ( SLOG_duration_t ) 0.15;
            irec_starttime = irec_endtime - irec_duration;
        }
        if ( ii == 7 ) {
            irec_duration  = ( SLOG_duration_t ) 0.55;
            irec_starttime = irec_endtime - irec_duration;
        }
        if ( ii == 12 ) {
            irec_duration = ( SLOG_duration_t ) 0.75;
            irec_starttime = irec_endtime - irec_duration;
        }
        if ( ii == 17 ) {
            irec_duration = ( SLOG_duration_t ) 1.55;
            irec_starttime = irec_endtime - irec_duration;
        }

        /*
          - Check the support of variable length Association arguments is OK.
          - Check if the support of variable length MPI arguments is OK.
            Modify the irec_intvltype to reflect the number of
            association arguments attached in the interval records.
        */

        switch ( 0 ) {
        case 0 : 
            irec_Nassocs  = 0;
            irec_Nargs    = 0;
            break;
        case 1 :
            irec_Nassocs  = 0;
            irec_Nargs    = ii % ( MaxIdx4VarIrec + 1 );
            break;
        case 2 :
            irec_Nassocs  = ii % ( MaxIdx4VarIrec + 1 );
            irec_Nargs    = 0;
            break;
        case 3 :
            irec_Nassocs  = ii % ( MaxIdx4VarIrec + 1 ); 
            irec_Nargs    = ii / ( MaxIdx4VarIrec + 1 );
            break;
        default :
            printf( "Wrong int in the switch statements\n" );
        }

        /*  Set the default idx_type  */
        idx_type      = ii % MaxIdxType;

        for ( jj = 0; jj < Nentries_extra_recdefs; jj++ ) {
            /*  Make up some extra idx_type, hence extra irec_intvltype  */
            if ( ii == (jj+1)*3 ) {
                idx_type = MaxIdxType + jj;
                break;
            }
        }

        /*  Formulas used to match the intvltype in Record Definition Table  */
        irec_intvltype = GetIntvlType( idx_type, irec_Nassocs, irec_Nargs );

        /*
            Create an record definition which is NOT in 
            Record Definition Table at the moment
            Create an interval descriptor which is NOT in 
            Display Profile Table at the moment
        */
        for ( jj = 0; jj < Nentries_extra_recdefs; jj++ ) {
            /*  Only one of the jj's is true in the if statement  */
            if ( ii == (jj+1)*3 ) {
                count++;
                SLOG_RecDef_Assign( &(recdefs_reserved[jj]), irec_intvltype,
                                    irec_bebits[0], irec_bebits[1], 
                                    irec_Nassocs, irec_Nargs );
                str_add_num_label( label, fix_label, count*10 );
                /*  str_add_num_label( color, fix_color, count*10 );  */
                strcpy( color, color_strs[ count ] );
                SLOG_IntvlInfo_Assign( &(intvlinfos_reserved[jj]),
                                       irec_intvltype,
                                       classtype, label, color );
                break;
            }
        }

        /*  Creation of an interval record  */
        irec = SLOG_Irec_Create();
        SLOG_Irec_SetMinRec( irec, irec_rectype, irec_intvltype, 
                             irec_bebits[0], irec_bebits[1],
                             irec_starttime, irec_duration,
                             irec_node_id, irec_cpu_id, irec_thread_id,
                             irec_where );
        if ( irec_Nassocs > 0 )
            SLOG_Irec_SetAssocs( irec, slog, irec_Nassocs, irec_assocs );
        if ( irec_Nargs > 0 )
            SLOG_Irec_SetArgs( irec, slog, irec_Nargs, irec_args );

        /*  Print the irec to the stdout  */
        SLOG_Irec_Print( irec, stdout ); fprintf( stdout, "\n" );

        /*  Make up an array of tagged interval records  */
        events[ 2*ii   ].IsStartEvent = SLOG_TRUE;
        events[ 2*ii   ].irec         = irec;
        events[ 2*ii+1 ].IsStartEvent = SLOG_FALSE;
        events[ 2*ii+1 ].irec         = SLOG_Irec_Copy( irec ); 

    }   /*  for ( ii = 0; ii < MaxIrec; ii++ )  */

    printf( "Finish the creation of records :\n" );

    /*
        Sort the tagged irec array, i.e., events, so we can simulate an 
        event based calling sequence of SLOG_Irec_ReserveSpace() 
        & SLOG_Irec_ToOutputStream().  The SLOG API does NOT demand
        use of qsort() to use the API.
    */
    qsort( events, 2*MaxIrec, sizeof( tagged_irec ), &event_cmp_fn );

    printf( "\t""Append fixed records:\n" );

    /*
        The 1st character in stdout indicates if the call corresponds
        a Start event or an End event of the interval record.
    */
    for ( ii = 0; ii < 2*MaxIrec; ii++ ) {
        irec = events[ii].irec;

        /*
            Check if new Record Definition needs to be added to
            the Record Definition Table before either
            SLOG_Irec_ReserveSpace() or SLOG_Irec_ToOutputStream()
            is called.
        */
        for ( jj = 0; jj < Nentries_extra_recdefs; jj++ ) {

            recdef = &(recdefs_reserved[ jj ]);
            intvlinfo = &(intvlinfos_reserved[ jj ]);
            if (    irec->intvltype == recdef->intvltype
                 && irec->bebits[0] == recdef->bebits[0] 
                 && irec->bebits[1] == recdef->bebits[1]
                 && events[ii].IsStartEvent == SLOG_TRUE ) {

                ierr = SLOG_RDEF_AddExtraRecDef( slog, recdef->intvltype,
                                                 recdef->bebits[0],
                                                 recdef->bebits[1],
                                                 recdef->Nassocs,
                                                 recdef->Nargs );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__":Main() - "
                                      "SLOG_RDEF_AddExtraRecDef() fails!\n" );
                    fflush( errfile );
                    exit( 1 );
                }
                fprintf( stdout, "SLOG_RDEF_AddExtraRecDef(" );
                SLOG_RecDef_Print( recdef, stdout );
                fprintf( stdout, ") before %d-th event\n", ii );

                ierr = SLOG_PROF_AddExtraIntvlInfo( slog, intvlinfo->intvltype,
                                                    intvlinfo->classtype,
                                                    intvlinfo->label,
                                                    intvlinfo->color );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__":Main() - "
                                   "SLOG_PROF_AddExtraIntvlInfo() fails!\n" );
                    fflush( errfile );
                    exit( 1 );
                }
                fprintf( stdout, "SLOG_PROF_AddExtraIntvlInfo(" );
                SLOG_IntvlInfo_Print( intvlinfo, stdout );
                fprintf( stdout, ") before %d-th event\n", ii );

            }
        }

        /*
            Check if the tagged Irec corresponds to a Start Event
            or End Event.
            The following comments are based on the irec are arranged
            in increasing endtime order.
            For a Start Event tagged Irec, SLOG_Irec_ReserveSpace()
            should be called if SLOG_SetFrameReservedSpace() has NOT
            been called.
            For a End Event tagged Irec, SLOG_Irec_ToOutputStream()
            should be called.
        */
        if ( events[ii].IsStartEvent == SLOG_TRUE ) {
            /*  Does Nothing  */
        }
        else {
            SLOG_Irec_ToOutputStream( slog, irec );
            fprintf( stdout, "E " );
            SLOG_Irec_Print( irec, stdout );
            fprintf( stdout, "\n" );
        }
    }   /*  for ( ii = 0; ii < 2*MaxIrec; ii++ )  */

    printf( "Finish appending records :\n" );

    for ( ii = 0; ii < 2*MaxIrec; ii++ )
        SLOG_Irec_Free( events[ii].irec );
    free( events );

    SLOG_CloseOutputStream( slog );

    return 0;
}



/*  Wrapper for event_cmp()  */
int event_cmp_fn( const void *event1, const void *event2 )
{
    return( event_cmp( (tagged_irec *) event1, (tagged_irec *) event2 ) );
}

/*
    Compare functions needed in qsort() to sort the starttime or endtime
    of all interval records ( depend on if it is a start or end event )
    in accending time order, so SLOG_Irec_ReserveSpace() and 
    SLOG_Irec_ToOutputStream() can be called at the right order to 
    Simulate a event based input order for SLOG API.
*/
int event_cmp( const tagged_irec *event1, const tagged_irec *event2 )
{
    SLOG_time   time1, time2;

    if ( event1->IsStartEvent == SLOG_TRUE )
        time1 = event1->irec->starttime;
    else
        time1 = event1->irec->starttime + event1->irec->duration;

    if ( event2->IsStartEvent == SLOG_TRUE ) 
        time2 = event2->irec->starttime;
    else
        time2 = event2->irec->starttime + event2->irec->duration;

    if ( time1 < time2 )
        return -1;
    else if ( time1 == time2 )
        return 0;
    else
        return 1;
}



SLOG_intvltype_t GetIntvlType( const SLOG_uint32 IdxType,
                               const SLOG_uint32 IdxAssocs,
                               const SLOG_uint32 IdxArgs )
{
    const unsigned int    UnitType       = 100;
    const unsigned int    UnitAssocs     = 10;
    const unsigned int    UnitArgs       = 1;
    /*  May want to check if UnitXXXX is in the correct range  */
    return ( IdxType * UnitType + IdxAssocs * UnitAssocs + IdxArgs * UnitArgs );
}

int str_add_num_label( char *str, const char *fix_str, int num )
{
    char  num_str[ 10 ];

    sprintf( num_str, " %u", num );
    strcpy( str, fix_str );
    return ( (int)strcat( str, num_str ) );
}
