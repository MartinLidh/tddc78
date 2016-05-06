#include "p2p_mon.h"

#define MAXLEN 500
#define MAXPROCS 256

struct globmem {
    int length;
    int a[MAXLEN], b[MAXLEN], c[MAXLEN];
    int num_added[MAXPROCS];
    int sub;
    int slave_id;
    int globid;
    p2p_lock_t go_lock;
    p2p_askfor_monitor_t askfor;
    p2p_barrier_monitor_t barrier;
} *glob;


int getprob(v)			/* return next available subscript */
void *v;
{
int rc = 1;   /* any non-zero means NO problem found */
int *p;

    p = (int *) v;
    if (glob->sub < glob->length)
    {
	*p = glob->sub++;
	rc = 0;    /* FOUND a problem */
    }
    return(rc);
}

void reset()
{
}

slave()
{
    work();
}

int ntotalprocs;

main(argc,argv)
int argc;
char **argv;
{
    int i, j, n, start, end;
    int masterid = getpid();

    printf("size of msemaphore = %d\n",sizeof(struct msemaphore));

    p2p_init(20,8000000);

    glob = (struct globmem *) p2p_shmalloc(sizeof(struct globmem));

    glob->sub = 0;
    glob->slave_id = 0;
    p2p_lock_init(&(glob->go_lock));
    /* p2p_lock(&(glob->go_lock)); */
    p2p_askfor_init(&(glob->askfor));
    p2p_barrier_init(&(glob->barrier));

    /* read in the length and the two vectors */
    scanf("%d",&glob->length);
    for (i = 0; i < glob->length; i++)
	scanf("%d",&glob->a[i]);
    for (i = 0; i < glob->length; i++)
	scanf("%d",&glob->b[i]);
    
    glob->globid = 0;
    ntotalprocs = 3;
    p2p_create_procs(ntotalprocs-1);
    if (masterid != getpid())
    {
	slave();
	exit(0);
    }

    /* p2p_unlock(&(glob->go_lock)); */
    /* start = p2p_clock(); */
    work();
    /* end   = p2p_clock(); */

    for (i = 0; i < glob->length;) 
    {
	for (j = 0; (j < 9) && (i < glob->length); j++)
	    printf("%d\t", glob->c[i++]);
	printf("\n");
    }
    /*****/
    for (i = 0, n = ntotalprocs; i < n; i++)
	printf("num by %d = %d \n",i,glob->num_added[i]);
    /*****/

    p2p_cleanup();
}

work()
{
int i, j, myid, rc;

    p2p_lock(&(glob->go_lock));
    myid = glob->globid++;
    glob->num_added[myid] = 0;
    p2p_unlock(&(glob->go_lock));

    p2p_barrier(&(glob->barrier),ntotalprocs);
    
    while (p2p_askfor(&(glob->askfor),ntotalprocs,getprob,(void *)&i,reset) == 0)
    {
	for (j=0; j < 5000; j++)
	    glob->c[i] = glob->a[i] + glob->b[i];
	/*****/
	glob->num_added[myid]++;
	/*****/
    }
}
