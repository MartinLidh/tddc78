/*

   Module for allowing Tcl code to hold C pointers.
   
   Ed Karrels
   Argonne National Laboratory

*/


#ifdef __STDC__

int AllocTclPtr( void *ptr );
  /* allocate a slot in the tcl pointer table, store the pointer,
     return the index */

void *GetTclPtr( int i );
  /* given an index into the table, get the pointer */

int FreeTclPtr( int i );
  /* release an entry in the table */

#else
int AllocTclPtr();
void *GetTclPtr();
int FreeTclPtr();
#endif


