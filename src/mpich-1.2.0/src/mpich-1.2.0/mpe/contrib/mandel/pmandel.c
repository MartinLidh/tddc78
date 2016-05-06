#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "mpi.h"
#include "mpe.h"
#include "pmandel.h"
#include "lists.h"

#include "pm_genproc.h"

#ifndef DEBUG
#define DEBUG 0
#endif

FILE *debug_file;

MPI_Datatype winspecs_type, flags_type, NUM_type, rect_type;

/* Forward refs */
#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif
void DrawImage           ANSI_ARGS(( MPE_XGraph, Winspecs *, Flags * ));
void ProcessArgsFromFile ANSI_ARGS(( MPE_XGraph, Winspecs *, Flags *));
int DragZoom             ANSI_ARGS(( MPE_XGraph, Flags *));

int main( argc, argv )
int argc;
char **argv;
{
  int np, myid, myWindowOpened, allWindowsOpened, masterHalt;
  Winspecs winspecs;
  Flags flags;
  MPE_XGraph graph;
#if DEBUG
  char fileName[50];
#endif

#ifndef SGI_MPI
  if (IsArgPresent( &argc, argv, "-h" )) {
    PrintHelp( argv[0] );
  } 
#endif

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );
  MPI_Comm_size( MPI_COMM_WORLD, &np );

  if (np == 1) {
      fprintf( stderr, 
	      "pmandle requires at least 2 processes (master + slaves)\n" );
      MPI_Finalize();
      return 1;
      }

  if (!myid) {
    if (!getenv( "DISPLAY" )) {
      masterHalt=1;
      MPI_Bcast( &masterHalt, 1, MPI_INT, 0, MPI_COMM_WORLD );
      printf ("DISPLAY environment variable not set.  Exiting.\n\n");
      exit( -1 );
    }
    masterHalt=0;
    MPI_Bcast( &masterHalt, 1, MPI_INT, 0, MPI_COMM_WORLD );
  } else {
    MPI_Bcast( &masterHalt, 1, MPI_INT, 0, MPI_COMM_WORLD );
    if (masterHalt) {
      MPI_Finalize();
      exit( 0 );
    }
  }


  
#if DEBUG
  sprintf( fileName, "pm_debug_%d.out", myid );
  debug_file = fopen( fileName, "w" );
/*
    debug_file = 0;
*/
  if (!debug_file) {
    fprintf( stderr, "Could not open %s, using stderr.\n", fileName );
    debug_file = stderr;
  }
  fflush( debug_file );
#endif

  DefineMPITypes();
  GetDefaultWinspecs( &winspecs );
  GetDefaultFlags( &winspecs, &flags );
  GetWinspecs( &argc, argv, &winspecs );
  GetFlags( &argc, argv, &winspecs, &flags );

  myWindowOpened = (MPE_Open_graphics( &graph, MPI_COMM_WORLD, (char *)0,
				       winspecs.xpos, winspecs.ypos,
				       winspecs.width, winspecs.height, 0 )
		    ==MPE_SUCCESS);
#if DEBUG
  fprintf( debug_file, "[%d] connected? %d\n", myid, myWindowOpened );
  fflush( debug_file );
#endif

  MPI_Allreduce( &myWindowOpened, &allWindowsOpened, 1, MPI_INT, MPI_LAND,
		 MPI_COMM_WORLD );

  if (allWindowsOpened) {
    if (myid == 1) {
	/* Check for movie file flag */
	if (IsArgPresent( &argc, argv, "-movie" )) {
	    int freq = 1;
	    GetIntArg( &argc, argv, "-freq", &freq );
	    MPE_CaptureFile( graph, "mandel_out", freq );
	    }
	}
    if (!winspecs.bw) {
      winspecs.colorArray = (MPE_Color *) malloc( winspecs.numColors * 
						  sizeof( MPE_Color ) );
      MPE_Make_color_array( graph, winspecs.numColors, winspecs.colorArray );
    }
    DrawImage( graph, &winspecs, &flags );
    if (!myid) {
      fprintf( stderr, "Press <Return> to close window\n" );
      while (getchar()!='\n');
    }
    MPI_Barrier( MPI_COMM_WORLD );
    MPE_Close_graphics( &graph );
  } else {
    if (!myid) {
      fprintf( stderr, "One or more processes could not connect\n" );
      fprintf( stderr, "to the display.  Exiting.\n\n" );
     }
    if (myWindowOpened)
      MPE_Close_graphics( &graph );
  }

  MPI_Finalize();
}

void ProcessArgsFromFile( graph, winspecs, oldFlags )
MPE_XGraph graph;
Winspecs *winspecs;
Flags *oldFlags;
{
  Flags newFlags;
  char line[1025], *copy, *tok, **argv;
  int doOneMore, ndrawn, myid, argc;
  xpand_list_Strings *argList;
  FILE *inf;

  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

#if DEBUG
  fprintf( stderr, "%d going into PAFF\n", myid );
#endif

  if (myid == 0) {
    if (!oldFlags->inf || strcmp( oldFlags->inf, "-" ) == 0) {
      inf = stdin;
    } else {
      inf = fopen( oldFlags->inf, "r" );
      if (!inf) {
	fprintf( stderr, "Sorry, could not open %s, skipping.\n",
		oldFlags->inf );
	doOneMore = 0;
	MPI_Bcast( &doOneMore, 1, MPI_INT, 0, MPI_COMM_WORLD );
      }
    }

#if DEBUG
    fprintf( stderr, "%d opened input file\n", myid );
#endif

    ndrawn = 0;

    while( oldFlags->loop || fgets( line, 1024, inf )) {
      if (oldFlags->loop && !fgets( line, 1024, inf )) {
	rewind( inf );
	fgets( line, 1024, inf );
      }
      if (*line!='#' && StrContainsNonWhiteSpace( line )) {
	/* skip blank lines and lines starting with # */
	
	line[strlen( line ) - 1] = 0; /* chop off trailing '\n' */
	argList = Strings_CreateList(10);
	Strings_AddItem( argList, oldFlags->inf );
	tok = strtok( line, " \t" );
	while (tok) {
	  copy = (char *) malloc( sizeof( char ) * strlen( tok ) + 1 );
	  strcpy( copy, tok );
	  Strings_AddItem( argList, copy );
	  tok = strtok( (char *)0, " \t" );
	}
	newFlags = *oldFlags;
	newFlags.inf = (char *)0;
	newFlags.loop = 0;
	newFlags.zoom = 0;
	argc = ListSize( argList );
	argv = ListHeadPtr( argList );
	doOneMore = 1;
	MPI_Bcast( &doOneMore, 1, MPI_INT, 0, MPI_COMM_WORLD );
	GetFlags( &argc, argv, winspecs, &newFlags );
	DrawImage( graph, winspecs, &newFlags );
      }
    }
    doOneMore = 0;
    MPI_Bcast( &doOneMore, 1, MPI_INT, 0, MPI_COMM_WORLD );

  } else {
    MPI_Bcast( &doOneMore, 1, MPI_INT, 0, MPI_COMM_WORLD );
    argc = 0;
    argv = 0;
    while (doOneMore) {
#if DEBUG
	fprintf( stderr, "%d About to do one more loop\n", myid );
#endif
      newFlags = *oldFlags;
      GetFlags( &argc, argv, winspecs, &newFlags );
      DrawImage( graph, winspecs, &newFlags );
      MPI_Bcast( &doOneMore, 1, MPI_INT, 0, MPI_COMM_WORLD );
    }
  }

#if DEBUG
  fprintf( stderr, "%d exiting PAFF\n", myid );
#endif
return;
}



void DrawImage( graph, winspecs, flags )
MPE_XGraph graph;
Winspecs *winspecs;
Flags *flags;
{
  int myid, i, drawAnother;

  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

#if DEBUG
  fprintf( stderr, "%d entering DrawImage (%s)\n", myid, flags->inf );
#endif

  MPI_Barrier( MPI_COMM_WORLD );
  /* helpful when starting up debuggers */

  if (flags->inf) {
#if DEBUG
      fprintf( stderr, "%d about to process args from file\n", myid );
#endif
    ProcessArgsFromFile( graph, winspecs, flags );
  } else {
    
    drawAnother = 0;
    
    do {
#if DEBUG
	fprintf( stderr, "%d in drawing loop\n", myid );
#endif
      MPE_INIT_LOG();
      if (myid == 0) {
	MPE_Fill_rectangle( graph, 0, 0, winspecs->width, winspecs->height,
			   MPE_WHITE );
	fprintf( stdout, "Drawing region -rmin %.17lf -imin %.17lf -rmax %.17lf -imax %.17lf\n",
		NUM2DBL( flags->rmin ), NUM2DBL( flags-> imin ),
		NUM2DBL( flags->rmax ), NUM2DBL( flags-> imax ) );
	MPE_Update( graph );
	SeparateRect_Master( winspecs, flags );
      } else {
	SeparateRect_Slave( graph, winspecs, flags );
      }
      MPE_Update( graph );
      MPI_Barrier( MPI_COMM_WORLD );
#if LOG
      if (!myid && flags->logfile) {
	fprintf( stderr, "Writing logfile\n" );
      }
#endif
      MPE_FINISH_LOG( flags->logfile );
      if (flags->zoom) {
	drawAnother = DragZoom( graph, flags );
      } else {
	sleep( 3 );
	MPI_Barrier( MPI_COMM_WORLD );
      }
    } while (drawAnother);
    

    
  }
  return;
}



int DragZoom( graph, flags )
MPE_XGraph graph;
Flags *flags;
{
  int x1, y1, x2, y2, i, myid, button;
  NUM zx1, zy1, zx2, zy2;
  
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  if (!myid) {
    fprintf( stdout, "Ready for zoom rectangle\n" );
    /*
    printf( "Ready for zoom rectangle; button 2 to quit.\n" );
    MPE_Iget_mouse_press( graph, &x1, &y1, &button, &i );
    if (i && button == MPE_BUTTON2) {
	return 0;
	}
    else
    */
        {
	MPE_Get_drag_region( graph, 1, MPE_DRAG_SQUARE, &x1, &y1, &x2, &y2 );
	if (x1>x2) {i=x1; x1=x2; x2=i;}
	if (y1>y2) {i=y1; y1=y2; y2=i;}
	Pixel2Complex( flags, x1, y1, &zx1, &zy1 );
	Pixel2Complex( flags, x2, y2, &zx2, &zy2 );
	NUM_ASSIGN( flags->rmin, zx1 );
	NUM_ASSIGN( flags->imin, zy2 );
	NUM_ASSIGN( flags->rmax, zx2 );
	NUM_ASSIGN( flags->imax, zy1 );
	/*
	   fprintf( stderr, 
	   "Zooming in on (%d,%d - %d,%d) (%lf,%lf - %lf,%lf)\n",
	   x1, y1, x2, y2, flags->rmin, flags->imin,
	   flags->rmax, flags->imax );
       */
	}
  }

  MPI_Bcast( flags, 1, flags_type, 0, MPI_COMM_WORLD );
  return 1;
}

