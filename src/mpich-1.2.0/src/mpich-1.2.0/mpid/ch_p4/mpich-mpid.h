/* mpich-mpid.h.  Generated automatically by configure.  */
/* mpich-mpid.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if mmap does not work correctly for anonymous memory */
/* #undef HAVE_NO_ANON_MMAP */

/* Define if signals reset to the default when used (SYSV vs BSD semantics).
   Such signals are essentially un-usable, because of the resulting race
   condition.  The fix is to use the sigaction etc. routines instead (they're
   usually available, since without them signals are entirely useless) */
#define SIGNALS_RESET_WHEN_USED 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* Define if you have the BSDgettimeofday function.  */
/* #undef HAVE_BSDGETTIMEOFDAY */

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the mmap function.  */
#define HAVE_MMAP 1
