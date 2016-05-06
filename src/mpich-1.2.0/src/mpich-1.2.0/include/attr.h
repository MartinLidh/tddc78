#ifndef MPIR_HBT_COOKIE

#include "comm.h"

typedef union {
    int (*c_copy_fn) ANSI_ARGS(( MPI_Comm, int, void *, void *, void *, 
				 int * ));
    void (*f77_copy_fn) ANSI_ARGS(( MPI_Comm, int *, int *, int *, int *, 
				    int *, int * ));
} MPIR_Copy_fn;
typedef union {
    int (*c_delete_fn) ANSI_ARGS(( MPI_Comm, int, void *, void * ));
    void (*f77_delete_fn) ANSI_ARGS(( MPI_Comm, int *, int *, void *, int * ));
} MPIR_Delete_fn;

typedef struct  {
    MPIR_COOKIE                    /* Cookie to help detect valid items */
    MPIR_Copy_fn copy_fn;
    MPIR_Delete_fn delete_fn;
    void  *extra_state;
    int    FortranCalling;        /* Used to indicate whether Fortran or
				     C calling conventions are used for
				     copy_fn (attribute_in is passed by 
				     value in C, must be passed by reference
				     in Fortran); the underlying code
				     must also understand what a 
				     Fortran logical looks like */
    int    ref_count;
    int    permanent;             /* Used to mark the permanent attributes of
				     MPI_COMM_WORLD */
    int    self;                  /* External value for keyval */
} MPIR_Attr_key;

#define MPIR_ATTR_COOKIE 0xa774c003

typedef struct _MPIR_HBT_node {
  MPIR_COOKIE                    /* Cookie to help detect valid items */
  void                  *value;
  MPIR_Attr_key         *keyval;
  short                 balance;
  struct _MPIR_HBT_node *left;
  struct _MPIR_HBT_node *right;
} MPIR_HBT_node;
#define MPIR_HBT_NODE_COOKIE 0x03b740de

struct _MPIR_HBT {
  MPIR_COOKIE                    /* Cookie to help detect valid items */
  unsigned int   height;
  int            ref_count;
  MPIR_HBT_node *root;
};
#define MPIR_HBT_COOKIE 0x03b7c007

int MPIR_Attr_copy_node ANSI_ARGS(( struct MPIR_COMMUNICATOR *, 
				    struct MPIR_COMMUNICATOR *, 
				    MPIR_HBT_node * ));
int MPIR_Attr_copy_subtree ANSI_ARGS(( struct MPIR_COMMUNICATOR *, 
				       struct MPIR_COMMUNICATOR *, MPIR_HBT, 
				       MPIR_HBT_node * ));
int MPIR_Attr_free_node ANSI_ARGS(( struct MPIR_COMMUNICATOR *, 
				    MPIR_HBT_node * ));
int MPIR_Attr_free_subtree ANSI_ARGS(( struct MPIR_COMMUNICATOR *, 
				       MPIR_HBT_node * ));

int MPIR_HBT_new_tree ANSI_ARGS(( MPIR_HBT * ));
int MPIR_HBT_new_node ANSI_ARGS(( MPIR_Attr_key *, void *, MPIR_HBT_node ** ));
int MPIR_HBT_free_node ANSI_ARGS(( MPIR_HBT_node * ));
int MPIR_HBT_free_subtree ANSI_ARGS(( MPIR_HBT_node * ));
int MPIR_HBT_free_tree ANSI_ARGS(( MPIR_HBT ));
int MPIR_HBT_lookup ANSI_ARGS(( MPIR_HBT, int, MPIR_HBT_node ** ));
int MPIR_HBT_insert ANSI_ARGS(( MPIR_HBT, MPIR_HBT_node * ));
int MPIR_HBT_delete ANSI_ARGS(( MPIR_HBT, int, MPIR_HBT_node ** ));
void MPIR_HBT_Init ANSI_ARGS((void));
void MPIR_HBT_Free ANSI_ARGS((void));

#define MPIR_GET_KEYVAL_PTR(idx) \
    (MPIR_Attr_key *)MPIR_ToPointer( idx )
#define MPIR_TEST_KEYVAL_NOTOK(idx,ptr) \
   (!(ptr) || ((ptr)->cookie != MPIR_ATTR_COOKIE))
#define MPIR_TEST_MPI_KEYVAL(idx,ptr,comm,routine_name) \
if (!(ptr)) {\
 mpi_errno = MPIR_Err_setmsg( MPI_ERR_ARG, MPIR_ERR_KEYVAL_NULL,myname,(char*)0,(char *)0);}\
else if ((ptr)->cookie != MPIR_ATTR_COOKIE) {\
mpi_errno = MPIR_Err_setmsg( MPI_ERR_INTERN, MPIR_ERR_ATTR_CORRUPT, myname,\
(char*)0,(char *)0,(ptr)->cookie);}
#endif
