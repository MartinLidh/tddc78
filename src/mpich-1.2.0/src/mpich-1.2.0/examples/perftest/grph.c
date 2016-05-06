#include <stdio.h>

#include "mpi.h"
extern int __NUMNODES, __MYPROCID;
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
/* 
    This file contains routines to generate output from the mpptest programs
 */

/* 
   In order to simplify the generation of graphs of multiple data sets, 
   we want to allow the generated cit code to contain the necessary 
   window selection commands.  To do this, we add arguments for
   -wx i n    windows in x, my # and total #
   -wy i n    windows in y, my # and total #
   -lastwindow generate the wait/new page.
 */
typedef enum { GRF_X, GRF_EPS, GRF_PS } OutputForm;
typedef struct {
    FILE *fp, *fpdata;
    char *fname2;
    void (*header)();
    void (*dataout)();
    void (*draw)();
    void (*endpage)();
    /* For collective operations */
    void (*headergop)();    
    void (*dataoutgop)();
    void (*drawgop)();
    /* Information about the graph */
    int wxi, wxn, wyi, wyn, is_lastwindow;
    int givedy;
    int do_rate;
    int do_fit;
    char *title;
    OutputForm output_type;
    } GraphData;

void PrintGraphHelp( )
{
fprintf( stderr, "\n\
Output\n\
  -cit         Generate data for CIt (default)\n\
  -gnuplot     Generate data for GNUPLOT\n\
  -gnuploteps  Generate data for GNUPLOT in Encapsulated Postscript\n\
  -gnuplotps   Generate data for GNUPLOT in Postscript\n\
  -givedy      Give the range of data measurements\n\
  -rate        Generate rate instead of time\n\
  -fname filename             (default is stdout)\n\
               (opened for append, not truncated)\n\
  -noinfo      Do not generate plotter command lines or rate estimate\n\
  -wx i n      windows in x, my # and total #\n\
  -wy i n      windows in y, my # and total #\n\
  -title string Use string as the title instead of the default title\n\
  -lastwindow  generate the wait/new page (always for 1 window)\n" );
}


void HeaderCIt( ctx, protocol_name, title_string, units )
GraphData *ctx;
char *protocol_name, *title_string, *units;
{
char archname[20], hostname[256], date[30], *p;
int dummy;

fprintf( ctx->fp, "set default\nset font variable\n" );
fprintf( ctx->fp, "set curve window y 0.15 0.90\n" );
if (ctx->wxn > 1 || ctx->wyn > 1) 
    fprintf( ctx->fp, "set window x %d %d y %d %d\n", 
	     ctx->wxi, ctx->wxn, ctx->wyi, ctx->wyn );
if (ctx->givedy) 
    /*fprintf( ctx->fp, "set order d d d x y d d d\n" )*/;
else {
    if (ctx->do_rate) 
	fputs( "set order d d d x d y\n", ctx->fp );
    else
	fprintf( ctx->fp, "set order d d d x y d\n" );
}
if (ctx->do_rate) 
    fprintf( ctx->fp, "title left 'Rate (MB/sec)', bottom 'Size %s',\n", 
	     units );
else
    fprintf( ctx->fp, "title left 'time (us)', bottom 'Size %s',\n", units );

strcpy(archname,"MPI" );
MPI_Get_processor_name(hostname,&dummy);
/* Must remove ' from hostname */
p = hostname;
while (*p) {
    if (*p == '\'') *p = ' ';
    p++;
    }
strcpy(date , "Not available" );
if (ctx->title) {
    /* Eventually could replace %D with date, %H with hostname */
    fprintf( ctx->fp, "top '%s'\n", ctx->title );
}
else {
/* For systems without a date routine, just leave off the data */
    if (!date[0] || strcmp( "Not available", date ) == 0) {
	fprintf( ctx->fp, 
		 "      top 'Comm Perf for %s (%s)',\n 'type = %s'\n", 
		 archname, hostname, protocol_name );
    }
    else {
	fprintf( ctx->fp, 
	   "      top 'Comm Perf for %s (%s)',\n 'on %s',\n 'type = %s'\n", 
		 archname, hostname, date, protocol_name );
    }
}
fprintf( ctx->fp, "\n#p0\tp1\tdist\tlen\tave time (us)\trate\n");
fflush( ctx->fp );
}

void HeaderForGopCIt( ctx, test_name, title_string, units )
GraphData *ctx;
char *test_name, *title_string, *units;
{
char archname[20], hostname[256], date[30], *p;
int  dummy;

fprintf( ctx->fp, "set default\nset font variable\n" );
fprintf( ctx->fp, "set curve window y 0.15 0.90\n" );
if (ctx->wxn > 1 || ctx->wyn > 1) 
    fprintf( ctx->fp, "set window x %d %d y %d %d\n", 
	     ctx->wxi, ctx->wxn, ctx->wyi, ctx->wyn );
fprintf( ctx->fp, "title left 'time (us)', bottom 'Processes',\n" );
strcpy(archname,"MPI" );
MPI_Get_processor_name(hostname,&dummy);
/* Must remove ' from hostname */
p = hostname;
while (*p) {
    if (*p == '\'') *p = ' ';
    p++;
    }
strcpy(date , "Not available" );
if (ctx->title) {
    /* Eventually could replace %D with date, %H with hostname */
    fprintf( ctx->fp, "top '%s'\n", ctx->title );
}
else {
/* For systems without a date routine, just leave off the data */
    if (!date[0] || strcmp( "Not available", date ) == 0) {
	fprintf( ctx->fp, 
		 "      top 'Comm Perf for %s (%s)',\n 'type = %s'\n", 
		 archname, hostname, test_name );
    }
    else {
	fprintf( ctx->fp, 
	    "      top 'Comm Perf for %s (%s)',\n 'on %s',\n 'type = %s'\n", 
		 archname, hostname, date, test_name );
    }
}
fprintf( ctx->fp, "\n#np time (us) for various sizes\n");
fflush( ctx->fp );
}

void DataoutGraph( ctx, proc1, proc2, distance, len, t, mean_time, rate,
		   tmean, tmax )
GraphData *ctx;
int     proc1, proc2, distance, len;
double  t, mean_time, rate, tmean, tmax;
{
    if(ctx->givedy) 
	fprintf( ctx->fpdata, "%d\t%d\t%d\t%d\t%f\t%.2f\t%f\t%f\n",
		 proc1, proc2, distance, len, tmean * 1.0e6, rate, 
		 mean_time*1.0e6, tmax * 1.0e6 );
    else
	fprintf( ctx->fpdata, "%d\t%d\t%d\t%d\t%f\t%.2f\n",
		 proc1, proc2, distance, len, mean_time*1.0e6, rate );
}

void DataoutGraphForGop( ctx, len, t, mean_time, rate, tmean, tmax )
GraphData *ctx;
int     len;
double  t, mean_time, rate, tmean, tmax;
{
    fprintf( ctx->fpdata, "%f ", mean_time*1.0e6 );
    fflush( ctx->fpdata );
}

void DataendForGop( ctx )
GraphData *ctx;
{
    fprintf( ctx->fpdata, "\n" );
}

void DatabeginForGop( ctx, np )
GraphData *ctx;
int       np;
{
    fprintf( ctx->fpdata, "%d ", np );
}

void RateoutputGraph( ctx, sumlen, sumtime, sumlentime, sumlen2, sumtime2, 
		     ntest, S, R )
GraphData *ctx;
double  sumlen, sumtime, sumlentime, sumlen2, sumtime2;
int     ntest;
double  *S, *R;
{
    double  s, r;
    double  variance, st;
    PIComputeRate( sumlen, sumtime, sumlentime, sumlen2, ntest, &s, &r );
    s = s * 0.5;
    r = r * 0.5;
/* Do we need to wait until AFTER the variance to scale s, r ? */
    if (ntest > 1) {
	st = sumtime / ntest;
	variance = (1.0 / (ntest - 1.0)) * 
	    (sumtime2 - 2.0 * s * sumtime - 2.0 * r * sumlentime + 
	     ntest * s * s + 2.0 * r * sumlen + r * r * sumlen2 );
    }

    if (ctx->do_fit) {					
	fprintf( ctx->fp, "# Model complexity is (%e + n * %e)\n", s, r );
	fprintf( ctx->fp, "# startup = " );
	if (s > 1.0e-3)
	    fprintf( ctx->fp, "%.2f msec ", s * 1.0e3 );
	else
	    fprintf( ctx->fp, "%.2f usec ", s * 1.0e6 );
	fprintf( ctx->fp, "and transfer rate = " );
	if (r > 1.e-6)
	    fprintf( ctx->fp, "%.2f Kbytes/sec\n", 1.0e-3 / r );
	else
	    fprintf( ctx->fp, "%.2f Mbytes/sec\n", 1.0e-6 / r );
	if (ntest > 1) 
	    fprintf( ctx->fp, "# Variance in fit = %f (smaller is better)\n", 
		     variance );
    }
    *S = s;
    *R = r;
}

void DrawCIt( ctx, first, last, s, r )
GraphData *ctx;
int     first, last;
double  s, r;
{
/* Convert to one-way performance */
    if (ctx->givedy) {
	fprintf( ctx->fp, "set order d d d x y d d d\n" );
	if (ctx->do_rate) 
	    fputs( "set change y 'x * 1.0e-6'\n", ctx->fp );
	fprintf( ctx->fp, "plot\n" );
	fprintf( ctx->fp, "set order d d d x d d y d\n" );
	if (ctx->do_rate) 
	    fputs( "set change y 'x * 1.0e-6'\n", ctx->fp );
	fprintf( ctx->fp, "join\n" );
	fprintf( ctx->fp, "set order d d d x d d d y\n" );
	if (ctx->do_rate) 
	    fputs( "set change y 'x * 1.0e-6'\n", ctx->fp );
	fprintf( ctx->fp, "join\n" );
    }
    else {
	if (ctx->do_rate) 
	    fputs( "set change y 'x * 1.0e-6'\n", ctx->fp );
	fprintf( ctx->fp, "plot square\njoin\n" );
    }
/* fit some times fails in Gnuplot; use the s and r parmeters instead */
/* fit '1'+'x'\njoin dots\n   */
    if (!ctx->do_rate && ctx->do_fit) {
	fprintf( ctx->fp, "set function x %d %d '%f+%f*x'\n", 
		 first, last, s*1.0e6, r*1.0e6 );
	fprintf( ctx->fp, "join dots\n" );
    }
}

void DrawGopCIt( ctx, first, last, s, r, nsizes, sizelist )
GraphData *ctx;
int     first, last, nsizes, *sizelist;
double  s, r;
{
int i, j;
/* Do this in reverse order to help keep the scales correct */
fprintf( ctx->fp, "set limits ymin 0\n" );
for (i=nsizes-1; i>=0; i--) {
    fprintf( ctx->fp, "set order x" );
    for (j=0; j<i; j++)
	fprintf( ctx->fp, " d" );
    fprintf( ctx->fp, " y" );
    for (j=i+1; j<nsizes; j++) 
	fprintf( ctx->fp, " d" );
    fprintf( ctx->fp, "\nplot square\njoin '%d'\n", sizelist[i] );
    }
}

/*
   Redisplay using rate instead of time (not used yet)
 */
void ChangeToRate( ctx, n_particip )
GraphData *ctx;
int     n_particip;
{
    fprintf( ctx->fp, "set order d d d x d d y\njoin\n" );
}

/*
   Generate an end-of-page
 */
void EndPageCIt( ctx )
GraphData *ctx;
{
    if (ctx->is_lastwindow)
	fprintf( ctx->fp, "wait\nnew page\n" );
}

/*
    GNUplot output 
 */
void HeaderGnuplot( ctx, protocol_name, title_string, units )
GraphData *ctx;
char *protocol_name, *title_string, *units;
{
    char archname[20], hostname[256], date[30], *p;
    int  dummy;

    switch (ctx->output_type) {
    case GRF_EPS:
	fprintf( ctx->fp, "set terminal postscript eps\n" );
	break;
    case GRF_PS:
	fprintf( ctx->fp, "set terminal postscript\n" );
	break;
    }
    fprintf( ctx->fp, "set xlabel \"Size %s\"\n", units );
    fprintf( ctx->fp, "set ylabel \"time (us)\"\n" );
    strcpy(archname,"MPI" );
    MPI_Get_processor_name(hostname,&dummy);
/* Must remove ' from hostname */
    p = hostname;
    while (*p) {
	if (*p == '\'') *p = ' ';
	p++;
    }
    strcpy(date , "Not available" );
    if (!date[0] || strcmp( "Not available", date ) == 0) {
	fprintf( ctx->fp, "set title \"Comm Perf for %s (%s) type %s\"\n", 
		 archname, hostname, protocol_name );
    }
    else {
	fprintf( ctx->fp, "set title \"Comm Perf for %s (%s) on %s type %s\"\n", 
		 archname, hostname, date, protocol_name );
    }
    fprintf( ctx->fpdata, "\n#p0\tp1\tdist\tlen\tave time (us)\trate\n");
    fflush( ctx->fp );
}

void HeaderForGopGnuplot( ctx, protocol_name, title_string, units )
GraphData *ctx;
char *protocol_name, *title_string, *units;
{
    char archname[20], hostname[256], date[30], *p;
    int  dummy;

    fprintf( ctx->fp, "set xlabel \"Processes\"\n" );
    fprintf( ctx->fp, "set ylabel \"time (us)\"\n" );

    strcpy(archname,"MPI" );
    MPI_Get_processor_name(hostname,&dummy);
/* Must remove ' from hostname */
    p = hostname;
    while (*p) {
	if (*p == '\'') *p = ' ';
	p++;
    }
    strcpy(date , "Not available" );
    if (!date[0] || strcmp( "Not available", date ) == 0) {
	fprintf( ctx->fp, "set title \"Comm Perf for %s (%s) type %s\"\n", 
		 archname, hostname, protocol_name );
    }
    else {
	fprintf( ctx->fp, 
		 "set title \"Comm Perf for %s (%s) on %s type %s\"\n", 
		 archname, hostname, date, protocol_name );
    }
    fprintf( ctx->fpdata, "\n#np time (us) for various sizes\n");
    fflush( ctx->fp );
}

void DrawGnuplot( ctx, first, last, s, r )
GraphData *ctx;
int     first, last;
double  s, r;
{
    if (ctx->givedy)
	fprintf( ctx->fp, "plot '%s' using 4:5:7:8 with errorbars,\\\n", 
		 ctx->fname2 );
    else {
	fprintf( ctx->fp, "plot '%s' using 4:5 with ", ctx->fname2 );

/* Where is the configure test for this? */    
#ifdef GNUVERSION_HAS_BOXES
	fprintf( ctx->fp, "boxes,\\\n\
'%s' using 4:7 with lines,\\\n", ctx->fname2 );
#else
	fprintf( ctx->fp, "lines,\\\n" );
#endif
    }

    fprintf( ctx->fp, "%f+%f*x with dots\n", 
	     s*1.0e6, r*1.0e6  );
}

void DrawGopGnuplot( ctx, first, last, s, r, nsizes, sizelist )
GraphData *ctx;
int     first, last, nsizes, *sizelist;
double  s, r;
{
int i;

fprintf( ctx->fp, "plot " );
for (i=0; i<nsizes; i++) {
#ifdef GNUVERSION_HAS_BOXES
    fprintf( ctx->fp, "'%s' using 1:%d title '%d' with boxes%s\n\
'%s' using 1:%d with lines,\\\n", ctx->fname2, i+2, sizelist[i], 
	    ctx->fname2, i+2, (i == nsizes-1) ? "" : ",\\" );
#else
    fprintf( ctx->fp, "'%s' using 1:%d title '%d' with lines%s\n", 
	     ctx->fname2, i+2, sizelist[i], (i == nsizes-1) ? "" : ",\\" );
#endif
    }
}

/*
   Generate an end-of-page
 */
void EndPageGnuplot( ctx )
GraphData *ctx;
{
if (ctx->is_lastwindow) {
    if (ctx->output_type == GRF_X) 
	fprintf( ctx->fp, "pause -1 \"Press <return> to continue\"\nclear\n" );
    else
	fprintf( ctx->fp, "exit\n" );
    }
}

/* Common operations */
void HeaderGraph( ctx, protocol_name, title_string, units )
GraphData *ctx;
char *protocol_name, *title_string, *units;
{
(*ctx->header)( ctx, protocol_name, title_string, units );
}

void HeaderForGopGraph( ctx, protocol_name, title_string, units )
GraphData *ctx;
char *protocol_name, *title_string, *units;
{
(*ctx->headergop)( ctx, protocol_name, title_string, units );
}

void DrawGraph( ctx, first, last, s, r )
GraphData *ctx;
int     first, last;
double  s, r;
{
(*ctx->draw)( ctx, first, last, s, r ) ;
}

void DrawGraphGop( ctx, first, last, s, r, nsizes, sizelist )
GraphData *ctx;
int     first, last, nsizes, sizelist;
double  s, r;
{
(*ctx->drawgop)( ctx, first, last, s, r, nsizes, sizelist ) ;
}

void EndPageGraph( ctx )
GraphData *ctx;
{
(*ctx->endpage)( ctx );
}

#define MAX_TSTRING 1024
/* Common create */
void *SetupGraph( argc, argv )
int *argc;
char **argv;
{
GraphData  *new;
char       filename[1024];
int        wsize[2];
int        isgnu;
int        givedy;
static char tstring[MAX_TSTRING];

OutputForm output_type = GRF_X;

new = (GraphData *)malloc(sizeof(GraphData));    if (!new)return 0;;

filename[0] = 0;
isgnu = SYArgHasName( argc, argv, 1, "-gnuplot" );
if (!isgnu) {
    if (isgnu = SYArgHasName( argc, argv, 1, "-gnuploteps" )) {
	output_type = GRF_EPS;
	};
    }
if (!isgnu) {
    if (isgnu = SYArgHasName( argc, argv, 1, "-gnuplotps" )) {
	output_type = GRF_PS;
	};
    }
if (SYArgHasName( argc, argv, 1, "-cit" )) isgnu = 0;
if (SYArgGetString( argc, argv, 1, "-fname", filename, 1024 ) &&
    __MYPROCID == 0) {
    new->fp = fopen( filename, "a" );
    if (!new->fp) {
	fprintf( stderr, "Could not open file %s\n", filename );
	return 0;
	}
    }
else 
    new->fp = stdout;
givedy = SYArgHasName( argc, argv, 1, "-givedy" );

new->do_rate = SYArgHasName( argc, argv, 1, "-rate" );

new->do_fit  = SYArgHasName( argc, argv, 1, "-fit" );

if (SYArgGetString( argc, argv, 1, "-title", tstring, MAX_TSTRING ))
    new->title = tstring;
else
    new->title = 0;

/* Graphics layout */
new->wxi    = 1;
new->wxn    = 1;
new->wyi    = 1;
new->wyn    = 1;
new->givedy = givedy;
if (SYArgGetIntVec( argc, argv, 1, "-wx", 2, wsize )) {
    new->wxi           = wsize[0];
    new->wxn           = wsize[1];
    }
if (SYArgGetIntVec( argc, argv, 1, "-wy", 2, wsize )) {
    new->wyi           = wsize[0];
    new->wyn           = wsize[1];
    }
new->is_lastwindow = SYArgHasName( argc, argv, 1, "-lastwindow" );
if (new->wxn == 1 && new->wyn == 1) new->is_lastwindow = 1;

new->output_type = output_type;
if (!isgnu) {
    new->header	    = HeaderCIt;
    new->dataout    = DataoutGraph;
    new->headergop  = HeaderForGopCIt;
    new->dataoutgop = DataoutGraphForGop;
    new->draw	    = DrawCIt;
    new->drawgop    = DrawGopCIt;
    new->endpage    = EndPageCIt;
    new->fpdata	    = new->fp;
    new->fname2	    = 0;
    }
else {
    char filename2[256];
    new->header	    = HeaderGnuplot;
    new->dataout    = DataoutGraph;
    new->headergop  = HeaderForGopGnuplot;
    new->dataoutgop = DataoutGraphForGop;
    new->draw	    = DrawGnuplot;
    new->drawgop    = DrawGopGnuplot;
    new->endpage    = EndPageGnuplot;
    if (filename[0]) {
	/* Try to remove the extension, if any, from the filename */
	char *p;
	strcpy( filename2, filename );
	p = filename2 + strlen(filename2) - 1;
	while (p > filename2 && *p != '.') p--;
	if (p > filename2)
	    strcpy( p, ".gpl" );
	else
	    strcat( filename2, ".gpl" );
	}
    else {
	strcpy( filename2, "mppout.gpl" );
	}
    new->fpdata	    = fopen( filename2, "a" );
    if (!new->fpdata) {
	fprintf( stderr, "Could not open file %s\n\
used for holding data for GNUPLOT\n", filename2 );
	return 0;
	}
    new->fname2 = (char *)malloc((unsigned)(strlen(filename2 ) + 1 ));
    strcpy( new->fname2, filename2 );
    }
return (void *)new;
}
