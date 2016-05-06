# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

MPIR_HOME   = 
CC          = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CLINKER     = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CCC         = /home/TDDB78/src/mpich-1.2.0/bin/mpiCC
CCLINKER    = $(CCC)
F77         = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
FLINKER     = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
OPTFLAGS    = 

### End User configurable options ###

SHELL = /bin/sh
prefix     = 
top_srcdir = /home/TDDB78/src/mpich-1.2.0/examples/test
srcdir     = /home/TDDB78/src/mpich-1.2.0/examples/test/profile
VPATH=.:$(srcdir)

PROFLIB =
CFLAGS  =  -DHAVE_STDLIB_H=1 -DUSE_STDARG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_LONG_LONG_INT=1 -DHAVE_PROTOTYPES=1 -DHAVE_SIGNAL_H=1 -DHAVE_SIGACTION=1 $(OPTFLAGS) 
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
# Use LIBS to add any special libraries for C programs
LIBS =  
# Use FLIBS to add any special libraries for Fortran programs
FLIBS =  
EXECS = ptest
OTHEREXECS = 
default: $(EXECS)

#
# Note that runtests builds the executables as required
testing: 
	  -./runtests $(TESTARGS)

all: testing

fortran_tests: 

ptest: ptest.o 
	$(CLINKER) $(OPTFLAGS) -o ptest ptest.o $(LIBS)

#
# Make sure that we remove executables for specific architectures
clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) *.out core profile.diff \
		$(OTHEREXECS) mpif.h ${srcdir}/*.o
	@-for file in $(EXECS) Makefile ; do \
	   /bin/rm -f $$file.sun4 $$file.alpha $$file.IRIX $$file.freebsd ;\
	done

.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) -c $<
