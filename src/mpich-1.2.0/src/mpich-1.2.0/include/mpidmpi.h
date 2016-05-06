#ifndef _MPIR_DMPI_INC
#define _MPIR_DMPI_INC

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

void MPID_BSwap_N_inplace ANSI_ARGS(( unsigned char *, int, int ));
void MPID_BSwap_short_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_int_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_long_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_float_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_double_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_long_double_inplace ANSI_ARGS(( unsigned char *, int ));
void MPID_BSwap_N_copy ANSI_ARGS(( unsigned char *, unsigned char *, 
				   int, int ));
void MPID_BSwap_short_copy ANSI_ARGS(( unsigned char *, unsigned char *, 
				       int ));
void MPID_BSwap_int_copy ANSI_ARGS(( unsigned char *, unsigned char *, int ));
void MPID_BSwap_long_copy ANSI_ARGS(( unsigned char *, unsigned char *, int ));
void MPID_BSwap_float_copy ANSI_ARGS(( unsigned char *, unsigned char *, 
				       int ));
void MPID_BSwap_double_copy ANSI_ARGS(( unsigned char *, unsigned char *, 
					int ));
void MPID_BSwap_long_double_copy ANSI_ARGS(( unsigned char *, 
					     unsigned char *, int ));

int MPID_Type_swap_copy ANSI_ARGS(( unsigned char *, unsigned char *, 
				    struct MPIR_DATATYPE *, int, void * ));
void MPID_Type_swap_inplace ANSI_ARGS(( unsigned char *, 
					struct MPIR_DATATYPE *, int ));
int MPID_Type_XDR_encode ANSI_ARGS(( unsigned char *, unsigned char *, 
				     struct MPIR_DATATYPE *, int, void * ));
int MPID_Type_XDR_decode ANSI_ARGS(( unsigned char *, int, 
				     struct MPIR_DATATYPE *, int, 
				     unsigned char *, int, int *, int *, 
				     void * ));
int MPID_Type_convert_copy ANSI_ARGS(( struct MPIR_COMMUNICATOR *, void *, 
				       int, void *, 
				       struct MPIR_DATATYPE *, 
				       int, int, int * ));
int MPID_Mem_convert_len ANSI_ARGS(( MPID_Msgrep_t, struct MPIR_DATATYPE *, 
				     int ));
int MPID_Mem_XDR_Len ANSI_ARGS(( struct MPIR_DATATYPE *, int ));

int MPIR_Comm_needs_conversion ANSI_ARGS(( struct MPIR_COMMUNICATOR * ));
int MPIR_Dest_needs_converstion ANSI_ARGS(( int ));
void MPIR_Pack_Hvector ANSI_ARGS(( struct MPIR_COMMUNICATOR *, char *, int, 
				   struct MPIR_DATATYPE*, int, char * ));
void MPIR_UnPack_Hvector ANSI_ARGS(( char *, int, struct MPIR_DATATYPE*, 
				     int, char * ));
int MPIR_HvectorLen ANSI_ARGS(( int, struct MPIR_DATATYPE * ));
int MPIR_PackMessage ANSI_ARGS(( char *, int, struct MPIR_DATATYPE *, int, 
				 int, MPI_Request ));
int MPIR_EndPackMessage ANSI_ARGS(( MPI_Request ));
int MPIR_SetupUnPackMessage ANSI_ARGS(( char *, int, struct MPIR_DATATYPE *, 
					int, MPI_Request ));
int MPIR_Receive_setup ANSI_ARGS(( MPI_Request * ));
int MPIR_Send_setup ANSI_ARGS(( MPI_Request * ));
int MPIR_SendBufferFree ANSI_ARGS(( MPI_Request ));

int MPIR_Elementcnt ANSI_ARGS(( unsigned char *, int, struct MPIR_DATATYPE*, 
			    int, unsigned char *, int, int *, int *, void * ));
void DMPI_msg_arrived ANSI_ARGS(( int, int, MPIR_CONTEXT, MPIR_RHANDLE **, 
				  int * ));
void DMPI_free_unexpected ANSI_ARGS(( MPIR_RHANDLE * ));

/*
int MPIR_Pack  ANSI_ARGS(( struct MPIR_COMMUNICATOR *, int, void *, int, 
                           struct MPIR_DATATYPE *,
			   void *, int, int * ));
int MPIR_Pack_size ANSI_ARGS(( int, struct MPIR_DATATYPE *, 
                               struct MPIR_COMMUNICATOR *, int, int * ));
*/
int MPIR_Pack2 ANSI_ARGS(( char *, int, int, struct MPIR_DATATYPE *, 
		int  (*) (unsigned char *, unsigned char *, 
				     struct MPIR_DATATYPE*, int, 
				     void *), void *, char *, int *, int * ));
int MPIR_Unpack2 ANSI_ARGS(( char *, int, struct MPIR_DATATYPE*, 
		   int  (*)(unsigned char *, int, struct MPIR_DATATYPE*, int,
			   unsigned char *, int, int *, int *, void *),
		   void *, char *, int, int *, int * ));
int MPIR_Unpack ANSI_ARGS(( struct MPIR_COMMUNICATOR *, void *, int, int, 
			    struct MPIR_DATATYPE *,
			    MPID_Msgrep_t, 
			    void *, int *, int * ));

int MPIR_Printcontig ANSI_ARGS(( unsigned char *, unsigned char *, 
				 struct MPIR_DATATYPE*, int, void *));
int MPIR_Printcontig2 ANSI_ARGS(( char *, int, struct MPIR_DATATYPE*, 
				  int, char *, void * ));
int MPIR_Printcontig2a ANSI_ARGS(( unsigned char *, int, 
				   struct MPIR_DATATYPE*, int, 
			unsigned char *, int, int *, int *, void * ));

#ifdef MPID_INCLUDE_STDIO
int MPIR_PrintDatatypePack ANSI_ARGS(( FILE *, int, struct MPIR_DATATYPE*, 
				       long, long ));
int MPIR_PrintDatatypeUnpack ANSI_ARGS(( FILE *, int, MPI_Datatype, 
					 long, long ));
#endif

#endif
