/*
 * Collective operations (this allows choosing either an implementation
 * in terms of point-to-point, or a special version exploiting special
 * facilities, in a communicator by communicator fashion).
 */
#ifndef MPIR_COLLOPS_DEF
#define MPIR_COLLOPS_DEF
struct _MPIR_COLLOPS {
    int (*Barrier) ANSI_ARGS((struct MPIR_COMMUNICATOR *));
    int (*Bcast) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, int, 
			    struct MPIR_COMMUNICATOR * ));
    int (*Gather) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
		  void*, int, struct MPIR_DATATYPE *, int, 
			     struct MPIR_COMMUNICATOR *)); 
    int (*Gatherv) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
			      void*, int *, int *, struct MPIR_DATATYPE *, 
			      int, struct MPIR_COMMUNICATOR *)); 
    int (*Scatter) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
		   void*, int, struct MPIR_DATATYPE *, int, 
			      struct MPIR_COMMUNICATOR *));
    int (*Scatterv) ANSI_ARGS((void*, int *, int *, struct MPIR_DATATYPE *, 
			       void*, int, 
		    struct MPIR_DATATYPE *, int, struct MPIR_COMMUNICATOR *));
    int (*Allgather) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
		     void*, int, struct MPIR_DATATYPE *, 
				struct MPIR_COMMUNICATOR *));
    int (*Allgatherv) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
		      void*, int *, int *, struct MPIR_DATATYPE *,
				 struct MPIR_COMMUNICATOR *));
    int (*Alltoall) ANSI_ARGS((void*, int, struct MPIR_DATATYPE *, 
		    void*, int, struct MPIR_DATATYPE *, 
			       struct MPIR_COMMUNICATOR *));
    int (*Alltoallv) ANSI_ARGS((void*, int *, int *, 
		     struct MPIR_DATATYPE *, void*, int *, 
		     int *, struct MPIR_DATATYPE *, 
				struct MPIR_COMMUNICATOR *));
    int (*Reduce) ANSI_ARGS((void*, void*, int, 
		  struct MPIR_DATATYPE *, MPI_Op, int, 
			     struct MPIR_COMMUNICATOR *));
    int (*Allreduce) ANSI_ARGS((void*, void*, int, 
		     struct MPIR_DATATYPE *, MPI_Op, 
				struct MPIR_COMMUNICATOR *));
    int (*Reduce_scatter) ANSI_ARGS((void*, void*, int *, 
			  struct MPIR_DATATYPE *, MPI_Op, 
				     struct MPIR_COMMUNICATOR *));
    int (*Scan) ANSI_ARGS((void*, void*, int, struct MPIR_DATATYPE *, MPI_Op, 
			   struct MPIR_COMMUNICATOR * ));
    int ref_count;     /* So we can share it */
};

/* Predefined function tables for collective routines, the device
 * can also use its own, but these are the defaults.
 */
extern MPIR_COLLOPS MPIR_inter_collops;   /* Simply raises appropriate error */
extern MPIR_COLLOPS MPIR_intra_collops;   /* Do the business using pt2pt     */


#endif
