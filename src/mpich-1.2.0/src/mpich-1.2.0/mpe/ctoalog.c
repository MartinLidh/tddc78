#include "mpeconf.h"
#include <stdio.h>

int ctoalog();

int main( argc, argv )
int argc;
char *argv[];
{
    int rc;

    if (argc < 2) {
	fprintf(stderr,"usage:  ctoalog <execfilename>;  .clog implied\n");
	exit(-1);
    }
    
    rc = clog2alog( argv[1] );
    if (rc)
	fprintf(stderr,"unsuccessful conversion from clog to alog format\n");
    return(rc);
}
