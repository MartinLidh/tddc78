/*
 *  $Id: sbcnst.h,v 1.1.1.1 1997/09/17 20:38:06 gropp Exp $
 *
 *  (C) 1993 by Argonne National Laboratory and Mississipi State University.
 *      All rights reserved.  See COPYRIGHT in top-level directory.
 */

/* %W% %G% */
#ifndef _SBCNST
#define _SBCNST

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#ifndef _SBCNSTDEF
extern void *MPIR_SBinit ANSI_ARGS(( int, int, int )), 
             MPIR_SBfree ANSI_ARGS(( void *, void * )),
             MPIR_SBiAllocate ANSI_ARGS(( void *, int, int )),
            *MPIR_SBalloc ANSI_ARGS(( void * )),
             MPIR_SBPrealloc ANSI_ARGS(( void *, int ));
#endif

#define SBinit BUGGG%
#define SBfree BUGGG%
#define SBiAllocate BUGGG%
#define SBalloc BUGGG%
#define SBPrealloc BUGGG%

/* Chameleon/PETSc includes memory tracing functions that can be used
   to track storage leaks.  This code chooses that or the copy that 
   has been placed into mpich/util/tr.c 
 */
#ifndef MALLOC

#if defined(DEVICE_CHAMELEON) && defined(MPIR_DEBUG_MEM)
/* Use Chameleon/PETSC version of MALLOC/FREE */
#define MALLOC(a)    trmalloc((unsigned int)(a),__LINE__,__FILE__)
#define CALLOC(a,b)  trcalloc((unsigned int)(a),(unsigned)(b),__LINE__,__FILE__)
#define FREE(a)      trfree((char *)(a),__LINE__,__FILE__)
#define NEW(a)    (a *)MALLOC(sizeof(a))
/* Also replace the SB allocators so that we can get the trmalloc line/file
   tracing. */
#define MPIR_SBinit(a,b,c) ((void *)(a))
#define MPIR_SBalloc(a)    trmalloc((unsigned int)(a),__LINE__,__FILE__)
#define MPIR_SBfree(a,b)   trfree((char *)(b),__LINE__,__FILE__)
#define MPIR_SBdestroy(a)

#elif defined(MPIR_MEMDEBUG)
/* Use MPI tr version of MALLOC/FREE */
extern void *MPIR_trmalloc ANSI_ARGS((unsigned,int,char *)), 
            MPIR_trfree ANSI_ARGS((char *, int, char *)), 
           *MPIR_trcalloc ANSI_ARGS((unsigned,unsigned,int,char *));
#define MALLOC(a)    MPIR_trmalloc((unsigned)(a),__LINE__,__FILE__)
#define CALLOC(a,b)  \
    MPIR_trcalloc((unsigned)(a),(unsigned)(b),__LINE__,__FILE__)
#define FREE(a)      MPIR_trfree((char *)(a),__LINE__,__FILE__)
#define NEW(a)        (a *)MALLOC(sizeof(a))
/* Also replace the SB allocators so that we can get the trmalloc line/file
   tracing. */
#define MPIR_SBinit(a,b,c) ((void *)(a))
#define MPIR_SBalloc(a)    MPIR_trmalloc((unsigned)(a),__LINE__,__FILE__)
#define MPIR_SBfree(a,b)   MPIR_trfree((char *)(b),__LINE__,__FILE__)
#define MPIR_SBdestroy(a)
#else

/* We also need to DECLARE malloc etc here.  Note that P4 also declares
   some of these, and thus if P4 in including this file, we skip these
   declarations ... */
#ifndef P4_INCLUDED

#if HAVE_STDLIB_H || STDC_HEADERS
#include <stdlib.h>

#else
#ifdef __STDC__
extern void 	*calloc(/*size_t, size_t*/);
extern void	free(/*void * */);
extern void	*malloc(/*size_t*/);
#elif defined(MALLOC_RET_VOID)
extern void *malloc();
extern void *calloc();
#else
extern char *malloc();
extern char *calloc();
/* extern int free(); */
#endif /* __STDC__ */
#endif /* HAVE_STDLIB_H || STDC_HEADERS */
#endif /* !defined(P4_INCLUDED) */

#define MALLOC(a)    malloc((unsigned)(a))
#define CALLOC(a,b)  calloc((unsigned)(a),(unsigned)(b))
#define FREE(a)      free((char *)(a))
#define NEW(a)    (a *)MALLOC(sizeof(a))
#endif /*MPIR_DEBUG_MEM*/
#endif /*MALLOC*/

#endif
