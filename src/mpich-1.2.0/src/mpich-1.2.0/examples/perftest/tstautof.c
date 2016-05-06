/* tstauto.c */
/* Fortran interface file */
#include "tools.h"

#ifdef POINTER_64_BITS
extern void *__ToPointer();
extern int __FromPointer();
extern void __RmPointer();
#else
#define __ToPointer(a) (a)
#define __FromPointer(a) (int)(a)
#define __RmPointer(a)
#endif

#ifdef FORTRANCAPS
#define tstauto1d_ TSTAUTO1D
#elif !defined(FORTRANUNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define tstauto1d_ tstauto1d
#endif
int  tstauto1d_( xmin, xmax, dxmin, dxmax, 
               rtol, atol, results, rsize, rmax, fcn, fcnctx)
double *xmin, *xmax, *dxmin, *dxmax, *rtol, *atol, (*fcn)();
int    *rsize, *rmax;
char   *results;
void   *fcnctx;
{
return TSTAuto1d(*xmin,*xmax,*dxmin,*dxmax,*rtol,*atol,results,*rsize,*rmax,fcn,fcnctx);
}
#ifdef FORTRANCAPS
#define tstrsort_ TSTRSORT
#elif !defined(FORTRANUNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define tstrsort_ tstrsort
#endif
void  tstrsort_( results, rsize, rcnt)
char *results;
int  *rsize, *rcnt;
{
TSTRSort(results,*rsize,*rcnt);
}
