# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

MPIR_HOME    = 
CC           = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CLINKER      = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CCC          = /home/TDDB78/src/mpich-1.2.0/bin/mpiCC
CCLINKER     = $(CCC)
F77          = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
FLINKER      = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
OPTFLAGS     = 
MPIFDEP      = 
F77_OPTFLAGS = 
### End User configurable options ###

SHELL = /bin/sh
prefix     = 
top_srcdir = /home/TDDB78/src/mpich-1.2.0/examples/test
srcdir     = /home/TDDB78/src/mpich-1.2.0/examples/test/context
VPATH=.:$(srcdir)

PROFLIB =
CFLAGS  =  -DHAVE_STDLIB_H=1 -DUSE_STDARG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_LONG_LONG_INT=1 -DHAVE_PROTOTYPES=1 -DHAVE_SIGNAL_H=1 -DHAVE_SIGACTION=1 $(OPTFLAGS) 
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
# Use LIBS to add any special libraries for C programs
LIBS =  
# Use FLIBS to add any special libraries for Fortran programs
FLIBS =  
EXECS = attrtest attrt ictest ictest2 ictest3 grouptest groupcreate \
	attrerr
OTHEREXECS = attrtest
MPI2EXECS = commnames commnamesf
default: $(EXECS)

fortran_tests: attrtest commnamesf

#
# Note that runtests builds the executables as required
testing: 
	-./runtests $(TESTARGS)

linktest: default clean

all: testing

attrtest: $(MPIFDEP) attrtest.o
	$(FLINKER) $(OPTFLAGS) -o attrtest attrtest.o $(FLIBS)

commnamesf: $(MPIFDEP) commnamesf.o
	$(FLINKER) $(OPTFLAGS) -o commnamesf commnamesf.o $(FLIBS)

attrt: attrt.o test.o
	$(CLINKER) $(OPTFLAGS) -o attrt attrt.o test.o $(LIBS)

attrerr: attrerr.o test.o
	$(CLINKER) $(OPTFLAGS) -o attrerr attrerr.o test.o $(LIBS)

commnames: commnames.o
	$(CLINKER) $(OPTFLAGS) -o commnames commnames.o $(LIBS)
ictest: ictest.o
	$(CLINKER) $(OPTFLAGS) -o ictest ictest.o $(LIBS)
ictest2: ictest2.o
	$(CLINKER) $(OPTFLAGS) -o ictest2 ictest2.o $(LIBS)
ictest3: ictest3.o
	$(CLINKER) $(OPTFLAGS) -o ictest3 ictest3.o $(LIBS)
grouptest: grouptest.o
	$(CLINKER) $(OPTFLAGS) -o grouptest grouptest.o $(LIBS)
groupcreate: groupcreate.o
	$(CLINKER) $(OPTFLAGS) -o groupcreate groupcreate.o $(LIBS)

clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) $(OTHEREXECS) $(MPI2EXECS) \
		core *.out *.diff *.trace mpif.h ${srcdir}/*.o

.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) $(F77_OPTFLAGS) -c $<
