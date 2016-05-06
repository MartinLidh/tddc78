# Generated automatically from Makefile.in by configure.
##### User configurable options #####
# This is an example Makefile.in (or Makefile configured with mpireconfig)
# for the programs cpi, pi3, and cpilog.  

INSTALL_DIR = /home/TDDB78/mpich-1.2.0/
CC          = /home/TDDB78/mpich-1.2.0//bin/mpicc
CCC         = /home/TDDB78/mpich-1.2.0//bin/mpiCC
F77         = /home/TDDB78/mpich-1.2.0//bin/mpif77
CLINKER     = /home/TDDB78/mpich-1.2.0//bin/mpicc
CCLINKER    = /home/TDDB78/mpich-1.2.0//bin/mpiCC
FLINKER     = /home/TDDB78/mpich-1.2.0//bin/mpif77
F90         = /home/TDDB78/mpich-1.2.0//bin/mpif90
F90LINKER   = /home/TDDB78/mpich-1.2.0//bin/mpif90	  
OPTFLAGS    = -fast
MAKE        = make --no-print-directory
SHELL       = /bin/sh
#

### End User configurable options ###

CFLAGS  = $(OPTFLAGS)
FFLAGS =  $(OPTFLAGS)
LIBS =
FLIBS =
EXECS = cpi 

default: cpi pi3 cpip pi3p

# 
# The cp for pi3f90 is needed because different Fortran 90 compilers
# accept *different* suffixes.
# pi3f90 also wants an MPI module.  If modules not supported, don't 
# try to build pi3f90
all: $(EXECS)
	if [ "1" = 1 -a -s simpleio.c ] ; then $(MAKE) simpleio ; fi
	if [ "1" = 1 ] ; then $(MAKE) hello++ ; fi
	if [ "0" = 0 ] ; then  \
		$(MAKE) pi3 ; \
		$(MAKE) pi3p; \
	fi
	if [ "0" = 0 ] ; then $(MAKE) cpilog ; fi
	if [ "f90" != "false" -a -n "f90" -a -n "-M" ] ; then \
	    if [ ! -s pi3f90.f90 ] ; then \
		cp pi3f90.f90 pi3f90.f90 ; fi ; \
	    $(MAKE) pi3f90 ; fi

cpi: cpi.o 
	$(CLINKER) $(OPTFLAGS) -o cpi cpi.o -lm
cpip: cpip.o 
	$(CLINKER) $(OPTFLAGS) -o cpip cpip.o -lm

cpilog: cpilog.o 
	$(CLINKER) $(OPTFLAGS) -o cpilog cpilog.o -lmpe -lm

#
# This uses MPICH's -mpilog option to the mpicc command to add the automatic
# logging interface.
cpiwithlog: cpi.o
	$(CLINKER) $(OPTFLAGS) -o cpiwithlog cpi.o -mpilog -lm

pi3: pi3.o 
	$(FLINKER) $(OPTFLAGS) -o pi3 pi3.o $(FLIBS)
pi3p: pi3p.o 
	$(FLINKER) $(OPTFLAGS) -o pi3p pi3p.o $(FLIBS)

# Some Fortran 90 compilers do not accept a .f90 suffix (xlf90 for one)
pi3f90.o: pi3f90.f90
	$(F90) $(FFLAGS) -c pi3f90.f90
pi3f90: pi3f90.o 
	$(F90LINKER) $(OPTFLAGS) -o pi3f90 pi3f90.o $(FLIBS)

#
# Here are the rules for the optional package examples (ROMIO/C++)
simpleio: simpleio.o
	$(CLINKER) $(OPTFLAGS) -o simpleio simpleio.o 
hello++: hello++.o
	$(CCLINKER) $(OPTFLAGS) -o hello++ hello++.o
hello++.o: hello++.cc
	$(CCC) -c $(OPTFLAGS) hello++.cc

#
# The Solaris C++ compiler creates a directory containing miscellaneous
# data.  The action is not documented in the Solaris CC man pages
# We also remove any copy of pi3f90 that is created (e.g., pi3f90.f; pi3f90.f90
# is the master file)
clean:
	/bin/rm -f *.o *~ PI* $(EXECS) pi3 simpleio hello++ pi3f90 cpilog
	/bin/rm -rf SunWS_cache pi3f90.f pi3p

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f
.f90.o:
	$(F90) $(FFLAGS) -c $*.f90
.SUFFIXES: .f90
