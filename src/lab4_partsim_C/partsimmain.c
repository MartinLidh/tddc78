#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "coordinate.h"
#include "definitions.h"
#include "physics.h"
#include "doublylist.c"



int main(int argc, char* argv[]){

  int me, np;
  List list;

     
  MPI_Comm com = MPI_COMM_WORLD;
  MPI_Init( &argc, &argv );
  MPI_Comm_size( com, &np );
  MPI_Comm_rank( com, &me );
  
  
  
  
  while(1)
    {
      
      
      
    }
  

}
