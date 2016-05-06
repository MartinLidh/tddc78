#include "test.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_MESSAGE_LENGTH 256
#define MAX_MESSAGE_LENGTH (16*1024*1024)
#define TAG1 1
#define TAG2 2
#define TAG3 3
#define TAG4 4
#define TAGSR 101

void Resetbuf( char *, int );
void Checkbuf( char *, int, MPI_Status * );

void Resetbuf( char *buf, int len )
{
    int i;
    for (i=0; i<len; i++) 
	buf[i] = 0;
}

void Checkbuf( char *buf, int len, MPI_Status *status )
{
    int count, i;
    int err = 0;
    char ival;
    
    MPI_Get_count( status, MPI_CHAR, &count );
    if (count != len) {
	fprintf( stderr, "Got len of %d but expected %d\n", count, len );
	err++;
    }
    ival = 0;
    for (i=0; i<len; i++) {
	if (buf[i] != ival) {
	    err++;
	    fprintf( stderr, 
		     "Found wrong value in buffer[%d] = %d, expected %d\n",
		     i, buf[i], ival );
	    if (err > 10) break;
	}
	ival++;
    }
    if (err) MPI_Abort( MPI_COMM_WORLD, 1 );
}

int main( int argc, char *argv[] )
{
    int msglen, i;
    int msglen_min = MIN_MESSAGE_LENGTH;
    int msglen_max = MAX_MESSAGE_LENGTH;
    int rank,poolsize,Master;
    char *sendbuf,*recvbuf;
    char ival;
    MPI_Request request;
    MPI_Status status;
	
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&poolsize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if(poolsize != 2) {
	printf("Expected exactly 2 MPI processes\n");
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    Master = (rank == 0);	

    if(Master)
	printf("Size (bytes)\n------------\n");
    for(msglen = msglen_min; msglen <= msglen_max; msglen *= 2) {

	sendbuf = malloc(msglen);
	recvbuf = malloc(msglen);
	if(sendbuf == NULL || recvbuf == NULL) {
	    printf("Can't allocate %d bytes\n",msglen);
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}

	ival = 0;
	for (i=0; i<msglen; i++) {
	    sendbuf[i] = ival++;
	    recvbuf[i] = 0;
	}


	if(Master) 
	    printf("%d\n",msglen);
	fflush(stdout);

	MPI_Barrier(MPI_COMM_WORLD);
		
	/* Send/Recv */
	if(Master) 
	    MPI_Send(sendbuf,msglen,MPI_CHAR,1,TAG1,MPI_COMM_WORLD);
	else {
	    Resetbuf( recvbuf, msglen );
	    MPI_Recv(recvbuf,msglen,MPI_CHAR,0,TAG1,MPI_COMM_WORLD,&status);
	    Checkbuf( recvbuf, msglen, &status );
	}

	MPI_Barrier(MPI_COMM_WORLD);

	/* Ssend/Recv */
	if(Master) 
	    MPI_Ssend(sendbuf,msglen,MPI_CHAR,1,TAG2,MPI_COMM_WORLD);
	else {
	    Resetbuf( recvbuf, msglen );
	    MPI_Recv(recvbuf,msglen,MPI_CHAR,0,TAG2,MPI_COMM_WORLD,&status);
	    Checkbuf( recvbuf, msglen, &status );
	}

	MPI_Barrier(MPI_COMM_WORLD);
		
	/* Rsend/Recv */
	if (Master) {
	    MPI_Sendrecv( MPI_BOTTOM, 0, MPI_INT, 1, TAGSR,
			  MPI_BOTTOM, 0, MPI_INT, 1, TAGSR,
			  MPI_COMM_WORLD, &status );
	    MPI_Rsend( sendbuf,msglen,MPI_CHAR,1,TAG3,MPI_COMM_WORLD );
	}
	else {
	    Resetbuf( recvbuf, msglen );
	    MPI_Irecv( recvbuf,msglen,MPI_CHAR,0,TAG3,MPI_COMM_WORLD,&request);
	    MPI_Sendrecv( MPI_BOTTOM, 0, MPI_INT, 0, TAGSR,
			  MPI_BOTTOM, 0, MPI_INT, 0, TAGSR,
			  MPI_COMM_WORLD, &status );
	    MPI_Wait( &request, &status );
	    Checkbuf( recvbuf, msglen, &status );
	}
	    
	MPI_Barrier(MPI_COMM_WORLD);

	/* Isend/Recv - receive not ready */
	if(Master) {
	    MPI_Sendrecv( MPI_BOTTOM, 0, MPI_INT, 1, TAGSR,
			  MPI_BOTTOM, 0, MPI_INT, 1, TAGSR,
			  MPI_COMM_WORLD, &status );
	    MPI_Isend(sendbuf,msglen,MPI_CHAR,1,TAG4,MPI_COMM_WORLD, &request);
	    MPI_Wait( &request, &status );
	}
	else {
	    Resetbuf( recvbuf, msglen );
	    MPI_Sendrecv( MPI_BOTTOM, 0, MPI_INT, 0, TAGSR,
			  MPI_BOTTOM, 0, MPI_INT, 0, TAGSR,
			  MPI_COMM_WORLD, &status );
	    MPI_Recv(recvbuf,msglen,MPI_CHAR,0,TAG4,MPI_COMM_WORLD,&status);
	    Checkbuf( recvbuf, msglen, &status );
	}

	MPI_Barrier(MPI_COMM_WORLD);

	free(sendbuf);
	free(recvbuf);
    }

    if (rank == 0) printf( "Completed long message test\n" );

    MPI_Finalize();
    return 0;
}
