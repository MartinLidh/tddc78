# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

SHELL       = /bin/sh
ARCH        = solaris
MPIR_HOME   = /home/TDDB78/src/mpich-1.2.0
CC          = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CLINKER     = $(CC)
F77         = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
FLINKER     = $(F77)
F90         = /home/TDDB78/src/mpich-1.2.0/bin/mpif90
F90LINKER   = $(F90)      
CCC         = /home/TDDB78/src/mpich-1.2.0/bin/mpiCC
CCLINKER    = $(CCC)
AR          = ar cr
RANLIB      = ranlib
LOG_LIB     = -mpilog -lm
PROF_LIB    = -lmpe -lm
OPTFLAGS    = -fast
MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
F77_OPTFLAGS = 
#
srcdir=/home/TDDB78/src/mpich-1.2.0/examples/basic

VPATH=.:$(srcdir)
### End User configurable options ###

CFLAGS	  = $(OPTFLAGS) 
CFLAGSMPE = $(CFLAGS) -I$(MPE_DIR) 
CCFLAGS	  = $(CFLAGS)
#FFLAGS	  = '-qdpc=e' 
FFLAGS	  = $(OPTFLAGS)
EXECS	  = cpi systest srtest 

all: cpi systest srtest fpi cpilog hello++ iotest fibon 

default: $(EXECS)
	if [ "1" = 1 -a -s iotest.c ] ; then $(MAKE) iotest ; fi
	if [ "1" = 1 ] ; then $(MAKE) hello++ ; fi
	if [ "0" = 0 ] ; then $(MAKE) fpi ; fi
	if [ "0" = 0 ] ; then $(MAKE) cpilog ; fi
	if [ "0" = 0 ] ; then $(MAKE) cpi_autolog ; fi
	if [ -n "f90" -o "0" = 1 ] ; then $(MAKE) fibon ; fi


fpi: fpi.o 
	$(FLINKER) $(OPTFLAGS) -o fpi fpi.o

fibon: fibon.o 
	$(F90LINKER) $(OPTFLAGS) -o fibon fibon.o

cpi: cpi.o
	$(CLINKER) $(OPTFLAGS) -o cpi cpi.o -lm

cpi_autolog: cpi.o
	$(CLINKER) $(OPTFLAGS) -o cpi_autolog cpi.o $(LOG_LIB)

cpilog: cpilog.o
	$(CLINKER) $(OPTFLAGS) -o cpilog cpilog.o $(PROF_LIB)

srtest: srtest.o
	$(CLINKER) $(OPTFLAGS) -o srtest srtest.o -lm

systest: systest.o
	$(CLINKER) $(OPTFLAGS) -o systest systest.o -lm

iotest: iotest.o
	$(CLINKER) $(OPTFLAGS) -o iotest iotest.o -lm

hello++: hello++.o
	$(CCLINKER) hello++.o -o hello++

unsafe: unsafe.o
	$(CLINKER) $(OPTFLAGS) -o unsafe unsafe.o 
clean:
	/bin/rm -f *.o *~ PI* $(EXECS) fpi cpilog cpi_autolog hello++ iotest fibon upshot rdb.* startup.* 

cpilog.o: cpilog.c
	@if test -z "$(MPE_DIR)" ; then \
	    echo "Can only build cpilog if MPE libraries built" ; \
	    echo "Use -mpe to configure to have the MPE libraries built" ; \
	    exit 1 ; \
	fi
	$(CC) $(CFLAGSMPE) -c ${srcdir}/cpilog.c
.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(F77_OPTFLAGS) $(FFLAGS) -c $<
.cc.o:
	$(CCC) -c $<
.C.o:
	$(CCC) -c $<
.SUFFIXES:	\
	.cc .C
.f90.o:
	$(F90) -c $<
.SUFFIXES:	\
	.f90
