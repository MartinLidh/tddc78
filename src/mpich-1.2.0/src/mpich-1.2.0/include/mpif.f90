Module MPI
!
!  
!  (C) 1993 by Argonne National Laboratory and Mississipi State University.
!      All rights reserved.  See COPYRIGHT in top-level directory.
!
!
! user include file for MPI programs, with no dependencies.
! THIS IS AN UNTESTED F90 MODULE FILE.  PLEASE SEND US ANY FIXES 
!
! It really isn't possible to make a perfect include file that can
! be used by both F77 and F90 compilers, but this is close.  We've removed
! continuation lines (allows free form input in F90); systems whose
! Fortran compilers support ! instead of just C or * for comments can
! globally replace a C in the first column with !; the resulting file
! should work for both Fortran 77 and Fortran 90.
!
!
! return codes 
 Integer, parameter :: MPI_SUCCESS = 0, MPI_ERR_BUFFER=1,MPI_ERR_COUNT=2, &
      MPI_ERR_TYPE=3, MPI_ERR_TAG=4, MPI_ERR_COMM=5, MPI_ERR_RANK=6, &
      MPI_ERR_ROOT=7,MPI_ERR_GROUP=8, MPI_ERR_OP=9,MPI_ERR_TOPOLOGY=10, &
      MPI_ERR_DIMS=11, MPI_ERR_ARG=12,MPI_ERR_UNKNOWN=13, &
      MPI_ERR_TRUNCATE=14,MPI_ERR_OTHER=15, MPI_ERR_INTERN=16, &
      MPI_ERR_IN_STATUS=17, MPI_ERR_PENDING=18, MPI_ERR_REQUEST=19, &
      MPI_ERR_LASTCODE=4114 
!
      Integer, parameter :: MPI_UNDEFINED = (-32766)
!
      Integer, parameter :: MPI_GRAPH = 1, MPI_CART = 2
      Integer, parameter :: MPI_PROC_NULL = (-1)
!
      Integer, parameter :: MPI_BSEND_OVERHEAD = 512

      Integer, parameter :: MPI_SOURCE=2, MPI_TAG=3, MPI_ERROR=4
      Integer, parameter :: MPI_STATUS_SIZE=4
      Integer, parameter :: MPI_MAX_PROCESSOR_NAME=256
      Integer, parameter :: MPI_MAX_ERROR_STRING=512
      Integer, parameter :: MPI_MAX_NAME_STRING=63
!
      Integer, parameter :: MPI_COMM_NULL=0
!
      Integer, parameter :: MPI_DATATYPE_NULL = 0
      
      Integer, parameter :: MPI_ERRHANDLER_NULL = 0
      
      Integer, parameter :: MPI_GROUP_NULL = 0
      
      Integer, parameter :: MPI_KEYVAL_INVALID = 0
      
      Integer, parameter :: MPI_REQUEST_NULL = 0
! 
      Integer, parameter :: MPI_IDENT=0, MPI_CONGRUENT=1, MPI_SIMILAR=2, &
           MPI_UNEQUAL=3
!
!     MPI_BOTTOM needs to be a known address; here we put it at the
!     beginning of the common block.  The point-to-point and collective
!     routines know about MPI_BOTTOM, but MPI_TYPE_STRUCT as yet does not.
!
!     The types MPI_INTEGER1,2,4 and MPI_REAL4,8 are OPTIONAL.
!     Their values are zero if they are not available.  Note that
!     using these reduces the portability of code (though may enhance
!     portability between Crays and other systems)
!
      Integer :: MPI_BOTTOM
!
      Integer, parameter :: MPI_ERRORS_ARE_FATAL=119, MPI_ERRORS_RETURN=120
!
      Integer, parameter :: MPI_COMPLEX=23,MPI_DOUBLE_COMPLEX=24,&
           MPI_LOGICAL=25, MPI_REAL=26,MPI_DOUBLE_PRECISION=27,MPI_INTEGER=28,&
           MPI_2INTEGER=29,MPI_2COMPLEX=30,MPI_2DOUBLE_COMPLEX=31,&
           MPI_2REAL=32,MPI_2DOUBLE_PRECISION=33,MPI_CHARACTER=1,&
           MPI_BYTE=3,MPI_UB=16,MPI_LB=15,MPI_PACKED=14

      Integer, parameter :: MPI_INTEGER1=0,MPI_INTEGER2=0,MPI_INTEGER4=0
      Integer, parameter :: MPI_REAL4=0,MPI_REAL8=0


      Integer, parameter :: MPI_MAX=100,MPI_MIN=101,MPI_SUM=102,MPI_PROD=103,&
           MPI_LAND=104,MPI_BAND=105,MPI_LOR=106,MPI_BOR=107, &
           MPI_LXOR=108,MPI_BXOR=109,MPI_MINLOC=110, &
           MPI_MAXLOC=111, MPI_OP_NULL=0
!
      Integer, parameter :: MPI_GROUP_EMPTY=90,MPI_COMM_WORLD=91,&
           MPI_COMM_SELF=92, MPI_TAG_UB=80,MPI_HOST=82,MPI_IO=84, &
           MPI_WTIME_IS_GLOBAL=86
!
      Integer, parameter :: MPI_ANY_SOURCE = (-2), MPI_ANY_TAG = (-1)
!
      Integer, parameter :: MPI_VERSION    = 1, MPI_SUBVERSION = 1
!
!     All other MPI routines are subroutines
!     This may cause some Fortran compilers to complain about defined and
!     not used.  Such compilers should be improved.
!
      double precision :: MPI_WTIME, MPI_WTICK
      external MPI_WTIME, MPI_WTICK
!
!     The attribute copy/delete subroutines are symbols that can be passed
!     to MPI routines
!
      external MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, MPI_DUP_FN

End Module MPI
