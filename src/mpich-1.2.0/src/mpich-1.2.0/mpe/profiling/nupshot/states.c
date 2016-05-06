/*
   State stuff for Upshot.

   Ed Karrels
   Argonne National Laboratory
*/

#include <string.h>
#include "vis.h"
#include "states.h"
#include "str_dup.h"
#include "bitmaps.h"
#include "colors.h"


#define DEBUG 0
#define TESTING 0
#define USE_QSORT 1



  /* store the starting end of a state */
static int PushState ANSI_ARGS(( stateData *state_data, int proc, int type,
		      double time ));
static int FindMatchingPost ANSI_ARGS(( stateData*, int proc, int type ));
static int GetPostInfo ANSI_ARGS(( stateData*, int proc, int slot_no, stateInfo* ));
static int ClosePost ANSI_ARGS(( stateData*, int proc, int slot_no,
		      int finalRestingPlace ));
static int CreateStateIndices ANSI_ARGS(( stateData *state_data ));
static int SortStart ANSI_ARGS(( stateData *state_data, int *a, int n ));
static int SortEnd ANSI_ARGS(( stateData *state_data, int *a, int n ));


stateData *State_Create()
{
  stateData *state_data;

  state_data = (stateData*)malloc( sizeof( stateData ) );
  if (!state_data) {
    fprintf( stderr, "Out of memory in State_Create()\n" );
    return 0;
  }
  ListCreate( state_data->defs.list, stateDefInfo, 10 );
  ListCreate( state_data->list, stateInfo, 100 );

  state_data->stacks	     = 0;
  state_data->idx_start	     = 0;
  state_data->idx_end	     = 0;
  state_data->idx_proc_start = 0;
  state_data->idx_proc_end   = 0;
  state_data->draw.fn_list   = 0;

  return state_data;
}


int State_DataInit( state_data, np )
stateData *state_data;
int np;
{
  int proc;

  state_data->np = np;

  state_data->stacks = (xpandList *)
    malloc( sizeof(statePost) * np );
  for (proc=0; proc<np; proc++) {
    ListCreate( state_data->stacks[proc], statePost, 3 );
  }

  state_data->idx_start = state_data->idx_end = 0;
  state_data->idx_proc_start = state_data->idx_proc_end = 0;

  return 0;
}


int State_AddDef( state_data, color, bitmap, name )
stateData *state_data;
char *color, *bitmap, *name;
{
  stateDefInfo stateDef;

#if 0

    /* go through the list of state definitions already present */
  defptr = ListHeadPtr( state_data->defs.list, stateDefInfo );
  n = ListSize( state_data->defs.list, stateDefInfo );
  for (i=0; i<n; i++,defptr++) {
      /* if a state with the same name has already been defined,
         ignore this definition */
    if (!strcmp( name, defptr->name )) return 1;
  }

  /* don't do this.  you'll just have to live with multiple states with
     the same name.  The logfile reader should take care of anything
     that truly is duplicated.  BTW, this causes problems with logfiles
     like:

     -13 0 5 6 0 0 yellow: look
     -13 0 7 8 0 0 orange: look

     while it tries to fix logfiles like:

     -13 0 1 100 0 0 : state_1
     -13 0 1 100 0 0 : state_1

  */

#endif

    /* copy the name of the state */
  stateDef.name = STRDUP( name );

    /* if the color is null or a zero-length string,
       pick a color */
  if (!color || !*color) {
    stateDef.color = STRDUP( Color_Get() );
  } else {
    stateDef.color = STRDUP( color );
  }

    /* same deal for the bitmap */
  if (!bitmap || !*bitmap) {
    stateDef.bitmap = STRDUP( Bitmap_Get() );
  } else {
    stateDef.bitmap = STRDUP( bitmap );
  }

    /* no instances yet */
  stateDef.ninst     = 0;
  stateDef.is_active = 0;

    /* add this state definition to the list */
  ListAddItem( state_data->defs.list, stateDefInfo, stateDef );

  return ListSize( state_data->defs.list, stateDefInfo )-1;
}


int State_GetDef( state_data, n, name, color, bitmap )
stateData *state_data;
int n;
char **color, **bitmap, **name;
{
    stateDefInfo *def;

    /* create temporary pointer to the state definitions we want */
    def	    = ListHeadPtr( state_data->defs.list, stateDefInfo ) + n;

    *color  = def->color;
    *bitmap = def->bitmap;
    *name   = def->name;

    if (def->is_active) return 0;
    else return 1;
}

  /* Change a state definition */
int State_SetDef( state_data, n, color, bitmap, name )
stateData *state_data;
int n;
char *color, *bitmap, *name;
{
  stateDefInfo *def;

    /* create temporary pointer to the state definitions we want */
  def = ListHeadPtr( state_data->defs.list, stateDefInfo ) + n;

    /* if already set, deallocate memory */
  if (def->color)  free( def->color );
  if (def->bitmap) free( def->bitmap );
  if (def->name)   free( def->name );

    /* make copies of the data */
  def->color  = STRDUP( def->color );
  def->bitmap = STRDUP( def->bitmap );
  def->name   = STRDUP( def->name );

  return 0;
}


/* return the number of states definitions */
int State_Ndefs( state_data )
stateData *state_data;
{
  return ListSize( state_data->defs.list, stateDefInfo );
}


#if TESTING
int State_PrintDefs( state_data )
stateData *state_data;
{
  stateDefInfo *def;
  int ndefs, i;

  def = ListHeadPtr( state_data->defs.list, stateDefInfo );
  ndefs = ListSize( state_data->defs.list, stateDefInfo );

  fprintf( stderr, "State definitions:\n" );
  for (i=0; i<ndefs; i++,def++) {
    fprintf( stderr, "State def %d: %s %s %s\n", i, def->name, def->color,
	    def->bitmap );
  }
  return 0;
}
#endif


int State_Start( state_data, stateType, proc, time )
stateData *state_data;
int stateType, proc;
double time;
{
  PushState( state_data, proc, stateType, time );
  return 0;
}


int State_End( state_data, stateType, proc, time )
stateData *state_data;
int stateType, proc;
double time;
{
  int slot_no;
  stateInfo info;

    /* look for a matching post, return -1 if none found */
  slot_no = FindMatchingPost( state_data, proc, stateType );
  if (slot_no == -1) {
    /* no start event found */
    fprintf( stderr, "End state %s before beginning at %f on proc %d.\n",
	    ListItem( state_data->defs.list, stateDefInfo, stateType ).name,
	    time, proc );
    return -1;
  } else {
      /* end event found; add to state list */
      /* get the rest of the information about the post */
    GetPostInfo( state_data, proc, slot_no, &info );
    info.type = stateType;
    info.endTime = time;
    info.proc = proc;
      /* update the parent/firstChild stuff, and free memory */
      /* to do the parent stuff, need to know the final resting place,
         or the slot in the state_data->list array in which this
	 state will be stored.  ListSize() provides this. */
    ClosePost( state_data, proc, slot_no, 
	       ListSize( state_data->list, stateInfo ) );

    ListAddItem( state_data->list, stateInfo, info );
      /* add one to the # of instances of this state */
    ListItem( state_data->defs.list, stateDefInfo, stateType ).ninst++;
      /* make the state active */
    ListItem( state_data->defs.list, stateDefInfo, stateType ).is_active = 1;

    /*
      State_Draw( state_data, ListSize( state_data->list, stateInfo )-1 );
    */
    return 0;
  }

}


int State_DoneAdding( state_data )
stateData *state_data;
{
  int proc;

  /* release current state infomation */
  /* Some states may be left open, deal with this later */
  for (proc=0; proc<state_data->np; proc++) {
    ListDestroy( state_data->stacks[proc], statePost );
  }
  free( state_data->stacks );

  ListShrinkToFit( state_data->list, stateInfo );

  State_Draw( state_data, -1 );

  /* create various indices */
  CreateStateIndices( state_data );

  return 0;
}


int State_N( data )
stateData *data;
{
  return ListSize( data->list, stateInfo );
}


/*
   Return the # of instances of the given state type.
*/
int State_TypeNinst( data, typeno )
stateData *data;
int typeno;
{
  return ListItem( data->defs.list, stateDefInfo, typeno ).ninst;
}



int State_Get( state_data, n, type, proc, startTime, endTime,
	       parent, firstChild, overlapLevel )
stateData *state_data;
int n, *type, *proc, *parent, *firstChild, *overlapLevel;
double *startTime, *endTime;
{
  stateInfo *s;

    /* get temp pointer to the structure */
  s = ListHeadPtr( state_data->list, stateInfo ) + n;

    /* copy each field */
  *type         = s->type;
  *proc         = s->proc;
  *startTime    = s->startTime;
  *endTime      = s->endTime;
  *parent       = s->parent;
  *firstChild   = s->firstChild;
  *overlapLevel = s->overlapLevel;

  return 0;
}


int State_Close( data )
stateData *data;
{
  int proc, def;
  drawStateFnList *node, *last;
  
  ListDestroy( data->list, stateInfo );

    /* if indices have already been created, remove them */
  if (data->idx_start) {
    for (proc=0; proc < data->np; proc++) {
      ListDestroy( data->idx_proc_start[proc], int );
      ListDestroy( data->idx_proc_end[proc], int );
    }
    free( data->idx_proc_start );
    free( data->idx_proc_end );
    
    ListDestroy( data->idx_start, int );
    ListDestroy( data->idx_end, int );
  }

    /* free linked list of draw function pointers */
  node = data->draw.fn_list;
  while (node) {
    last = node;
    node = node->next;
    free( last );
  }

    /* free state definitions */
  for (def=0; def < ListSize( data->defs.list, stateDefInfo ); def++) {
    free( ListItem( data->defs.list, stateDefInfo, def ).name );
    free( ListItem( data->defs.list, stateDefInfo, def ).color );
    free( ListItem( data->defs.list, stateDefInfo, def ).bitmap );
  }
  ListDestroy( data->defs.list, stateDefInfo );

  free( data );

  return 0;
}



double State_StartTime( state_data, i )
stateData *state_data;
int i;
{
  return ListItem( state_data->list, stateInfo, i ).startTime;
}

double State_EndTime( state_data, i )
stateData *state_data;
int i;
{
  return ListItem( state_data->list, stateInfo, i ).endTime;
}


static int Bsearch( state_data, list, time, state_time_fn )
stateData *state_data;
xpandList /*int*/ list;
double time;
double (*state_time_fn) ANSI_ARGS(( stateData*, int ));
{
  int top, mid, bottom;

  bottom = 0;
  top = ListSize( list, int ) - 1;
  while (bottom < top) {
    mid = (top+bottom)/2;
    if ( (*state_time_fn)( state_data, mid ) < time ) {
      bottom = mid + 1;
    } else {
      top = mid - 1;
    }
  }

  return bottom;
}


static int BsearchLE( state_data, list, time, state_time_fn )
stateData *state_data;
xpandList /*int*/ list;
double time;
double (*state_time_fn) ANSI_ARGS(( stateData*, int ));
{
  int i;

  i = Bsearch( state_data, list, time, state_time_fn );
  if ( (*state_time_fn) (state_data, i) > time ) i--;
  return i;
}

static int BsearchGE( state_data, list, time, state_time_fn )
stateData *state_data;
xpandList /*int*/ list;
double time;
double (*state_time_fn) ANSI_ARGS(( stateData*, int ));
{
  int i;

  i = Bsearch( state_data, list, time, state_time_fn );
  if ( (*state_time_fn) (state_data, i) < time ) i++;
  return i;
}



int State_GetVis( state_data, time1, time2, list1, n1, list2, n2 )
stateData *state_data;
double time1, time2;
int **list1, *n1, **list2, *n2;
{
  int first, last;

/*
fprintf( stderr, "Getvis between %f and %f\n", time1, time2 );
*/

  if (!state_data->idx_start || !state_data->idx_end ||
      1 ) {
      /* indices not created yet, don't return anything */
    *n1 = 0;
    *n2 = 0;
    return 0;
  }

  first = BsearchGE( state_data, state_data->idx_start, time1,
		     State_StartTime );
  last  = BsearchLE( state_data, state_data->idx_start, time2,
		     State_StartTime );
  *list1 = ListHeadPtr( state_data->idx_start, int ) + first;
  *n1 = last - first + 1;

/*
fprintf( stderr, "startList: %d to %d\n", first, last );
*/

  first = BsearchGE( state_data, state_data->idx_end, time1,
		     State_EndTime );
  last  = BsearchLE( state_data, state_data->idx_end, time2,
		     State_EndTime );
  *list2 = ListHeadPtr( state_data->idx_start, int ) + first;
  *n2 = last - first + 1;

/*
fprintf( stderr, "endList: %d to %d\n", first, last );
*/

  return 0;
}





static int CreateStateIndices( state_data )
stateData *state_data;
{
  int nstates, i, proc;

  nstates = ListSize( state_data->list, stateInfo );

  /* create the necessary lists */
  ListCreate( state_data->idx_start, int, nstates );
  ListCreate( state_data->idx_end, int, nstates );
  state_data->idx_proc_start =
    (xpandList /*int*/ *)malloc( sizeof(xpandList) * state_data->np );
  state_data->idx_proc_end =
    (xpandList /*int*/ *)malloc( sizeof(xpandList) * state_data->np );
  for (proc=0; proc<state_data->np; proc++) {
    ListCreate( state_data->idx_proc_start[proc],
	        int, nstates/state_data->np );
    ListCreate( state_data->idx_proc_end[proc],
	        int, nstates/state_data->np );
  }

  /* fill the indices with the related indices */
  for (i=0; i<nstates; i++) {
    ListAddItem( state_data->idx_start, int, i );
    ListAddItem( state_data->idx_end, int, i );
    proc = ListItem( state_data->list, stateInfo, i ).proc;
    ListAddItem( state_data->idx_proc_start[proc], int, i );
    ListAddItem( state_data->idx_proc_end[proc], int, i );
  }

  /* sort the indices */
  SortStart( state_data,
	     ListHeadPtr( state_data->idx_start, int ),
	     ListSize( state_data->idx_start, int ) );
  SortEnd(   state_data, 
	     ListHeadPtr( state_data->idx_end, int ),
	     ListSize( state_data->idx_end, int ) );
  for (proc=0; proc<state_data->np; proc++) {
    ListShrinkToFit( state_data->idx_proc_start[proc], int );
    SortStart( state_data,
	       ListHeadPtr( state_data->idx_proc_start[proc], int ),
	       ListSize( state_data->idx_proc_start[proc], int ) );
    ListShrinkToFit( state_data->idx_proc_end[proc], int );
    SortEnd( state_data,
	     ListHeadPtr( state_data->idx_proc_end[proc], int ),
	     ListSize( state_data->idx_proc_end[proc], int ) );
  }

  return 0;
}


#if USE_QSORT

static stateData *compareData;

static int SetCompareData( state_data )
stateData *state_data;
{
  compareData = state_data;
  return 0;
}


static int CompareStartTimes( a, b )
  /* all right, who at Sun made this brilliant decision? */
#if !(defined(sparc) || defined(hpux)) || defined(__STDC__)
const
#endif
void *a, *b;
{
  if (ListItem( compareData->list, stateInfo, *(int*)a ).startTime >
      ListItem( compareData->list, stateInfo, *(int*)b ).startTime) {
    return 1;
  } else {
    return -1;
  }
}


static int CompareEndTimes( a, b )
#if !(defined(sparc) || defined(hpux)) || defined(__STDC__)
const
#endif
void *a, *b;
{
  if (ListItem( compareData->list, stateInfo, *(int*)a ).endTime >
      ListItem( compareData->list, stateInfo, *(int*)b ).endTime) {
    return 1;
  } else {
    return -1;
  }
}
#endif
  /* USE_QSORT */



static int SortStart( state_data, a, n )
stateData *state_data;
int *a, n;     /* array and the number of elements in it */
{
#if USE_QSORT

  SetCompareData( state_data );
  qsort( (void*)a, n, sizeof(int), CompareStartTimes );
  return 0;

#else
  register int temp;
  register double time;
  int i, m, c;  /* insertPt, movePt, current */

  for (c=1; c<n-1; c++) {
    temp = a[c];
    time = ListItem( state_data->list, stateInfo, a[c]).startTime;
    for (i=c; i && time < ListItem( state_data->list,
				    stateInfo, a[i-1]).startTime;
	 i--);
    for (m=c-1; m>=i; m--)
      a[m+1] = a[m];
    a[i] = temp;
  }
  return 0;
#endif

}



static int SortEnd( state_data, a, n )
stateData *state_data;
int *a, n;     /* array and the number of elements in it */
{
#if USE_QSORT

  SetCompareData( state_data );
  qsort( (void*)a, n, sizeof(int), CompareEndTimes );
  return 0;

#else
  register int temp;
  register double time;
  int i, m, c;  /* insertPt, movePt, current */

  for (c=1; c<n-1; c++) {
    temp = a[c];
    time = ListItem( state_data->list, stateInfo, a[c]).endTime;
    for (i=c; i && time < ListItem( state_data->list,
				    stateInfo, a[i-1]).endTime;
	 i--);
    for (m=c-1; m>=i; m--)
      a[m+1] = a[m];
    a[i] = temp;
  }
  return 0;
#endif

}




static int PushState( state_data, proc, type, time )
stateData *state_data;
int proc, type;
double time;
{
  register statePost *post, *parentPost;
  int slot, nslots, slotFound;
  statePost newPost;

#if DEBUG>1
  fprintf( stderr, "Pushing state %d on proc %d at %f\n", type, proc, time );
#endif

  newPost.time = time;
  newPost.type = type;
  newPost.parent = -1;
  newPost.children = 0;		/* don't always create the list */
  newPost.hangover = -1;
  newPost.firstChild = -1;
  newPost.am_hangover = 0; 

  /* ListVerify( state_data->stacks[proc], statePost ); */

  /* look for an empty slot */
  slotFound = -1;
  parentPost = 0;
  post = ListHeadPtr( state_data->stacks[proc], statePost );
  nslots = ListSize( state_data->stacks[proc], statePost );
  for (slot=0; slot<nslots; slot++, post++ ) {
#if DEBUG>2
    fprintf( stderr, "On proc %d, slot %d is %d %f %d %d %p\n",
             proc, slot, post->type, post->time, post->parent,
	     post->hangover, (void*)post->children );
#endif
      /* if the slot is empty */
    if (post->type == -1) {
        /* and a slot hasn't been found yet, remember this slot */
      if (slotFound == -1) {
	slotFound = slot;
      }
    } else {
        /* if the slot is not empty, keep track of the latest post */
      if (!parentPost || post->time > parentPost->time) {
	  /* find latest post, it is the parent of this guy */
	parentPost = post;
	  /* save parent's slot, so I can add myself to his
	     child list when I exit */
	newPost.parent = slot;
      }
    }
  }

    /* if a parent was found set overlap level to parent's plus one */
  newPost.overlapLevel = (parentPost) ? parentPost->overlapLevel + 1 : 0;

  if (slotFound == -1) {
      /* add to the end */
    ListAddItem( state_data->stacks[proc], statePost, newPost );
      /* set slotFound to the end of the list */
    slotFound = slot;
  } else {
    ListItem( state_data->stacks[proc], statePost, slotFound ) = newPost;
  }
  
    /* tell my parent who they would be leaving behind should they
       choose to end before me */
  if (parentPost) {
    parentPost->hangover = slotFound;
  }

  /* ListVerify( state_data->stacks[proc], statePost ); */

  return 0;
}




/* 
 * FindMatchingPost 
 *
 * look for a matching start event for this state type
 */

static int FindMatchingPost( state_data, proc, type )
stateData *state_data;
int proc, type;
{
  register statePost *post, *latestPost;
  int i, n, latestIdx;

  /* ListVerify( state_data->stacks[proc], statePost ); */

#if DEBUG>1
  fprintf( stderr, "Find matching post on proc %d of state type %d\n",
	   proc, type );
#endif

  latestPost = 0;
  latestIdx = -1;

    /* find matching end of this state */
  post = ListHeadPtr( state_data->stacks[proc], statePost );
  n = ListSize( state_data->stacks[proc], statePost );
  for (i=0; i<n; i++, post++) {
#if DEBUG>2
    fprintf( stderr, "On proc %d, slot %d is %d %f %d %d %p\n",
             proc, i, post->type, post->time, post->parent,
	     post->hangover, (void*)post->children );
#endif
      /* if the type matches, and either nothing else has been found
	 yet, or the time of this post is later han any other,
	 make a note of where we are */
    if (post->type == type && (!latestPost ||
			       post->time > latestPost->time )) {
      latestIdx = i;
      latestPost = post;
    }
  }

#if DEBUG>1
  fprintf( stderr, "Matching start of state found in slot %d\n",
	   latestIdx );
#endif

  return latestIdx;
}



  /* return information about a post by packing it into a stateInfo struct */

static int GetPostInfo( state_data, proc, slot_no, info )
stateData *state_data;
int proc, slot_no;
stateInfo *info;
{
  statePost *post;

  post = ListHeadPtr( state_data->stacks[proc], statePost ) + slot_no;

  info->startTime = post->time;

    /* if my parent has already exited, they have set my am_hangover
       field to 1, and put their FRP in my parent field. */
  info->parent = (post->am_hangover) ? post->parent : -1;
  info->firstChild = post->firstChild;
  info->overlapLevel = post->overlapLevel;

  return 0;
}




  /* take care of all the kids and stuff */

static int ClosePost( state_data, proc, slot_no, finalRestingPlace )
stateData *state_data;
int proc, slot_no, finalRestingPlace;
{
  statePost *post, *parentPost;
  stateInfo *parentFRP;
  int i, *childIdxPtr;

    /* get pointer to post information */
  post = ListHeadPtr( state_data->stacks[proc], statePost ) + slot_no;

  if (post->am_hangover) {
      /* if my parent has already exited, they have set my am_hangover
	 field to 1, and put their FRP in my parent field. */
      /* check if I am their first child */
    parentFRP = ListHeadPtr( state_data->list, stateInfo ) + post->parent;
    if (parentFRP->firstChild == -1) {
      parentFRP->firstChild = finalRestingPlace;
    }
  } else {
      /* if I have a parent on the stack, add me to its 'children' list */
    if (post->parent != -1) {
        /* set parentPost to point to my parent's post */
      parentPost = ListHeadPtr( state_data->stacks[proc], statePost ) +
	post->parent;

        /* mark this guy as having a parent, but the parent isn't
	   finished yet */
      post->parent = -2;

      parentPost->hangover = -1;
      if (!parentPost->children) {
	ListCreate( parentPost->children, int, 3 );
	  /* if I have to create the children list, I am his first child */
	parentPost->firstChild = finalRestingPlace;
      }
      ListAddItem( parentPost->children, int, finalRestingPlace );
#if DEBUG>2
      fprintf( stderr, "Add me (%d) to parent's child list (parent at %d)\n",
	      finalRestingPlace, post->parent );
#endif
    }
  }

    /* go through my list of children and inform them where I ended up */
  if (post->children) {
    childIdxPtr = ListHeadPtr( post->children, int );
    for (i=0; i<ListSize( post->children, int ); i++,childIdxPtr++) {
      ListItem( state_data->list, stateInfo, *childIdxPtr ).parent
	= finalRestingPlace;
#if DEBUG>2
      fprintf( stderr, "Telling child at %d that I'll be at %d\n",
	       *childIdxPtr, finalRestingPlace );
#endif
    }
    ListDestroy( post->children, int );
  }

  /* check for a child that may be dangling */
  if (post->hangover != -1) {
      /* this kid should remember to check if I got my firstChild set */
    ListItem( state_data->stacks[proc], statePost,
	      post->hangover ).parent = finalRestingPlace;
    ListItem( state_data->stacks[proc], statePost,
	      post->hangover ).am_hangover = 1;
#if DEBUG>2
    fprintf( stderr, "Dangling child at %d told I'm at %d\n",
	     post->hangover, finalRestingPlace );
#endif
  }

  post->type = -1;		/* mark this slot as unused */

  /* ListVerify( state_data->stacks[proc], statePost ); */

  return 1;
}



#if TESTING

int PrintStates( state_data )
stateData *state_data;
{
  int doIndices, nstates, i, *idx, proc;
  stateInfo *info, *masterList;

  doIndices = state_data->idx_start != 0;

  nstates = ListSize( state_data->list, stateInfo );
  masterList = info = ListHeadPtr( state_data->list, stateInfo );

  fprintf( stderr, "Master state instance list:\n" );
  for (i=0; i<nstates; i++,info++) {
    fprintf( stderr, "State %d: from %f to %f, proc %d, type %d, parent %d, firstBorn %d\n",
	    i, info->startTime, info->endTime, info->proc,
	    info->type, info->parent, info->firstChild );
  }

#if TESTING>1
  if (doIndices) {
    fprintf( stderr, "Index by start time:\n" );
    idx = ListHeadPtr( state_data->idx_start, int );
    nstates = ListSize( state_data->idx_start, int );
    for (i=0; i<nstates; i++,idx++) {
      info = masterList + *idx;
      fprintf( stderr,
	       "State %d: from %f to %f, proc %d, type %d, parent %d\n",
	       i, info->startTime, info->endTime, info->proc,
	       info->type, info->parent );
    }

    fprintf( stderr, "Index by end time:\n" );
    idx = ListHeadPtr( state_data->idx_end, int );
    nstates = ListSize( state_data->idx_end, int );
    for (i=0; i<nstates; i++,idx++) {
      info = masterList + *idx;
      fprintf( stderr,
	       "State %d: from %f to %f, proc %d, type %d, parent %d\n",
	       i, info->startTime, info->endTime, info->proc,
	       info->type, info->parent );
    }

    for (proc=0; proc<state_data->np; proc++) {
      fprintf( stderr, "State instances on process %d\n", proc );

      fprintf( stderr, "Index by start time:\n" );
      idx = ListHeadPtr( state_data->idx_proc_start[proc], int );
      nstates = ListSize( state_data->idx_proc_start[proc], int );
      for (i=0; i<nstates; i++,idx++) {
        info = masterList + *idx;
        fprintf( stderr,
  	       "State %d: from %f to %f, proc %d, type %d, parent %d\n",
  	       i, info->startTime, info->endTime, info->proc,
  	       info->type, info->parent );
      }
  
      fprintf( stderr, "Index by end time:\n" );
      idx = ListHeadPtr( state_data->idx_proc_end[proc], int );
      nstates = ListSize( state_data->idx_proc_end[proc], int );
      for (i=0; i<nstates; i++,idx++) {
        info = masterList + *idx;
        fprintf( stderr,
		 "State %d: from %f to %f, proc %d, type %d, parent %d\n",
		 i, info->startTime, info->endTime, info->proc,
		 info->type, info->parent );
      }
    }
  }
#endif

  return 0;
}
#endif



int State_Draw( state_data, idx )
stateData *state_data;
int idx;
{
  drawStateFnList *node;
  stateInfo *info;
  int type, proc, parent, firstChild, overlapLevel;
  double startTime, endTime;

#if DEBUG>1
  fprintf( stderr, "DrawState called\n" );
#endif

  if (!(node = state_data->draw.fn_list)) return 0;

#if DEBUG>1
  fprintf( stderr, "calling some functions, starting with %p\n",
	   (void *)node->fn );
#endif

  if (idx != -1) {
    info	 = ListHeadPtr( state_data->list, stateInfo ) + idx;
    type	 = info->type;
    proc	 = info->proc;
    startTime	 = info->startTime;
    endTime	 = info->endTime;
    parent	 = info->parent;
    firstChild	 = info->firstChild;
    overlapLevel = info->overlapLevel;
  } else {
      /* index -1 signifies that the last of the state events has
	 been sent */
    info = 0;
    type = startTime = endTime = 0;
    proc = parent = firstChild = overlapLevel = -1;
  }
  do {
      /* nonzero return value means to halt immediately */
    if ((*node->fn)( node->data, idx, type, proc,
		     startTime, endTime, parent, firstChild, overlapLevel )) {
      break;
    }
    node = node->next;
  } while (node);
  return 0;
}


void *State_AddDraw( state_data, fn, data )
stateData *state_data;
drawStateFn *fn;
void *data;
{
  drawStateFnList *node;

  node = (drawStateFnList*) malloc( sizeof(drawStateFnList) );

  node->fn = fn;
  node->data = data;
  node->next = state_data->draw.fn_list;
  state_data->draw.fn_list = node;

  return (void*)node;
}


/**********************************************************************\
* Remove a function from the draw() list                               *
* Return 0 if the function pointer was successfully found and removed. *
* Return -1 if the function pointer was not found.                     *
\**********************************************************************/

int State_RmDraw( state_data, token )
stateData *state_data;
void *token;
{
  drawStateFnList *node, *prev, *toDelete;

  toDelete = token;

    /* if null pointer given, or the list is empty, return with error */
  if (!toDelete || !(node = state_data->draw.fn_list)) return -1;

  prev = 0;

  do {
    if (node == toDelete) {
      if (!prev) {
	  /* first item in the list */
	state_data->draw.fn_list = node->next;
	free( node );
	return 0;
      } else {
	prev->next = node->next;
	free( node );
	return 0;
      }
    }
    prev = node;
    node = node->next;
  } while (node);

    /* couldn't find the function */
  return -1;
}

#if 0

int StateOverlapLevel( state_data, idx, vis )
stateData *state_data;
int idx;			/* index of state instance */
Vis *vis;			/* list of visible states */
{
  stateInfo *list;
  int level, next;

  list = ListHeadPtr( state_data->list, stateInfo );
  level = 0;
  
  next = idx;
  while (list[next].parent != -1) {
    next = list[next].parent;
    level++;
  }

#if DEBUG>1
  fprintf( stderr, "Overlap level for state %d is %d\n", idx, level );
#endif

  return level;
}

#endif
