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


class Datatype {
public:
  // construction / destruction
  inline Datatype() : mpi_datatype(MPI_DATATYPE_NULL) { }
  inline virtual ~Datatype() {}
  // inter-language operability
  inline Datatype(const MPI_Datatype &i) : mpi_datatype(i) { }

  // copy / assignment
  Datatype(const Datatype& dt) : mpi_datatype(dt.mpi_datatype) { }

  Datatype& operator=(const Datatype& dt) {
    mpi_datatype = dt.mpi_datatype; return *this;
  }
  
  // comparison
  MPI2CPP_BOOL_T operator==(const Datatype &a) const {
    return (MPI2CPP_BOOL_T) (mpi_datatype == a.mpi_datatype);
  }

  inline MPI2CPP_BOOL_T operator!= (const Datatype &a) const 
    { return (MPI2CPP_BOOL_T) !(*this == a); }

  // inter-language operability
  inline Datatype& operator= (const MPI_Datatype &i) 
    { mpi_datatype = i; return *this; }

  inline operator MPI_Datatype () const { return mpi_datatype; }

  inline const MPI_Datatype& mpi() const { return mpi_datatype; }

  //
  // Point-to-Point Communication
  //

  inline virtual Datatype Create_contiguous(int count) const;

  virtual Datatype Create_vector(int count, int blocklength,
					int stride) const;

  virtual Datatype Create_indexed(int count,
				       const int array_of_blocklengths[], 
				       const int array_of_displacements[]) const;
  
  static Datatype Create_struct(int count, const int array_of_blocklengths[],
				     const Aint array_of_displacements[],
				     const Datatype array_if_types[]);
  

  virtual Datatype Create_hindexed(int count, const int array_of_blocklengths[],
					const Aint array_of_displacements[]) const;

  virtual Datatype Create_hvector(int count, int blocklength,
				       Aint stride) const;

  virtual int Get_size() const;

  virtual void Get_extent(Aint& lb, Aint& extent) const;

  virtual void Commit();

  virtual void Free();

  //JGS I did not make these inline becuase of the dependency
  //on the Comm conversion function
  virtual void Pack(const void* inbuf, int incount, void *outbuf, 
		    int outsize, int& position, const Comm &comm) const;
  virtual void Unpack(const void* inbuf, int insize, void *outbuf,
		      int outcount, int& position, const Comm& comm) const;
  virtual int Pack_size(int incount, const Comm& comm) const;

protected:
  MPI_Datatype mpi_datatype;

};
