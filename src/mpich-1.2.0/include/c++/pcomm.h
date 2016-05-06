// -*- c++ -*-
//
// Copyright 1997-1999, University of Notre Dame.
// Authors:  Jeremy G. Siek, Michael P. McNally, Jeffery M. Squyres, 
//           Andrew Lumsdaine
//
// This file is part of the Notre Dame C++ bindings for MPI
//
// You should have received a copy of the License Agreement for the
// Notre Dame C++ bindings for MPI along with the software;  see the
// file LICENSE.  If not, contact Office of Research, University of Notre
// Dame, Notre Dame, IN  46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//

class Comm_Null {
public:

  // construction
  Comm_Null() : mpi_comm(MPI_COMM_NULL) { }
  // copy
  Comm_Null(const Comm_Null& data) : mpi_comm(data.mpi_comm) { }
  // inter-language operability  
  Comm_Null(const MPI_Comm& data) : mpi_comm(data) { }

  // destruction
  //JGS  virtual ~Comm_Null() { }

 // comparison
  // JGS make sure this is right (in other classes too)
  MPI2CPP_BOOL_T operator==(const Comm_Null& data) const {
    return (MPI2CPP_BOOL_T) (mpi_comm == data.mpi_comm); }

  MPI2CPP_BOOL_T operator!=(const Comm_Null& data) const {
    return (MPI2CPP_BOOL_T) !(*this == data);}

  // inter-language operability (conversion operators)
  operator MPI_Comm() const { return mpi_comm; }
  operator MPI_Comm*() /*const JGS*/ { return &mpi_comm; }

protected:
  MPI_Comm mpi_comm;
};

class Comm : public Comm_Null {

#if 0 // JGS getting rid of !@(&!$@ friends, justing making some stuff public
#if _MPIPP_USENAMESPACE_
#if IBM_SP
  friend void ::errhandler_intercept(MPI_Comm * mpi_comm, int * err, char*, int*, int*);

#else
  friend void ::errhandler_intercept(MPI_Comm * mpi_comm, int* err, ...);
#endif
  friend int ::copy_attr_intercept(MPI_Comm oldcomm, int keyval, 
				   void *extra_state, void *attribute_val_in, 
				   void *attribute_val_out, int *flag);
  friend int ::delete_attr_intercept(MPI_Comm comm, int keyval, 
				     void *attribute_val, void *extra_state);
#endif

#if ! _MPIPP_USENAMESPACE_

#if IBM_SP
  friend void errhandler_intercept(MPI_Comm * mpi_comm, int * err, char*, int*, int*);
#else
  friend void errhandler_intercept(MPI_Comm * mpi_comm, int* err, ...);
#endif
  friend int copy_attr_intercept(MPI_Comm oldcomm, int keyval, 
				 void *extra_state, void *attribute_val_in, 
				 void *attribute_val_out, int *flag);
  friend int delete_attr_intercept(MPI_Comm comm, int keyval, 
				   void *attribute_val, void *extra_state);
#endif
#endif

public:

  // $%^#$%^#$^ g++ 2.7.2!!! We have to rename these functions because
  // g++ does not scope properly.  It somehow loses the outter class (MPI
  // or PMPI) -- remember, g++ does not do namespaces! -- and thinks that 
  // we are conflicting with Errhandler_fn et al. in comm.h
  // @#$%@#$@#%$@#% g++!!
  typedef void PMPI_Errhandler_fn(Comm&, int*, ...);
  typedef int PMPI_Copy_attr_function(const Comm& oldcomm, int comm_keyval,
				      void* extra_state, void* attribute_val_in,
				      void* attribute_val_out, MPI2CPP_BOOL_T& flag);
  typedef int PMPI_Delete_attr_function(Comm& comm, int comm_keyval, 
					void* attribute_val, void* extra_state);
#define ERRHANDLERFN PMPI_Errhandler_fn
#define COPYATTRFN PMPI_Copy_attr_function
#define DELETEATTRFN PMPI_Delete_attr_function

  // construction
  Comm() { }
  // copy
  Comm(const Comm_Null& data) : Comm_Null(data) { }
  // inter-language operability  
  Comm(const MPI_Comm& data) : Comm_Null(data) { }

  //
  // Point-to-Point
  //

  virtual void Send(const void *buf, int count, 
		    const Datatype & datatype, int dest, int tag) const;

  virtual void Recv(void *buf, int count, const Datatype & datatype,
		    int source, int tag, Status & status) const;

  virtual void Recv(void *buf, int count, const Datatype & datatype,
		    int source, int tag) const;
  
  virtual void Bsend(const void *buf, int count,
		     const Datatype & datatype, int dest, int tag) const;
  
  virtual void Ssend(const void *buf, int count, 
		     const Datatype & datatype, int dest, int tag) const;
  
  virtual void Rsend(const void *buf, int count,
		     const Datatype & datatype, int dest, int tag) const;
  
  virtual Request Isend(const void *buf, int count,
			     const Datatype & datatype,
			     int dest, int tag) const;

  virtual Request Ibsend(const void *buf, int count, const
			      Datatype & datatype, int dest, int tag) const;
 
  virtual Request Issend(const void *buf, int count,
			      const Datatype & datatype, int dest, int tag) const;
  
  virtual Request Irsend(const void *buf, int count,
			      const Datatype & datatype, int dest, int tag) const;
  
  virtual Request Irecv(void *buf, int count,
			     const Datatype & datatype, int source, int tag) const;

  virtual MPI2CPP_BOOL_T Iprobe(int source, int tag, Status & status) const;
  
  virtual MPI2CPP_BOOL_T Iprobe(int source, int tag) const;

  virtual void Probe(int source, int tag, Status & status) const;

  virtual void Probe(int source, int tag) const;

  virtual Prequest Send_init(const void *buf, int count,
				  const  Datatype & datatype,
				  int dest, int tag) const;

  virtual Prequest Bsend_init(const void *buf, int count,
				   const Datatype & datatype,
				   int dest, int tag) const;
  
  virtual Prequest Ssend_init(const void *buf, int count,
				   const Datatype & datatype,
				   int dest, int tag) const;
  
  virtual Prequest Rsend_init(const void *buf, int count,
				   const Datatype & datatype,
				   int dest, int tag) const;

  virtual Prequest Recv_init(void *buf, int count,
				  const Datatype & datatype,
				  int source, int tag) const;
  
  virtual void Sendrecv(const void *sendbuf, int sendcount,
			const Datatype & sendtype, int dest, int sendtag, 
			void *recvbuf, int recvcount, 
			const Datatype & recvtype, int source,
			int recvtag, Status & status) const;
 
  virtual void Sendrecv(const void *sendbuf, int sendcount,
			const Datatype & sendtype, int dest, int sendtag, 
			void *recvbuf, int recvcount, 
			const Datatype & recvtype, int source,
			int recvtag) const;

  virtual void Sendrecv_replace(void *buf, int count,
				const Datatype & datatype, int dest, 
				int sendtag, int source,
				int recvtag, Status & status) const;

  virtual void Sendrecv_replace(void *buf, int count,
				const Datatype & datatype, int dest, 
				int sendtag, int source,
				int recvtag) const;

  //
  // Groups, Contexts, and Communicators
  //
  
  virtual Group Get_group() const;
  
  virtual int Get_size() const;
  
  virtual int Get_rank() const;

  static int Compare(const Comm & comm1, const Comm & comm2);

  virtual void Free(void);
  
  virtual MPI2CPP_BOOL_T Is_inter() const;
  
  //
  //Process Topologies
  //

  virtual int Get_topology() const;
  
  //
  // Environmental Inquiry
  //

  virtual void Abort(int errorcode);

  virtual void Set_errhandler(const Errhandler& errhandler);

  virtual Errhandler Get_errhandler() const;

  static Errhandler Create_errhandler(PMPI_Errhandler_fn* function);

  //
  // Keys and Attributes
  //

  static int Create_keyval(PMPI_Copy_attr_function*,
			   PMPI_Delete_attr_function*,
			   void*);
  
  static void Free_keyval(int& comm_keyval);
  
  virtual void Set_attr(int comm_keyval, const void* attribute_val) const;
  
  virtual MPI2CPP_BOOL_T Get_attr(int comm_keyval, void* attribute_val) const;
  
  virtual void Delete_attr(int comm_keyval);
  
  static int NULL_COPY_FN(const PMPI::Comm& oldcomm, int comm_keyval,
			  void* extra_state, void* attribute_val_in,
			  void* attribute_val_out, MPI2CPP_BOOL_T& flag);
  
  static int DUP_FN(const Comm& oldcomm, int comm_keyval,
		    void* extra_state, void* attribute_val_in,
		    void* attribute_val_out, MPI2CPP_BOOL_T& flag);
  
  static int NULL_DELETE_FN(Comm& comm, int comm_keyval, void* attribute_val,
			    void* extra_state);
private:
  static Status ignored_status;
public: // JGS friends cause portability problems
  static Map mpi_comm_map;
  static Map key_fn_map;
  Errhandler* my_errhandler;

};

