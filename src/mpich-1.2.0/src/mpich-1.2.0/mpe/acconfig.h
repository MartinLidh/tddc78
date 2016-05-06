/* MPE/acconfig.h */
/* These are copied from mpich/acconfig.h */
/* The PRIMARY source of this file is MPICH/acconfig.h */
/* These are needed for ANY declaration that may be made by an AC_DEFINE */

/* Define if Fortran functions are pointers to pointers */
#undef FORTRAN_SPECIAL_FUNCTION_PTR

/* Define is C supports volatile declaration */
#undef HAS_VOLATILE

/* Define if XDR libraries available */
#undef HAS_XDR

/* Define if message catalog programs available */
#undef HAVE_GENCAT

/* Define if getdomainname function available */
#undef HAVE_GETDOMAINNAME

/* Define in gethostbyname function available */
#undef HAVE_GETHOSTBYNAME

/* Define if C has long long int */
#undef HAVE_LONG_LONG_INT

/* Define if C supports long doubles */
#undef HAVE_LONG_DOUBLE 

/* Define if msem_init function available */
#undef HAVE_MSEM_INIT

/* Define if C does NOT support const */
#undef HAVE_NO_C_CONST

/* Define if C supports prototypes (but isn't ANSI C) */
#undef HAVE_PROTOTYPES

/* Define if uname function available */
#undef HAVE_UNAME

/* Define if an int is smaller than void * */
#undef INT_LT_POINTER

/* Define if malloc returns void * (and is an error to return char *) */
#undef MALLOC_RET_VOID

/* Define if MPE extensions are included in MPI libraries */
#undef MPE_USE_EXTENSIONS

/* Define if MPID contains special case code for collective over world */
#undef MPID_COLL_WORLD

/* Define if MPID supports ADI collective */
#undef MPID_USE_ADI_COLLECTIVE

/* Define is ADI should maintain a send queue for debugging */
#undef MPI_KEEP_SEND_QUEUE

/* Define if mpe debug features should NOT be included */
#undef MPI_NO_MPEDBG

/* Define if struct msemaphore rather than msemaphore required */
#undef MSEMAPHORE_IS_STRUCT

/* Define if void * is 8 bytes */
#undef POINTER_64_BITS

/* Define if stdarg can be used */
#undef USE_STDARG

/* For Cray, define two word character descriptors in use */
#undef _TWO_WORD_FCD

/* Define if extra traceback information should be kept */
#undef DEBUG_TRACE

/* Define if Fortran is NOT available */
#undef MPID_NO_FORTRAN

/* Define if memory debugging should be enabled */
#undef MPIR_MEMDEBUG

/* Define if object debugging should be enabled */
#undef MPIR_OBJDEBUG

/* Define if ptr conversion debugging should be enabled */
#undef MPIR_PTRDEBUG

/* Define if ADI is ADI-2 (required!) */
#undef MPI_ADI2

/* Define if mmap does not work correctly for anonymous memory */
#undef HAVE_NO_ANON_MMAP

/* Define if signals reset to the default when used (SYSV vs BSD semantics).
   Such signals are essentially un-usable, because of the resulting race
   condition.  The fix is to use the sigaction etc. routines instead (they're
   usually available, since without them signals are entirely useless) */
#undef SIGNALS_RESET_WHEN_USED

/* Define if MPI Structs should align on the largest basic element */
#undef USE_BASIC_ALIGNMENT

/* The number of processors expected on an SMP.  Usually undefined */
#undef PROCESSOR_COUNT

/* Define this to force a choice of shared memory allocator */
#undef SHMEM_PICKED

/* Define this to force SysV shmat for shared memory allocator */
#undef USE_SHMAT

/* Define this to force a choice for memory locking */
#undef LOCKS_PICKED

/* Define this to force SysV semop for locks */
#undef USE_SEMOP

/* End of MPICH acconfig.h */

/* MPE/acconfig.h specific entries */
/* define is stdargs.h is available */
#undef HAVE_STDARG_H

/* define for MPICH name */
#undef MPICH_NAME

/* define if Cray MPI version is 1.1.0.4 */
#undef CRAY1104

/* define if Cray MPI version is unknown */
#undef CRAYUNKNOWN

/* define if IBM version is unknown */
#undef IBMUNKNOWN
