# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

CC          = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
CLINKER     = $(CC)
AR          = ar -r
RANLIB      = ranlib
OPTFLAGS    = 
MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
LIB_PATH    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11 
LOG_LIB     = -L/home/TDDB78/src/mpich-1.2.0/lib -llmpi -lmpe -lX11
PROF_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11
MPE_GRAPH   = -DMPE_GRAPHICS
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR)
LDFLAGS     = 

### End User configurable options ###

CFLAGS	  =  -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(OPTFLAGS) $(INCLUDE_DIR) 
EXECS	  = systest

default: $(EXECS)

all: default

#
# BSD 4.4 derived systems have a make which is INCOMPATIBLE with all other
# versions of make (!!!).  FreeBSD is one such system.  In these systems,
# the 'include' keyword has been DELETED and replaced with #include (Heaven
# help you if you expected # to start comments!).  Because of this
# inconsistant behavior, we can't use include to simplify the creation 
# of makefiles
#
# Just to make things interesting, some makes (FreeBSD, for one) complain
# if the comments do not START in column 1.  I've also changed include to
# -include- just to head off any other problems
#
# -include- $(MPIR_HOME)/profiling/lib/Makefile.timing
# -include- $(MPIR_HOME)/profiling/lib/Makefile.logging
# -include- $(MPIR_HOME)/profiling/lib/Makefile.vismess
# -include- $(MPIR_HOME)/profiling/lib/Makefile.allprof
#
# The following is the contents of $(MPIR_HOME)/profiling/lib/Makefile.logging
# If you think that this is ridiculous, then contact the BSD 4.4 people and
# ask them to conform to existing practice.
# (eventually we'll make configure determine if 'include' is broken and
# add a @ MAKEINCLUDE @ substitution.)

# Add $(PROF_OBJ) to the dependency line
# Add $(PROF_FLG) to the compile line
# Add $(PROF_LIB) to the link line, after the main code but before -lmpich
# Add $(PROF_CLN) to the clean line

WGEN_DIR  = $(MPE_DIR)/profiling/wrappergen
PROF_DIR  = $(MPE_DIR)/profiling/lib
PROF_DEF  = $(PROF_DIR)/log_wrappers.w
PROF_ARG  = -w $(PROF_DIR)/log_wrappers.w -f $(PROF_DIR)/pt2pt_fn
PROF_SRC  = log_wrap.c
PROF_CLN  = $(PROF_SRC) *.log
PROF_OBJ  = log_wrap.o
PROF_FLG  = -I$(MPE_DIR)/profiling/lib -I$(MPE_DIR)
PROFS_LIB  = $(PROF_OBJ) $(MPE_LIBS)

$(WGEN_DIR)/wrappergen:
	(cd $(WGEN_DIR); $(MAKE) wrappergen)

$(PROF_SRC): $(PROF_DEF) $(WGEN_DIR)/wrappergen
	$(WGEN_DIR)/wrappergen $(PROF_ARG) -o $(PROF_SRC)

# End of $(MPIR_HOME)/profiling/lib/Makefile.logging 

systest: systest.o $(PROF_OBJ)
	$(CLINKER) $(LDFLAGS) -o systest systest.o \
	$(PROF_LIB) -lm

clean:
	/bin/rm -f *.o *~ PI* $(EXECS) upshot

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f

