#ifndef _MPPTEST
#define _MPPTEST

/* Definitions for pair-wise communication testing */
typedef struct {
    int  proc1, proc2;
    int  source, destination,    /* Source and destination.  May be the
				    same as partner (for pair) or different
				    (for ring) */
         partner;                /* = source = destination if same */
    int  is_master, is_slave;
    } PairData;

/* Structure for the collective communication testing */
typedef struct {
    MPI_Comm pset;       /* Procset to test over */
    int     src;         /* Source (for scatter) */
    } GOPctx;

#define NO_NBR -1

/* size of the job and my rank in MPI_COMM_WORLD */
extern int __NUMNODES, __MYPROCID;

/* Function prototypes */
void *PairInit( int, int );
void *BisectInit( int );
void PairChange( int, PairData * );
void BisectChange( int, PairData * );
void *GOPInit( int *, char ** );
void RunATest( int, int*, int*, double *, double *, int *, double (*)(), 
	       int, int, int, int, void *, void *);
void CheckTimeLimit( void );
void *OverlapInit();

double (*GetPairFunction())(int, int, PairData *);
double (*GetGOPFunction( int*, char **, char *, char *))( int, int, GOPctx *);
double memcpy_rate( int, int, PairData *);

/* Overlap testing */
double round_trip_nb_overlap();
double round_trip_b_overlap();

/* Routine to generate graphics context */
void *SetupGraph( int *, char *[] );

/* Global operations */
void PrintGOPHelp( void );

/* Patterns */
void PrintPatternHelp( void );

/* Prototypes */
double RunSingleTest();
void time_function();
void ClearTimes(void);


#endif
