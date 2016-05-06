#include <stdio.h>
#include "mpi.h"

int main(int argc,char *argv[])
{

    int a[8]={1,2,3,4,5,6,7,8};
    int rcounts[4]={2,2,2,2};
    int disp[4]={0,2,4,6};
    int b[8]={0,0,0,0,0,0,0,0};
    int procid;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&procid);

    MPI_Scatterv(&a, &rcounts, disp, MPI_INT, &b, 100, MPI_INT, 0, MPI_COMM_WORLD);
    printf("%d got element %d and %d\n",procid,b[0],b[1]);
    MPI_Finalize();
    return 0;
}    
