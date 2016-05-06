/* basic stuff for fractal engine */



#ifndef _FRACT_GEN_H_
#define _FRACT_GEN_H_

#include <stdio.h>

typedef enum _Fractal_type {MBROT, JULIA, NEWTON} Fractal_type;

#define NUM double

#define NUM_ASSIGN(a,b)     {(a)=(b);}
#define NUM_PTR_ASSIGN(a,b) {*(a)=(b);}
#define NUM_ADD(a,b)        ((a)+(b))
#define NUM_SUB(a,b)        ((a)-(b))
#define NUM_MULT(a,b)       ((a)*(b))
#define NUM_DIV(a,b)        ((a)/(b))
#define INT2NUM(a)          ((NUM)(a))
#define NUM2INT(a)          ((int)(a))
#define DBL2NUM(a)          (a)
#define NUM2DBL(a)          (a)

/* yea, I know the macros are a bit ugly, but I want to abstract out
   as much math as possible so if we change to an variable precision
   set of math routines, things will be easier */

#define COMPLEX_SQUARE(r,i,temp) { \
	NUM_ASSIGN( (temp), NUM_MULT((r),(i))); \
	NUM_ASSIGN( (r), NUM_SUB( NUM_MULT((r),(r)), NUM_MULT((i),(i)) ) ); \
	NUM_ASSIGN( (i), NUM_ADD((temp),(temp)) ); \
}

#define COMPLEX_ADD(ar,ai,br,bi) { \
  NUM_ASSIGN( (ar), NUM_ADD( (ar), (br)) ); \
  NUM_ASSIGN( (ai), NUM_ADD( (ai), (bi)) ); \
}

/* square and add */
#define COMPLEX_SQ_ADD(ar,ai,br,bi,temp) { \
	NUM_ASSIGN( (temp), NUM_MULT((ar),(ai))); \
	NUM_ASSIGN( (ar), NUM_ADD( NUM_SUB( NUM_MULT((ar),(ar)), \
					 NUM_MULT((ai),(ai)) ), \
				 (ar) ) ); \
	NUM_ASSIGN( (ai), NUM_ADD( NUM_ADD((temp),(temp)), (bi) ) ); \
}

  /* complex magnitude squared */
#define COMPLEX_MAGNITUDE_SQ(ar,ai) \
  NUM_ADD( NUM_MULT( (ar), (ar)), NUM_MULT( (ai), (ai) ) )

#define CHECK_ORDER(a,b) { \
  int temp; \
  if ((a)>(b)) {(temp)=(a); (a)=(b); (b)=(temp);} \
}

/* convert a screen (integer) coordinate to a complex (NUM) value */
#define COORD2CMPLX(cmin, cmax, xmin, xmax, x) \
  NUM_ADD( NUM_MULT( INT2NUM( (x) - (xmin) ), NUM_DIV( \
    NUM_SUB( (cmax), (cmin) ), INT2NUM( (xmax)-(xmin) ) ) ), (cmin) )

#ifdef _ANSI_ARGS_
#undef _ANSI_ARGS_
#endif

#ifdef __STDC__
#define _ANSI_ARGS_(x) x
#else
#define _ANSI_ARGS_(x) ()
#endif


void Mbrot_Settings _ANSI_ARGS_((double boundary, int maxiter));
int  MbrotCalcIter _ANSI_ARGS_((NUM re, NUM im));
void Julia_Settings _ANSI_ARGS_((double boundary, int maxiter, NUM real, NUM
				imag));
int  JuliaCalcIter _ANSI_ARGS_((NUM re, NUM im));
void Mbrotrep_Settings _ANSI_ARGS_((double boundary, int maxiter, int miniter,
		       int longestCycle, double fudgeFactor));
int  MbrotrepCalcIter _ANSI_ARGS_((NUM re, NUM im));

void CalcField _ANSI_ARGS_(( Fractal_type, int *iterField,
		int xstart, int xend, int ystart, int yend));

void Copysub2DArray _ANSI_ARGS_((int *mainArray, int *subArray, int mainWidth,
		     int mainHeight, int subWidth, int subHeight,
		     int xpos, int ypos));

typedef struct Mbrot_settings_ {
  double boundary_sq;		/* maximum allowable distance from origin,
				   squared */
  int maxiter;			/* maximum number of iterations */
} Mbrot_settings;

typedef struct Julia_settings_ {
  double boundary_sq;
  int maxiter;
  NUM r, i;			/* the point specifying this julia set */
} Julia_settings;

typedef struct Newton_settings_ {
  double epsilon;		/* stop computing when
				   epsilon > Z(n-1) > Z(n) > 0 */
  int *coeff;			/* coefficents of each term */
  int nterms;			/* number of terms */
  int maxiter;			/* when to give up */
    /* for example: nterms=4, coeff={-1,0,0,1}  == x^3 - 1 */
} Newton_settings;

#endif

/* _FRACT_GEN_H_ */
