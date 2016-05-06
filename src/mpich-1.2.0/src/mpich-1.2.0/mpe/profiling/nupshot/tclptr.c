/*

   Module for allowing Tcl code to hold C pointers.
   
   An array of generic pointers is kept (void*), and when
   a C routine returns a pointer to a Tcl
   routine, the C routine allocates an element in this array
   and passes the index to the Tcl routine.  When the Tcl
   calls anther C routine, it passes this integer handle, and
   the C routine look up the correct pointer in the table.
   The same table is shared by all windows, since any window can
   request data at any time.

   0 will always be an invalid value.

*/

#include "expandingList.h"


static xpandList ptr_list;  /* list of generic pointers */

int AllocTclPtr( ptr )
void *ptr;
{
  static int firstCall = 1;
  int i;

  if (firstCall) {
    ListCreate( ptr_list, void *, 10 );
    ListAddItem( ptr_list, void *, (void*)0 );
    firstCall = 0;
  }

    /* reserve 0 as an invalid value */
  i=1;
  while (i < ListSize( ptr_list, void * ) &&
	 ListItem(ptr_list, void *, i)) i++;
  /* find either the end of the pointer list or the first unused slot */

  if (i == ListSize( ptr_list, void * )) {
    /* if at the end of the list */
    ListAddItem( ptr_list, void *, ptr );
    return i;
  }
  ListItem( ptr_list, void *, i ) = ptr;
  return i;
}


void *GetTclPtr( i )
int i;
{
  if (i >= ListSize( ptr_list, void * )) return 0;
  return ListItem( ptr_list, void *, i );
}

int FreeTclPtr( i )
int i;
{
  ListItem( ptr_list, void *, i ) = 0;
  return 0;
}

