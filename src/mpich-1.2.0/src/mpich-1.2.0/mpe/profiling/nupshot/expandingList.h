#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif

#include <stdio.h>

/*
 * The basic idea is to create an array that you can
 * add to to your heart's content, and whenever the allocated data
 * area gets full, it realloc's twice the size.  The increase by a
 * factor rather than a sum is important for scaleability.  If you
 * pick an arbitrary size, say, 100 extra elements, you're gonna
 * do 10000 reallocations if you read a million elements.  With
 * a factor of two, it'll only do about 20 reallocations.
 * Once you are done adding to the list, it can be 'shrink-wrapped'
 * to only allocate the amount of memory actually used.
 * 
 * typical use:
 * 
 *   mystruct item, *normalArray;
 *   xpandList / *mystruct* / list;
 *   int nelements;
 * 
 *   ListCreate( list, mystruct, 10 );
 *     / * create space for 10 elements from the start * /
 *   while (!OutOfData()) {
 *     ReadData( &item );
 *     ListAddItem( list, mystruct, item );
 *   }
 * 
 *   ListClose( list, mystruct, normalArray, nelements );
 * 
 * Note that ListItem() gives you a modifiable lvalue, so you can do:
 * 
 *   ListItem( list, mystruct, 23 ).fieldx = 2.5;
 * 
 * which should be equivalent to
 * 
 *   ((mystruct *)list)[20].fieldx = 2.5;
 */




#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
int fprintf( FILE *, const char *, ... );
#endif


#ifndef EXPANDINGLIST_H
#define EXPANDINGLIST_H

/* Array implementation of exapanding lists */
/* written by Ed Karrels */

/* version 2 - all macros */

/* I've started using this technique in various places, and I don't feel
   like reinventing the wheel anymore with various data types, so let's
   see if we can generalize it and do it typeless */

/*

   expandingList.h will create the following structure:
   struct xpandList_ {
     void *list;  pointer to the list data
     int size;    size of the list
     int nused;   number of elements in used, starting at 0
   };
   typedef xpandList_ *xpandList;

   The following macros are created:

   ListCreate( listVar, type, initialSize )
     creates a list in listVar (an xpandList variable)
   ListAddItem( listVar, type, newItem )
     add newItem to the list (newItem will have only 1 instance)
   ListSize( listVar, type )
     returns the size of listVar
   ListItem( listVar, type, index )
     returns item # index in listVar as a _modifiable_lvalue_
   ListHeadPtr( listVar, type )
     returns the pointer to the head of the list (&(list[0]))
   ListClose( listVar, type, head, nitems )
     deallocate the list and return the head and nitems
   ListDestroy( listVar, type )
     frees the memory in use by listVar and sets listVar to 0
   ListClear( listVar, type )
     clear out a list, leaving it with the same allocated space 
   ListRemoveItems( listVar, type, nitems )
     remove items from the end of a list
   ListVerify( listVar, type )
     make sure the list isn't using more elements than it owns

*/


#ifndef DEFAULT_LEN
#define DEFAULT_LEN 10
#endif

#ifndef GROWTH_FACTOR
#define GROWTH_FACTOR 2
#endif



struct xpandList_ {
  void *list;		     /* pointer to the list data */
  int size;		     /*  size of the list */
  int nused;	             /*  number of elements in used, starting at 0 */
};
typedef struct xpandList_ *xpandList;

#define xpandList_type(itemtype) struct { \
  itemtype list; \
  int size, nused; }


#define ListCreate( listVar, type, initialSize ) { \
  int initialLen; \
  initialLen = initialSize; \
  if (initialLen<1) initialLen = DEFAULT_LEN; \
  (listVar) = (xpandList) malloc( sizeof( struct xpandList_ ) ); \
  if ((listVar)) { \
    (listVar)->list = (void *) malloc( sizeof( type ) * initialLen ); \
    if (!(listVar)->list) { \
      fprintf( stderr, "Could not allocate memory for expanding list\n" ); \
    } \
    (listVar)->nused = 0; \
    (listVar)->size = initialLen; \
  } else { \
    fprintf( stderr, "Could not allocate memory for expanding list\n" ); \
  } \
}



#define ListAddItem( listVar, type, newItem ) { \
  void *newPtr; \
  int newSize; \
  newPtr = (listVar)->list; \
  if ((listVar)->nused == (listVar)->size) { \
    newSize = (listVar)->size; \
    if (newSize < 1) newSize = 1; \
    newSize *= GROWTH_FACTOR; \
    newPtr = (void *) malloc( sizeof( type ) * newSize ); \
    if (!newPtr) { \
      fprintf( stderr, "Could not allocate memory for expanding list\n" ); \
    } else { \
      memcpy( newPtr, (listVar)->list, sizeof( type ) * (listVar)->size ); \
      (listVar)->list = newPtr; \
      (listVar)->size = newSize; \
    } \
  } \
  if (newPtr) ((type *)((listVar)->list)) [((listVar)->nused)++] = newItem; \
}

#define ListClear( listVar, type ) {(listVar)->nused = 0;}

#define ListRemoveItems( listVar, type, nitems ) { \
  (listVar)->nused -= (nitems); \
}

#define ListVerify( listVar, type ) { \
  if ((listVar)->nused > (listVar)->size) { \
    fprintf( stderr, "Error! (file %s, line %d)\n", __FILE__, __LINE__ );}}

#define ListSize( listVar, type ) ( (listVar)->nused )

#define ListItem( listVar, type, idx ) ( ((type *)((listVar)->list)) [(idx)] )

#define ListHeadPtr( listVar, type ) ( (type *)((listVar)->list) )


#define ListShrinkToFit( listVar, type ) { \
  (listVar)->size = (listVar)->nused; \
  if (!(listVar)->size) (listVar)->size = 1; \
  (listVar)->list = (type*) realloc( (listVar)->list, \
                    sizeof( type ) * (listVar)->size ); }

#define ListClose( listVar, type, headPtr, nitems ) { \
  ListShrinkToFit( (listVar), type ); \
  headPtr = ListHeadPtr( (listVar), type ); \
  nitems = ListSize( (listVar), type ); \
  free( (listVar) ); \
}

#define ListDestroy( listVar, type ) \
  {free( (listVar)->list ); free( (listVar) ); (listVar) = 0;}

#endif
