#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "coordinate.h"
#include "definitions.h"
#include "physics.h"
#include "doublylist.c"
#include <math.h>

#define T_STEP 1



int main(int argc, char* argv[]){

  int me, np,i,workload,work2, bufsize,ITERATIONS,INIT_P;
  List list;
  cord_t box;
  long double pressure=0;
  long double tot_pressure=0;

  struct timespec stime, etime;

  particle_t sendUp[MAX_NO_PARTICLES];
  particle_t sendDown[MAX_NO_PARTICLES];
  particle_t recvBuff[MAX_NO_PARTICLES];
  int recvUp, recvDown;
  int upCount=0, downCount=0;
  if(argc!=3)
    {
      printf("invalid number of args");
      return 1;
    }


  sscanf(argv[1], "%d", &ITERATIONS);
  sscanf(argv[2], "%d", &INIT_P);
  printf("iterationsa and init %d    %d \n",ITERATIONS, INIT_P);
  

  MPI_Comm com = MPI_COMM_WORLD;
  MPI_Init( &argc, &argv );
  MPI_Comm_size( com, &np );
  MPI_Comm_rank( com, &me );

  srand((unsigned int)time(NULL)+me);


  MPI_Aint offsets[1] = {0};
  MPI_Datatype oldtypes[1] = {MPI_FLOAT};
  int blockcounts[1] = {4};
  MPI_Datatype MPI_CORD;
  MPI_Type_struct(1, blockcounts,offsets, oldtypes,&MPI_CORD);
  MPI_Type_commit(&MPI_CORD);
  

  MPI_Aint offsets2[2] = {0,sizeof(MPI_CORD)};
  MPI_Datatype oldtypes2[2] = {MPI_CORD,MPI_INT};
  int blockcounts2[2] = {1,1};
  MPI_Datatype MPI_PART;

  MPI_Type_struct(2, blockcounts2,offsets2, oldtypes2,&MPI_PART);
  MPI_Type_commit(&MPI_PART);

 clock_gettime(CLOCK_REALTIME, &stime);


  //root initates box
  if(me == 0){
    box = (cord_t){.x0 = 0,.x1=BOX_HORIZ_SIZE, .y0=0,.y1=BOX_VERT_SIZE};
  }
    
    
  Node * buf = (Node *)malloc(((MAX_NO_PARTICLES)*np)*sizeof(Node));
  bufsize = (MAX_NO_PARTICLES)*np)*sizeof(Node) + 2 * MPI_BSEND_OVERHEAD;
  MPI_Buffer_attach(buf, bufsize);
 
  MPI_Bcast(&box,1,MPI_CORD,0,com);
  workload = (BOX_VERT_SIZE/np);

  MPI_Request sendReqs;
  
  //everyone initiates 10k particles.
  list = (List){NULL,NULL};

  
  for(i = 0; i <INIT_P;i++){
    float x,y,vx,vy,type,pvel,angl;
    
    x = (float)rand()/(float)(RAND_MAX) * BOX_HORIZ_SIZE;
    y = (float)rand()/(float)(RAND_MAX) * workload+workload*me;
    pvel = (float)rand()/(float)(RAND_MAX) * 50;
    angl = (float)rand()/(float)(RAND_MAX) * (2*M_PI);
    
    vx = pvel*cos(angl);
    vy = pvel*sin(angl);
    
    type = 1;

       
    pcord_t pc = {.x = x, .y=y, .vx=vx, .vy=vy};
    particle_t p = {.pcord = pc, .ptype=type};
    Node n = {.particle = p};

    InsertAtTail(&n,&list);
  }

  
  //main loop
  int time_s=0;
  float wall_c = 0;
  while(time_s<ITERATIONS) {
    
    Node *node = list.head;
    while(node!=NULL){
      if(node->particle.last_col != time_s){
	Node *nextNode = node->next;
	while(nextNode != NULL){
	  if(nextNode->particle.last_col != time_s){
	    float t = collide(&(node->particle.pcord),&(nextNode->particle.pcord));
	    interact(&(node->particle.pcord),&(nextNode->particle.pcord),t);
	
	    if(t!=-1)
	      {
		node->particle.last_col = time_s;
		nextNode->particle.last_col = time_s;
		break;
	      }
	  }
	  nextNode = nextNode->next;

	}
      }
      node = node->next;
	
    }
    node = list.head;
    while(node!=NULL){
      if(node->particle.last_col != time_s){
	feuler(&(node->particle.pcord),T_STEP);
	wall_c = wall_collide(&(node->particle.pcord), box);
	if(node->particle.pcord.y < workload*me && me != 0){
	  sendUp[upCount++] = RemoveNode(node, &list)->particle;
	}else if(node->particle.pcord.y > workload*me+workload && me != np-1){
	  sendDown[downCount++] = RemoveNode(node, &list)->particle;
	}
	pressure+= (long double)wall_c;
      }
      node=node->next;
    }
    
    //send and recive particles
    //hejejfnjafndjf
    if(me != 0){
      MPI_Ibsend(&upCount, 1, MPI_INT ,me-1,1,com, &sendReqs);
      MPI_Ibsend(sendUp, upCount, MPI_PART,me-1,2,com, &sendReqs);
    }
    if(me != np-1){
      MPI_Ibsend(&downCount, 1, MPI_INT ,me+1,3,com, &sendReqs);
      MPI_Ibsend(sendDown, downCount, MPI_PART,me+1,4,com, &sendReqs );
    }
    
    if(me != 0){
      MPI_Recv(&recvUp, 1, MPI_INT, me-1,3,com,MPI_STATUS_IGNORE);
      MPI_Recv(recvBuff,recvUp,MPI_PART,me-1,4,com,MPI_STATUS_IGNORE);
    }
    if(me!= np-1){
      MPI_Recv(&recvDown, 1, MPI_INT, me+1,1,com,MPI_STATUS_IGNORE);
      MPI_Recv(&recvBuff[recvUp],recvDown,MPI_PART,me+1,2,com,MPI_STATUS_IGNORE);
    }
    for(i=0;  i < (recvUp+recvDown); i++){
      Node n = {.particle = recvBuff[i]};
      InsertAtTail(&n,&list);
    }
    upCount=0;
    downCount=0;
    time_s++;
  }

  MPI_Reduce(&pressure,&tot_pressure,1,MPI_LONG_DOUBLE,MPI_SUM,0,com);
  if(me==0)
    {
      tot_pressure = tot_pressure/ITERATIONS/WALL_LENGTH;
      printf("preasure: %LF\n",tot_pressure);
      clock_gettime(CLOCK_REALTIME, &etime);
      printf("Runtime was: %2g secs\n", (etime.tv_sec  - stime.tv_sec) +
	     1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;
    }
  



 


  MPI_Finalize();
  

}
