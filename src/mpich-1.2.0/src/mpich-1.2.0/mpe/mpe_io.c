/*
 * This file contains a start on routines to provide some simple I/O functions
 * for MPE
 */

#include "mpeconf.h"

#include "mpi.h"
#include "mpe.h"
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
/* Prototype for dup2 */
#include <unistd.h>
#endif


#ifndef STDOUT
#define STDOUT 1
#endif

/*@ 
  MPE_IO_Stdout_to_file - Re-direct stdout to a file
 
  Parameters:
+  name - Name of file.  If it contains '%d', this value will be replaced with
  the rank of the process in 'MPI_COMM_WORLD'.

-  mode - Mode to open the file in (see the man page for 'open').  A common
  value is '0644' (Read/Write for owner, Read for everyone else).  
  Note that this
  value is `anded` with your current 'umask' value.

  Notes:
  Some systems may complain when standard output ('stdout') is closed.  
@*/
void MPE_IO_Stdout_to_file( name, mode )
char *name;
int  mode;
{
char fname[1024], *p;
int  rank, fd;

if ((p = strchr(name,'%')) && p[1] == 'd') {
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    sprintf( fname, name, rank );
    fd = open( fname, O_WRONLY | O_CREAT, mode );
    }
else {
    fd = open( name, O_WRONLY | O_CREAT, mode );
    }
dup2( fd, STDOUT );
}
