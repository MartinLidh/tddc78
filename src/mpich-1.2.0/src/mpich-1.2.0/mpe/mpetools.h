/* $Id: mpetools.h,v 1.2 1998/09/16 19:14:40 gropp Exp $ */

/*
    This file contains some basic definitions that the tools routines
    may use.  They include:

    The name of the storage allocator
 */    
#ifndef __MPETOOLS
#define __MPETOOLS

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#define MALLOC(a)    malloc((unsigned)(a))
#define FREE(a)      free((char *)(a))
#define CALLOC(a,b)    calloc((unsigned)(a),(unsigned)(b))
#define REALLOC(a,b)   realloc(a,(unsigned)(b))

#define NEW(a)    (a *)MALLOC(sizeof(a))

#define MEMSET(s,c,n)   memset((char*)(s),c,n)


#endif
