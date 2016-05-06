#include "expandingList.h"


/* Implement a heap-sort */

/*
   Create a heap.
   HeapCreate( heapList heapVar,
	       (typename) heapType,
	       int initialSize,
	       int (*compare_fn)( const void *a, const void *b ) )

   Add an item to the heap.
   HeapAdd( heapList heapVar,
	    (typename) heapType,
	    heapType item )

   Return the value of an item in the heap.
   HeapItem( heapList heapVar,
	     (typename) heapType,
	     heapType x )

   Return the size of the heap.
   HeapSize( heapList heapVar,
	     (typename) heapType )

   Return the top item on the heap.  (as modifiable lvalue)
   HeapTop( heapList heapVar,
	     (typename) heapType )

   Remove the top item from the heap, assign it to itemVar.
   HeapPop( heapList heapVar,
	    (typename) heapType,
	    heapType itemVar )

   Destroy a heap with or without data in it.
   HeapDestroy( heapList heapVar,
	        (typename) heapType )

*/

/*
   The heapsort algorithm works well when adding data to a list
in random order, and removing it in a sorted order.  Worst-case
performance is O(n log(n)).

   As items are added, they are kept in a min-heap.  A min-heap
is a binary tree in which every parent is less than either of its
children.  It will always be full, so it can be implemented with
a simple array.  Like such:


 a[] = {2, 5, 3, 13, 11, 17, 7}

   - or -

           2
          / \
        5     3
       / \   / \
      13 11 17  7

   When an item is added, it is stuck at the end of the tree and
floated up until is less than its parent.

           2
          / \
        5     3
       / \   / \
      13 11 17  7
     /
    4

      - 4 floats up -

           2
          / \
        5     3
       / \   / \
      4 11 17  7
     /
    13

           2
          / \
        4     3
       / \   / \
      5 11 17  7
     /
    13


   When pulling items off the list in sorted order, pull off the top
node first, replace it with the last node, and have the new top node
sink down until it is less than either of its children.  Make sure,
when swapping it with a child, to swap it with the lesser child.

 2<--          
          / \
        4     3
       / \   / \
      5 11 17  7
     /
    13

           13
          / \
        4     3
       / \   / \
      5 11 17  7

           3
          / \
        4     13
       / \   / \
      5 11 17  7

           3
          / \
        4     7
       / \   / \
      5 11 17  13


   Do this until the tree is empty.

*/


#ifndef HEAPSORT_H
#define HEAPSORT_H

#define heapsort_parent_(x) (((x)==0) ? -1 : (((x)-1) / 2))
#define heapsort_lchild_(x) (((x)+1)*2-1)
#define heapsort_rchild_(x) (((x)+1)*2)

#ifdef __STDC__
#define heapsort_compare_(heapVar, type, a, b) \
  ((*(int (*)(type*,type*))(heapVar)->compare)( a, b ))
#else
#define heapsort_compare_(heapVar, type, a, b) \
  ((*(heapVar)->compare)( a, b ))
#endif

struct heapList_ {
  xpandList list;
  int (*compare)();
};

typedef struct heapList_ *heapList;


#define HeapCreate( heapVar, type, initialSize, compare_fn ) do { \
  (heapVar) = (heapList) malloc( sizeof( struct heapList_ ) ); \
  if (!(heapVar)) { \
    fprintf( stderr, "Could not allocate memory for min-heap\n" ); \
  } else { \
    ListCreate( (heapVar)->list, type, (initialSize) ); \
    if ((heapVar)->list) { \
      (heapVar)->compare = (int (*)())(compare_fn); \
    } \
  } \
} while (0)


#define HeapDestroy( heapVar, type ) do { \
  ListDestroy( (heapVar), type ); \
  free( (char*)(heapVar) ); } while (0)


#define HeapAdd( heapVar, type, item ) do { \
  int _i; \
  type *_head; \
  type _x; \
  _head = ListHeadPtr( (heapVar)->list, type ); \
    /* make room in the list */ \
  ListAddItem( (heapVar)->list, type, (item) ); \
  _x = (item); \
    /* float the new item up to the top */ \
  _i = ListSize( (heapVar)->list, type ) - 1; \
  while (_i && heapsort_compare_( heapVar, type, \
				  (_head)+ heapsort_parent_(_i), \
			           &_x ) > 0) { \
    *(_head+_i) = *(_head + heapsort_parent_(_i)); \
    _i = heapsort_parent_(_i); \
  } \
  *(_head + _i) = _x; \
} while (0)

#define HeapItem( heapVar, type, i ) \
  ListItem( (heapVar)->list, type, (i) )

#define HeapSize( heapVar, type ) \
  ListSize( (heapVar)->list, type )

#define HeapTop( heapVar, type ) \
  ListItem( (heapVar)->list, type, 0 )

#define HeapPop( heapVar, type, itemVar ) do { \
  int _i, _size; \
  type *_head; \
  type _x; \
 \
    /* get the top item */ \
  (itemVar) = ListItem( (heapVar)->list, type, 0 ); \
    /* grab the last item */ \
  _x = ListItem( (heapVar)->list, type, \
	        ListSize( (heapVar)->list, type ) - 1); \
  ListRemoveItems( (heapVar)->list, type, 1 ); \
 \
  _i = 0; \
  _size = HeapSize( heapVar, type ); \
  _head = ListHeadPtr( (heapVar)->list, type ); \
 \
    /* if I haven't sunk past the end of the heap, \
       and one of our chilren outweighs me */ \
  while (heapsort_lchild_(_i) < _size) { \
    if (heapsort_rchild_(_i) >= _size) { \
 \
        /* only have left child, no right child */ \
      if (heapsort_compare_( heapVar, type, \
          &_x, _head + heapsort_lchild_(_i) ) > 0) { \
	  /* if greater than this one child, move the child up \
	     and set i so that x gets put in this child's place */ \
	*(_head+_i) = *(_head+heapsort_lchild_(_i)); \
	_i = heapsort_lchild_(_i); \
      } else { \
	  /* if less than this one child, we're home!  Leave i where it is \
	     and exit so that x gets written */ \
	break; \
      } \
    } else { \
        /* have both children */ \
      if ( heapsort_compare_(heapVar, type, \
			      _head+heapsort_lchild_(_i), &_x ) > 0 && \
	   heapsort_compare_(heapVar, type, \
			      _head+heapsort_rchild_(_i), &_x ) > 0) { \
	  /* if both children are greater than x, we're home */ \
	break; \
      } else { \
	  /* swap with the smaller of the children */ \
	if (heapsort_compare_(heapVar, type, \
    _head+heapsort_lchild_(_i), _head+heapsort_rchild_(_i) ) < 0) { \
	  *(_head+_i) = *(_head+heapsort_lchild_(_i)); \
	  _i = heapsort_lchild_(_i); \
	} else { \
	  *(_head+_i) = *(_head+heapsort_rchild_(_i)); \
	  _i = heapsort_rchild_(_i); \
	} \
      } \
    } \
  } \
 \
    /* put x in its rightful place */ \
  *(_head + _i) = _x; \
} while(0)
  

#endif
