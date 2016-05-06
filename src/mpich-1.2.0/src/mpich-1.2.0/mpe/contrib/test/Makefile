# Generated automatically from Makefile.in by configure.

##### User configurable options #####

CC          = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
F77         = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpif77
CLINKER     = $(CC)
FLINKER     = $(F77)
MAKE        = make --no-print-directory
OPTFLAGS    = 
#
DEBUG_LIBS  = 
MPI_INC     = 
MPI_LIBS    = 
LIB_PATH    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11 $(MPI_LIBS) -lm 
FLIB_PATH   =  -L/home/TDDB78/src/mpich-1.2.0/lib -lfmpich -llmpi -lmpe $(MPI_LIBS)
MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
LOG_LIB     = -L/home/TDDB78/src/mpich-1.2.0/lib -llmpi -lmpe $(MPI_LIBS)
TRACE_LIB   = -L/home/TDDB78/src/mpich-1.2.0/lib -ltmpi $(MPI_LIBS)
ANIM_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lampi -lmpe -lX11 $(MPI_LIBS)
PROF_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe $(MPI_LIBS)
#
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR) $(MPI_INC)
LDFLAGS     = 
NO_X        = 0
NOF77       = 0

RM          = rm

### End User configurable options ###

VPATH=.:$(srcdir)
top_srcdir = /home/TDDB78/src/mpich-1.2.0/mpe
srcdir     = /home/TDDB78/src/mpich-1.2.0/mpe/contrib/test

CFLAGS  =  -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(OPTFLAGS) $(INCLUDE_DIR) 
FFLAGS  =  $(OPTFLAGS) $(INCLUDE_DIR)
EXECS   = cpilog cpi fpilog

default: $(EXECS)

cpi_log: cpi.o
	$(CLINKER) $(LDFLAGS) -o cpi_log cpi.o $(LOG_LIB) $(DEBUG_LIBS) -lm

cpi_trace: cpi.o
	$(CLINKER) $(LDFLAGS) -o cpi_trace cpi.o $(TRACE_LIB) $(DEBUG_LIBS) -lm

cpi_anim: cpi.o
	$(CLINKER) $(LDFLAGS) -o cpi_anim cpi.o $(ANIM_LIB) $(DEBUG_LIBS) -lm 

cpilog.o: ${srcdir}/cpilog.c
	$(CC) $(CFLAGS) -DUSE_GRAPHICS -o cpilog.o -c ${srcdir}/cpilog.c

cpilog: cpilog.o $(MPE_DIR)/mpe.h
	$(CLINKER) $(LDFLAGS)  -o cpilog cpilog.o $(LOG_LIB) $(DEBUG_LIBS) -lm 

fpilog: fpi.o
	$(FLINKER) $(OPTFLAGS) -o fpilog fpi.o $(FLIB_PATH) $(DEBUG_LIBS) -lm

linktest: linktest_C

linktest_C: 
	@-echo "** Testing if C application can be linked with logging library" ; \
	$(RM) -f cpi_log ; \
	$(MAKE) cpi_log ; \
	if test -x cpi_log ; then \
	    echo "** C application can be linked with logging library" ; \
	    $(RM) -f cpi_log ; \
	else \
	    echo "** C application cannot be linked with logging library" ; \
	fi 
	@-echo "** Testing if C application can be linked with tracing library" ; \
	$(RM) -f cpi_trace ; \
	$(MAKE) cpi_trace ; \
	if test -x cpi_trace ; then \
	    echo "** C application can be linked with tracing library" ; \
	    $(RM) -f cpi_trace ; \
	else \
	    echo "** C application cannot be linked with tracing library" ; \
	fi 
	@-if [ $(NO_X) = 0 ] ; then \
	    echo "** Testing if C application can be linked with animation library" ; \
	    $(RM) -f cpi_anim ; \
	    $(MAKE) cpi_anim ; \
	    if test -x cpi_anim ; then \
	        echo "** C application can be linked with animation library" ; \
	        $(RM) -f cpi_anim ; \
	    else \
	        echo "** C application cannot be linked with animation library" ; \
	    fi \
	fi  
	@-echo "** Testing if C application can use both automatic and manual logging together" ; \
	$(RM) -f cpilog ; \
	$(MAKE) cpilog ; \
	if test -x cpilog ; then \
	    echo "** C application can use both automatic and manual logging together" ; \
	    $(RM) -f cpilog ; \
	else \
	    echo "** C application cannot use both automatic and manual logging together" ; \
	fi 
	$(MAKE) clean

linktest_f77: 
	@if [ $(NOF77) = 0 ] ; then \
	    echo "** Testing if Fortran77 application can be linked with logging library" ; \
	    $(RM) -f fpilog ; \
	    $(MAKE) fpilog ; \
	    if test -x fpilog ; then \
	        echo "** Fortran77 application can be linked with logging library" ; \
	        $(RM) -f fpilog ; \
	    else \
	        echo "** Fortran77 application cannot be linked with logging library***" ; \
	    fi \
	fi

clean:
	$(RM) -f *.o *~ PI* $(EXECS) cpilog cpi_anim cpi_trace cpi_log fpilog

.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) -c $<

