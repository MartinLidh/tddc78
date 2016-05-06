/*
   State stuff for Upshot.

   Ed Karrels
   Argonne National Laboratory
*/

/*
   States types each have three properties; name, color, and bitmap.
   Each one has a start time, end time, process number, type, parent
   state #, first child state #, and overlapLevel--the number of
   generations down this state is.
*/

#ifndef _STATES_H_
#define _STATES_H_

#include "vis.h"
#include "expandingList.h"


typedef struct stateDefInfo_ {
    char *name, *color, *bitmap;
    int ninst;			/* # of instances of this state */
    int is_active;              /* Allows all uses of this state to be turned
				   off */
} stateDefInfo;


typedef struct stateDefData_ {
  xpandList /*stateDefInfo*/ list;
} stateDefData;

typedef struct stateInfo_ {
  double startTime, endTime;
  int proc, type, parent;
  /* parent is the index of the state in which this state was started,
     -1 if no parent */
  int firstChild;		/* index of firstborn */
  int overlapLevel;		/* nesting level */
} stateInfo;



typedef struct statePost_ {
  double time;
  int type;			/* state type */
  int parent;			/* parent post's final resting place */
				/* will be set by parent if there is one */
				/* -1 otherwise */
  int firstChild;		/* FRP of first child */
  int hangover;			/* slot number of child that is still */
				/* active when I exit */
  int am_hangover;		/* set by my parent to 1 if I am a hangover */
				/* should probably 'orphan' rather than */
				/* 'hangover', but I like 'hangover' */

  int overlapLevel;		/* overlapLevel - # of generations above me */

  /* if this state ends within its parent, this state will add its final
     resting place 
     in the main state list to its parent's 'children' list.  That parent
     will then know who to give the correct index when it ends and gets
     its final resting place.  If my parent's 'children' list does
     not exist yet, store myself as its first child.
     When a state starts, it sets its parent's 'hangover' field to its
     index in the stack.  When the state exits, it sets its parent's
     'hangover' back to -1.  If a state ends and its hangover is not -1,
     it knows to set the 'parent' field for the child at that position
     on the stack.  If a parent exits with a null children list
     the hangover value is its first child.  Whew.  */

  xpandList /*int*/ children;
    /* list of indices of states that started and ended within this state */

} statePost;



/* As states are added or traversed, they should call DrawState, which
   will in turn call each of the functions in the draw list.  This
   is to allow incremental drawing of multiple simultaneous displays */

struct stateData_;


#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif


typedef int drawStateFn ANSI_ARGS(( void *, int idx, int type, int proc,
			       double startTime, double endTime,
			       int parent, int firstChild, int overlapLevel ));

typedef struct drawStateFnList_ {
  drawStateFn *fn;
  void *data;
  struct drawStateFnList_ *next;
} drawStateFnList;

typedef struct stateDrawData_ {
  /* drawRegion region; */
  drawStateFnList *fn_list;
} stateDrawData;


typedef struct stateData_ {
  xpandList /*stateInfo*/ list;    /* master list of state instances */
  xpandList /*statePost*/ *stacks; /* stack of states on each process */
  xpandList /*int*/ idx_start;     /* sorted by start time */
  xpandList /*int*/ idx_end;       /* sorted by end time */
  xpandList /*int*/ *idx_proc_start;
    /* index by process number, sorted by start time */
  xpandList /*int*/ *idx_proc_end;
    /* index by process number, sorted by end time */
  stateDefData defs;		/* state definitions */
  stateDrawData draw;		/* info on how drawing states */
  int np;			/* number of processes */
} stateData;

typedef struct statesVisible_ {
  int *list, n;
} statesVisible;



  /* Create the state record */
stateData *State_Create ANSI_ARGS((void));

  /* Prepare to recieve state instances */
int State_DataInit ANSI_ARGS(( stateData *, int np ));

  /* Call any functions that have asked for state information,
     particularly for drawing. */
int State_Draw ANSI_ARGS(( stateData *, int idx ));

  /* Add a function to the list that want to be called at each state */
void *State_AddDraw ANSI_ARGS(( stateData *, drawStateFn *, void *data ));

  /* Remove function from the list */
int State_RmDraw ANSI_ARGS(( stateData *, void *token ));

  /* Return the # of state definitions */
int State_Ndefs ANSI_ARGS(( stateData * ));

  /* Add state definition, return type num */
int State_AddDef ANSI_ARGS(( stateData *, char *color,
		        char *bitmap, char *name ));

  /* Retrieve a state definition */
int State_GetDef ANSI_ARGS(( stateData *, int n, char **name, char **color,
		        char **bitmap ));

  /* Change a state definition */
int State_SetDef ANSI_ARGS(( stateData *, int n, char *name, char *color,
		        char *bitmap ));

  /* mark the start of a state */
int State_Start ANSI_ARGS(( stateData *, int stateType, int proc,
		       double time ));

  /* mark the end of a state */
int State_End ANSI_ARGS(( stateData *, int stateType, int proc,
		     double time ));

  /* return the number of state instances logged */
int State_N ANSI_ARGS(( stateData * ));

/* return the # of instances logged of the given state type. */
int State_TypeNinst ANSI_ARGS(( stateData*, int typeno ));

  /* Get info on state instance.  If parent is -1, this state had no parent.
     If parent is -2, this state has a parent, but the parent has not
     exited yet.  If nonnegative, parent is the index of the parent's canvas
     ID. */
int State_Get ANSI_ARGS(( stateData *, int stateNum, int *type, int *proc,
		     double *startTime, double *endTime,
		     int *parent, int *firstChild, int *overlapLevel ));

  /* finished adding states, create indices */
int State_DoneAdding ANSI_ARGS(( stateData * ));

  /* Get two lists of all the states that may overlap a given period of time */
int State_GetVis ANSI_ARGS(( stateData *, double time1, double time2,
		        int **list1, int *n1,
		        int **list2, int *n2 ));


  /* free all memory associated with the state data */
int State_Close ANSI_ARGS(( stateData *));

#if TESTING
int State_PrintDefs ANSI_ARGS(( stateData * ));
int State_PrintAll ANSI_ARGS(( stateData * ));
#endif

#endif
