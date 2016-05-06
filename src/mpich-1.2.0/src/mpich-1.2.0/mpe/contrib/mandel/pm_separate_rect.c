#include <stdio.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <sys/time.h>
#include "pmandel.h"
#include "pm_genproc.h"

#ifndef DEBUG
#define DEBUG 0
#endif
#define MASTER_PROC 0
#define DEBUG_ASSIGNMENTS 0

void SeparateRect_Master( winspecs, flags )
Winspecs *winspecs;
Flags *flags;
{
  rect *assigList, recvRectBuf[2], tempRect;
  /* assigList - list of what rectangles have been assigned to each process */
  /* recvRectBuf - when a slave process tells the master that some rectangles
     need to be calculated, the rectangle definitions are temporarily stored
     here */

  int inProgress, nidle, *idleList, splitPt, np, data,
      mesgTag, firstToEnqueue, procNum, i, randomPt;
  /* inProgress - number of rectangles currently under construction */
  /* nidle - number of idle processes */
  /* idleList - list of process ranks that are idle */
  /* splitPt - point at which to split the Mandelbrot set--can't have the
     border algorithm enclosing the whole set */
  /* np - number of processes */
  /* data - one int returned by slave processes; different uses */
  /* mesgTag - the tag of the received message */
  /* firstToEnqueue - when receiving a bunch of rectangles to enqueue, the
     master process may send some off to idle processes right away before
     queueing them.  firstToEnqueue points to the first one that is actually
     queued */
  /* procNum - rank of the process that sent the last message */
  /* randomPt - index+1 of the last item in the queue that has not been
                placed in random order */

  rect_queue rect_q;
  /* queue of rectangles to calculate */

  MPI_Status mesgStatus;

  struct timeval time;

  MPI_Comm_size( MPI_COMM_WORLD, &np );
  gettimeofday( &time, 0 );
    /* initialize the random number generator for the -randomize option */
  srand48( time.tv_sec * time.tv_usec );

  MPE_DESCRIBE_STATE( S_COMPUTE, E_COMPUTE,
		     "Compute", "blue:gray" );
  MPE_DESCRIBE_STATE( S_DRAW_BLOCK, E_DRAW_BLOCK,
		     "Draw block", "yellow:gray3" );
  MPE_DESCRIBE_STATE( S_DRAW_RECT, E_DRAW_RECT,
		     "Draw border", "green:light_gray" );
  MPE_DESCRIBE_STATE( S_WAIT_FOR_MESSAGE, E_WAIT_FOR_MESSAGE,
		     "Wait for message", "red:boxes" );
  MPE_DESCRIBE_STATE( S_DRAW_CHUNK, E_DRAW_CHUNK, "Draw Chunk",
		     "steelBlue:2x2" );

  assigList  = (rect *) malloc( (np) * sizeof( rect ) );
  idleList   = (int *)  malloc( (np) * sizeof( rect ) );

  nidle = inProgress = 0;
  Q_Create( &rect_q, flags->randomize );
    /* create the queue */

  if (flags->imin<0 && flags->imax>0) { /* might have to split the set */
    splitPt = winspecs->height +
      NUM2INT( NUM_DIV( NUM_MULT( flags->imin, INT2NUM( winspecs->height ) ),
				    NUM_SUB( flags->imax, flags->imin ) ) );
    RECT_ASSIGN( tempRect, 0, winspecs->width-1, 0, splitPt-1 );
    tempRect.length = RectBorderLen( &tempRect );
    Q_Enqueue( &rect_q, &tempRect );
    RECT_ASSIGN( tempRect, 0, winspecs->width-1, splitPt, winspecs->height-1 );
    tempRect.length = RectBorderLen( &tempRect );
    Q_Enqueue( &rect_q, &tempRect );
  } else {
    RECT_ASSIGN( tempRect, 0, winspecs->width-1, 0, winspecs->height-1 );
    tempRect.length = RectBorderLen( &tempRect );
    Q_Enqueue( &rect_q, &tempRect );
  }

#if DEBUG
  fprintf( debug_file, "Master starting up\n" );
  fflush( debug_file );
#endif

  while (inProgress || !IS_Q_EMPTY(rect_q) ) {
    /* while someone's working, and the q is !empty */

    MPE_LOG_EVENT( S_WAIT_FOR_MESSAGE, 0, 0 );
    MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &mesgStatus );
    MPE_LOG_RECEIVE( mesgStatus.MPI_SOURCE, mesgStatus.MPI_TAG, 0 );
    MPE_LOG_EVENT( E_WAIT_FOR_MESSAGE, 0, 0 );

    procNum = mesgStatus.MPI_SOURCE;
    mesgTag = mesgStatus.MPI_TAG;

#if DEBUG
    fprintf( debug_file, "Master receives %d from %d\n", mesgTag, procNum );
    fflush( debug_file );
#endif

    switch (mesgTag) {

    case READY_TO_START:
      inProgress++;
    case READY_FOR_MORE:
      MPI_Recv( 0, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
	        MPI_COMM_WORLD, &mesgStatus );
      if (IS_Q_EMPTY(rect_q)) {		      /* if the queue is empty, */
	idleList[nidle++] = procNum;  /* remember this process was left idle */
	inProgress--;
      } else {
	Q_Dequeue( &rect_q, &tempRect );
#if DEBUG_ASSIGNMENTS
	fprintf( stderr, "about to assign (%d,%d - %d,%d) to %d\n",
		tempRect.l, tempRect.t,
		tempRect.r, tempRect.b, procNum );
	while (getchar()!='\n');
#endif
	MPI_Send( &tempRect, 1, rect_type,
		 procNum, ASSIGNMENT, MPI_COMM_WORLD );
	MPE_LOG_SEND( procNum, ASSIGNMENT, sizeof( rect ) );
	  /* otherwise, assign it the next in the queue */
	assigList[procNum] = tempRect;
	  /* remember which rect this process is on */
#if DEBUG
	fprintf( debug_file, "Master assigns (%d %d %d %d) to %d\n",
		assigList[procNum].l,
		assigList[procNum].r, assigList[procNum].t,
		assigList[procNum].b,
		procNum );
	fflush( debug_file );
#endif
      }
      break;

    case ADD2Q:
        /* slave is posting more rectangles to be queued */
      MPI_Recv( recvRectBuf, 2, rect_type, procNum,
	        ADD2Q, MPI_COMM_WORLD, &mesgStatus );
#if DEBUG
	fprintf( debug_file, "Master receives (%d %d %d %d)\n",
		(recvRectBuf[0]).l,
		(recvRectBuf[0]).r,
		(recvRectBuf[0]).t,
		(recvRectBuf[0]).b );
	fprintf( debug_file, "Master receives (%d %d %d %d)\n",
		(recvRectBuf[1]).l,
		(recvRectBuf[1]).r,
		(recvRectBuf[1]).t,
		(recvRectBuf[1]).b );
	fflush( debug_file );
#endif
        /* get rect definitions */
      firstToEnqueue = 0;
      while (nidle && firstToEnqueue < 2) {
	  /* if processes are idle, */	
	nidle--;
	assigList[idleList[nidle]] = recvRectBuf[firstToEnqueue];
	  /* remember which rect this process is on */
	MPI_Send( recvRectBuf + firstToEnqueue, 1, rect_type,
		  idleList[nidle], ASSIGNMENT, MPI_COMM_WORLD );
	MPE_LOG_SEND( idleList[nidle], ASSIGNMENT, sizeof( rect ) );
	  /* give them something to do */
#if DEBUG
	fprintf( debug_file, "Master assigns (%d %d %d %d) to %d\n",
		(recvRectBuf[firstToEnqueue]).l,
		(recvRectBuf[firstToEnqueue]).r,
		(recvRectBuf[firstToEnqueue]).t,
		(recvRectBuf[firstToEnqueue]).b, idleList[nidle] );
	fflush( debug_file );
#endif
	inProgress++; firstToEnqueue++;
      }
      for (; firstToEnqueue<2; firstToEnqueue++) {
	Q_Enqueue( &rect_q, recvRectBuf+firstToEnqueue );
#if DEBUG
	fprintf( debug_file, "Master queues (%d %d %d %d)\n",
		(recvRectBuf[firstToEnqueue]).l,
		(recvRectBuf[firstToEnqueue]).r,
		(recvRectBuf[firstToEnqueue]).t,
		(recvRectBuf[firstToEnqueue]).b );
	fflush( debug_file );
#endif
      }
      break;
    }
  }

#if DEBUG
  fprintf( debug_file, "All done\n" );
  fflush( debug_file );
#endif

  for (i=1; i<np; i++) {							 /* tell everyone to exit */
    MPI_Send( rect_q.r, sizeof( rect ), MPI_BYTE, i, ALL_DONE, MPI_COMM_WORLD );
    MPE_LOG_SEND( i, ALL_DONE, sizeof( rect ) );
  }
}




void SeparateRect_Slave( graph, winspecs, flags )
MPE_XGraph graph;
Winspecs *winspecs;
Flags *flags;
{
  int x, y, working, isContinuous, *borderData, *datPtr, mesgTag, myid,
      dataSize, *iterData, npoints;
  /* x, y - integer counters for the point being calculated */
  /* working - whether this process is still working or has been retired */
  /* isContinuous - whether the border just computed is one continuous color */
  /* borderData - storage for the values calculated */
  /* datPtr - pointer into borderData */
  /* mesgTag - what type of message was just received */

  NUM rstep, istep;
  MPE_Point *pointData;

  rect r, rectBuf[2];
  /* r - the rectangle being calculated */

  MPI_Status mesgStatus;

  MPI_Comm_rank( MPI_COMM_WORLD, &myid );

  MPI_Send( 0, 0, MPI_INT, MASTER_PROC, READY_TO_START, MPI_COMM_WORLD );
  MPE_LOG_SEND( MASTER_PROC, READY_TO_START, 0 );

#if DEBUG
  fprintf( debug_file, "[%d]ready for duty\n", myid );
  fflush( debug_file );
#endif

  working = 1;
  NUM_ASSIGN( rstep,  NUM_DIV( NUM_SUB( flags->rmax, flags->rmin ),
			       INT2NUM( winspecs->width-1 ) ) );
  NUM_ASSIGN( istep,  NUM_DIV( NUM_SUB( flags->imin, flags->imax ),
			       INT2NUM( winspecs->height-1 ) ) );


  /* figure out how much data might be stored and allocate space for it */
  x = flags->breakout * flags->breakout;
  y = 2 * (winspecs->height + winspecs->width);
  dataSize = ((y>x) ? y : x);
  iterData = (int *) malloc( dataSize * sizeof( int ) );
  pointData = (MPE_Point *) malloc( dataSize * sizeof( MPE_Point ) );

  Fract_SetRegion( flags->rmin, flags->rmax, flags->imin, flags->imax, 0,
		   winspecs->width-1, 0, winspecs->height-1 );
  
/*
  fprintf( stderr, "fractal = %d, MBROT = %d\n", flags->fractal, MBROT );
*/

  switch (flags->fractal) {
  case MBROT:
    Mbrot_Settings( flags->boundary_sq, flags->maxiter );
    break;
  case JULIA:
    Julia_Settings( flags->boundary_sq, flags->maxiter,
		    flags->julia_r, flags->julia_i );
    break;
  case NEWTON:
/*
    Newton_Settings( flags->epsilon, flags->newton.coeff,
		     flags->newton.nterms );
*/
    Mbrot_Settings( flags->boundary_sq, flags->maxiter );
    break;
  }

  while (working) {
    MPE_LOG_EVENT( S_WAIT_FOR_MESSAGE, 0, 0 );
    MPI_Recv( &r, 1, rect_type, MASTER_PROC, MPI_ANY_TAG,
	      MPI_COMM_WORLD, &mesgStatus );
    MPE_LOG_RECEIVE( MASTER_PROC, mesgStatus.MPI_TAG, sizeof( rect ) );
      /* get command from master process */
    MPE_LOG_EVENT( E_WAIT_FOR_MESSAGE, 0, 0 );
    mesgTag = mesgStatus.MPI_TAG;

#if DEBUG
    fprintf( debug_file, "receive: %d\n", mesgTag);
    fflush( debug_file );
#endif

    switch (mesgTag) {

    case ASSIGNMENT:		/* new rectangle to compute */
#if DEBUG
      fprintf( debug_file, "Assigned (%d %d %d %d)\n", r.l, r.r, r.t, r.b );
      fflush( debug_file );
#endif
      if (r.b-r.t<flags->breakout || r.r-r.l<flags->breakout) {
	/* if smaller than breakout, compute directly */

#if DEBUG
	fprintf( debug_file, "[%d]computing chunk\n", myid );
	fflush( debug_file );
#endif
	MPE_LOG_EVENT( S_COMPUTE, 0, 0 );
	ComputeChunk( flags, &r, pointData, iterData, dataSize, &npoints );
	MPE_LOG_EVENT( E_COMPUTE, 0, 0 );

	MPI_Send( 0, 0, MPI_INT, MASTER_PROC, READY_FOR_MORE,
		  MPI_COMM_WORLD );
	MPE_LOG_SEND( MASTER_PROC, READY_FOR_MORE, 0 );

	MPE_LOG_EVENT( S_DRAW_CHUNK, 0, 0 );
#if DEBUG
	fprintf( debug_file, "[%d]drawing chunk\n", myid );
	fflush( debug_file );
#endif
	MPE_Draw_points( graph, pointData, npoints );
	MPE_Update( graph );
	MPE_LOG_EVENT( E_DRAW_CHUNK, 0, 0 );
      } else {			/* otherwise, compute the boundary */

	MPE_LOG_EVENT( S_COMPUTE, 0, 0 );
#if DEBUG
	fprintf( debug_file, "[%d]computing border\n", myid );
	fflush( debug_file );
#endif
	ComputeBorder( winspecs, flags, &r, pointData,
		       dataSize, &npoints, &isContinuous );
#if DEBUG>1
	{
	  int i;
	  fprintf( debug_file, "computed %d %s points\n", npoints,
		   isContinuous ? "continuous" : "noncontinuous");
	  for (i=0; i<npoints; i++) {
	    fprintf( debug_file, "check computed (%d %d) %d\n", pointData[i].x,
		     pointData[i].y, pointData[i].c );
	  }
	}
#endif

	MPE_LOG_EVENT( E_COMPUTE, 0, 0 );

	if (!isContinuous) {
#if DEBUG
	  fprintf( debug_file, "[%d]splitting and sending to master\n", myid );
	  fflush( debug_file );
#endif
	  SplitRect( flags, r );
      }
	MPI_Send( 0, 0, MPI_INT, MASTER_PROC, READY_FOR_MORE,
		  MPI_COMM_WORLD );
	MPE_LOG_SEND( MASTER_PROC, READY_FOR_MORE, 0 );

	if (isContinuous) {
          MPE_LOG_EVENT( S_DRAW_BLOCK, 0, 0 );
#if DEBUG
	  fprintf( debug_file, "[%d]drawing block\n", myid );
	  fflush( debug_file );
#endif
	  DrawBlock( graph, pointData, &r );
          MPE_LOG_EVENT( E_DRAW_BLOCK, 0, 0 );
	} else {
	  MPE_LOG_EVENT( S_DRAW_RECT, 0, 0 );
#if DEBUG
	  fprintf( debug_file, "[%d]drawing border\n", myid );
	  fflush( debug_file );
#endif
#if DEBUG>2
	  {
	    int i;
	    for (i=0; i<npoints; i++) {
	      fprintf( debug_file, "drawing (%d %d) %d\n",
		       pointData[i].x, pointData[i].y, pointData[i].c );
	    }
	  }
#endif
	  MPE_Draw_points( graph, pointData, npoints );
	  MPE_Update( graph );
	  MPE_LOG_EVENT( E_DRAW_RECT, 0, 0 );
	}

      }	/* else !breakout */
      break;			      /* end if case ASSIGNMENT: */

    case ALL_DONE:
#if DEBUG
      fprintf( debug_file, "[%d]all done\n", myid );
      fflush( debug_file );
#endif
      working=0;
      break;

    } /* end of switch */
  } /* end of while (working) */
}


SplitRect( flags, r )
Flags *flags;
rect r;
{
  int xsplit, ysplit, numRect;
  rect rectBuf[2];
  
  xsplit = (r.r-r.l)>>1;
  ysplit = (r.b-r.t)>>1;
  if (xsplit>ysplit) {		/* split the long side */
    RECT_ASSIGN( rectBuf[0], r.l+1, r.l+xsplit, r.t+1, r.b-1 );
    RECT_ASSIGN( rectBuf[1], r.l+xsplit+1, r.r-1, r.t+1, r.b-1 );
  } else {
    RECT_ASSIGN( rectBuf[0], r.l+1, r.r-1, r.t+1, r.t+ysplit );
    RECT_ASSIGN( rectBuf[1], r.l+1, r.r-1, r.t+ysplit+1, r.b-1 );
  }
  rectBuf[0].length = RectBorderLen( rectBuf );
  rectBuf[1].length = RectBorderLen( rectBuf+1 );
  MPI_Send( rectBuf, 2, rect_type, MASTER_PROC,
	    ADD2Q, MPI_COMM_WORLD );
  MPE_LOG_SEND( MASTER_PROC, ADD2Q, sizeof( rect ) * 2 );
    /* send the rectangles */
#if DEBUG
  fprintf( debug_file, "Sent master (%d %d %d %d and %d %d %d %d)\n",
	   rectBuf[0].l, rectBuf[0].t, rectBuf[0].r, rectBuf[0].b, 
	   rectBuf[1].l, rectBuf[1].t, rectBuf[1].r, rectBuf[1].b );
  fflush( debug_file );
#endif
}

