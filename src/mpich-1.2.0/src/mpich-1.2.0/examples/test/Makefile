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
MAKE        = make --no-print-directory

### End User configurable options ###

SHELL = /bin/sh

PROFLIB =
CFLAGS  = $(OPTFLAGS) 
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
# Use LIBS to add any special libraries for C programs
LIBS = 
# Use FLIBS to add any special libraries for Fortran programs
FLIBS = 
TESTDIRS = pt2pt coll topol env context profile 
SUBDIRS = pt2pt coll topol env context profile 

default: subdirs
	@echo "Type \"make testing\" to compile and run tests"

subdirs:
	@for dir in $(SUBDIRS) ;\
		do \
			echo " " ;\
			echo Making in directory $$dir ;\
			if [ -d $$dir -a -s $$dir/Makefile ] ; then \
			    (cd $$dir; $(MAKE)) ; fi ;\
		done

linktest:
	@for dir in $(SUBDIRS) ;\
		do \
			echo " " ;\
			echo Making linktest in directory $$dir ;\
			if [ -d $$dir -a -s $$dir/Makefile ] ; then \
			    (cd $$dir; $(MAKE) linktest) ; fi ;\
		done

# Make sure that everything works (don't run the tests if mpirun fails
# for some reason).  We use runtests to handle finding and setting the mpirun
# environment (only the pt2pt test has this option).  runtest exits with 1 if
# the test fails, which will cause the Make to exit.
runtest:
	(cd pt2pt ; ./runtests $(TESTARGS) -check )

#
# This also cleans the files after the testing to keep the
# total size down.  You can use TESTARGS to pass arguments to the
# test program; for example, 
# make TESTARGS="-echo"
# to run in "echo" mode (show what scripts are doing)
# The tests are run in "small" mode (build one executable at a time) to
# reduce the amount of disk space that is used.
# The make clean is directed into dev null to keep the testing as 
# quiet as possible
testing: runtest
	@-for dir in $(TESTDIRS) ;\
		do \
			echo " " ;\
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
				continue ; fi ; \
			echo Making testing in directory $$dir ;\
			(cd $$dir; $(MAKE) testing TESTARGS="-small $(TESTARGS)" ; \
	                 $(MAKE) clean > /dev/null 2>&1 ) ;\
			echo End of testing in directory $$dir ;\
		done

#
# This is a version of testing, but it saves the "diff" files in the top 
# level directory
testing_save: runtest
	@-for dir in $(TESTDIRS) ;\
		do \
			echo " " ;\
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
				continue ; fi ; \
			echo Making testing in directory $$dir ;\
			(cd $$dir; $(MAKE) testing TESTARGS="-small $(TESTARGS)" ; \
	                 if [ -s $$dir.diff ] ; then cp $$dir.diff .. ; fi ; \
	    		 $(MAKE) clean > /dev/null 2>&1 ) ;\
			echo End of testing in directory $$dir ;\
		done

clean:
	@-/bin/rm -f test.log testing.log *.diff sendchar.sun4 sendchar.freebsd \
		sendshort.sun4 sendshort.freebsd *.o \
		sendvector.sun4 sendvector.freebsd \
		sendstruct.sun4 sendstruct.freebsd \
	        sendchar sendshort sendvector sendstruct
	@for dir in $(SUBDIRS) ;\
		do \
			echo " " ;\
			if [ ! -d $$dir -o ! -s $$dir/Makefile ] ; then \
			    continue ; fi ; \
			echo Making clean in directory $$dir ;\
			(cd $$dir; $(MAKE) clean) ;\
		done

# MPICH specific
#$(MPIR_HOME)/util/mpirun:
#	(cd $(MPIR_HOME) ; $(MAKE) mpirun )
