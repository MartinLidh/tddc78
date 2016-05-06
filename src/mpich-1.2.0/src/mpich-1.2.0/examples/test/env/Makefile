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
F77_OPTFLAGS = 
### End User configurable options ###

SHELL = /bin/sh
prefix     = 
top_srcdir = /home/TDDB78/src/mpich-1.2.0/examples/test
srcdir     = /home/TDDB78/src/mpich-1.2.0/examples/test/env
VPATH=.:$(srcdir)

PROFLIB =
CFLAGS  =  -DHAVE_STDLIB_H=1 -DUSE_STDARG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_LONG_LONG_INT=1 -DHAVE_PROTOTYPES=1 -DHAVE_SIGNAL_H=1 -DHAVE_SIGACTION=1 $(OPTFLAGS) 
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
# Use LIBS to add any special libraries for C programs
LIBS =  
# Use FLIBS to add any special libraries for Fortran programs
FLIBS =  
EXECS = timers baseattr errhand sigchk timertest init gtime aborttest testerr \
	cmdline errhand2
OTHEREXECS = baseattrf errstringsf
default: $(EXECS)

#
# Note that runtests builds the executables as required
testing:
	-./runtests $(TESTARGS)

linktest: default clean

all: testing

init: init.o
	$(CLINKER) $(OPTFLAGS) -o init init.o $(LIBS)
timers: timers.o test.o
	$(CLINKER) $(OPTFLAGS) -o timers timers.o test.o $(LIBS)
timertest: timertest.o test.o
	$(CLINKER) $(OPTFLAGS) -o timertest timertest.o test.o $(LIBS)
baseattr: baseattr.o test.o
	$(CLINKER) $(OPTFLAGS) -o baseattr baseattr.o test.o $(LIBS)
gtime: gtime.o test.o
	$(CLINKER) $(OPTFLAGS) -o ./gtime gtime.o test.o $(LIBS) -lm

baseattrf: baseattrf.o
	$(FLINKER) $(OPTFLAGS) -o baseattrf baseattrf.o $(LIBS)

errstringsf: errstringsf.o
	$(FLINKER) $(OPTFLAGS) -o errstringsf errstringsf.o $(LIBS)

errhand: errhand.o test.o
	$(CLINKER) $(OPTFLAGS) -o errhand errhand.o test.o $(LIBS)
sigchk: sigchk.o test.o
	$(CLINKER) $(OPTFLAGS) -o sigchk sigchk.o test.o $(LIBS)
aborttest: aborttest.o
	$(CLINKER) $(OPTFLAGS) -o aborttest aborttest.o $(LIBS)
testerr: testerr.o
	$(CLINKER) $(OPTFLAGS) -o testerr testerr.o $(LIBS)
clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) core *.out *.orig env.diff \
	argstest $(OTHEREXECS) mpif.h ${srcdir}/*.o

.c:
	$(CC) $(CFLAGS) -c $<
	$(CLINKER) $(OPTFLAGS) -o $* $*.o $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) $(F77_OPTFLAGS) -c $<
