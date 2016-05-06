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
srcdir     = /home/TDDB78/src/mpich-1.2.0/examples/test/topol
VPATH=.:$(srcdir)

PROFLIB =
CFLAGS  =  -DHAVE_STDLIB_H=1 -DUSE_STDARG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_LONG_LONG_INT=1 -DHAVE_PROTOTYPES=1 -DHAVE_SIGNAL_H=1 -DHAVE_SIGACTION=1 $(OPTFLAGS) 
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
# Use LIBS to add any special libraries for C programs
LIBS =  
# Use FLIBS to add any special libraries for Fortran programs
FLIBS =  
EXECS = cart cart2 graphtest cartmap cartorder

default: $(EXECS)

#
# Note that runtests builds the executables as required
testing: 
	-./runtests $(TESTARGS)

linktest: default clean

all: testing

fortran_tests: cartf twod cart1f cart2f

cart: cart.o test.o
	$(CLINKER) $(OPTFLAGS) -o cart cart.o test.o $(LIBS)

cartmap: cartmap.o
	$(CLINKER) $(OPTFLAGS) -o cartmap cartmap.o $(LIBS)

cartorder: cartorder.o test.o
	$(CLINKER) $(OPTFLAGS) -o cartorder cartorder.o test.o $(LIBS)

cart2: cart2.o test.o
	$(CLINKER) $(OPTFLAGS) -o cart2 cart2.o test.o $(LIBS)

graphtest: graphtest.o
	$(CLINKER) $(OPTFLAGS) -o graphtest graphtest.o $(LIBS)

cartf: $(MPIFDEP) cartf.o
	$(FLINKER) $(OPTFLAGS) -o cartf cartf.o $(FLIBS)

cart1f: $(MPIFDEP) cart1f.o
	$(FLINKER) $(OPTFLAGS) -o cart1f cart1f.o $(FLIBS)

cart2f: $(MPIFDEP) cart2f.o
	$(FLINKER) $(OPTFLAGS) -o cart2f cart2f.o $(FLIBS)

twod: $(MPIFDEP) twod.o
	$(FLINKER) $(OPTFLAGS) -o twod twod.o $(FLIBS)

clean:
	@-/bin/rm -f *.o *~ core PI* $(EXECS) cartf twod cart1f cart2f \
	*.out topol.diff *.trace mpif.h ${srcdir}/*.o

.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) $(F77_OPTFLAGS) -c $<
