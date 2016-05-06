# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

ARCH        = solaris
MPIR_HOME   = /home/TDDB78/src/mpich-1.2.0
CC          = cc
CLINKER     = $(CC)
CCC         = CC
CCLINKER    = $(CCC)
F77         = f77
FLINKER     = $(F77)
AR          = ar cr
RANLIB      = ranlib
PROFILING   = $(PMPILIB)
OPTFLAGS    = -fast
MPE_GRAPH   = @MPE_GRAPHICS@
#
DEVICE      = ch_p4

### End User configurable options ###
SHELL = /bin/sh

# Were not ready to do contrib by default yet.
SUBDIRS = test perftest
TESTDIRS = test
EXECS = sendchar

default: subdirs

subdirs:
	@for dir in $(SUBDIRS) ;\
		do \
			echo " " ;\
			echo Making in directory $$dir ;\
			(cd $$dir; $(MAKE)) ;\
		done

testing:
	@for dir in $(TESTDIRS) ;\
		do \
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
				continue ; fi ; \
			echo " " ;\
			echo Making tests in directory $$dir ;\
			(cd $$dir; $(MAKE) testing) ;\
		done

linktest:
	@for dir in $(SUBDIRS) ;\
		do \
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
				continue ; fi ; \
			echo " " ;\
			echo Making linktest in directory $$dir ;\
			(cd $$dir; $(MAKE) linktest) ;\
		done

all: subdirs testing

clean:
	/bin/rm -f $(EXECS) *.o
	@for dir in $(SUBDIRS) ;\
		do \
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
				continue ; fi ; \
			echo " " ;\
			echo Making clean in directory $$dir ;\
			(cd $$dir; $(MAKE) clean) ;\
		done
