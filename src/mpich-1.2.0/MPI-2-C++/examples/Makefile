# Generated automatically from Makefile.in by configure.
##### User configurable options #####
# This is an example Makefile.in (or Makefile configured with mpireconfig)
# for the programs cpi, pi3, and cpilog.  

INSTALL_DIR = /home/TDDB78/mpich-1.2.0/
CXX         = /home/TDDB78/mpich-1.2.0//bin/mpiCC

EXECS       = hello_world ring topology user_bcast chapter_10_mpi2 pi

# Make sure .cc is recognized
.SUFFIXES: .o .cc

all: $(EXECS)

hello_world:  hello_world.o
	$(CXX) hello_world.o -o hello_world

pi: pi.o
	$(CXX) pi.o -o pi  -lm

ring:  ring.o
	$(CXX) ring.o -o ring

topology:  topology.o
	$(CXX) topology.o  -o topology

user_bcast:  user_bcast.o
	$(CXX) user_bcast.o  -o user_bcast

chapter_10_mpi2:  chapter_10_mpi2.o
	$(CXX) chapter_10_mpi2.o  -o chapter_10_mpi2

#
# clean
# On some weird Unix's, rm -f will complain if there are no files to 
# delete...
#

#
# The Solaris C++ compiler creates a directory containing miscellaneous
# data.  The action is not documented in the Solaris CC man pages
clean:
	@ touch bogus
	rm -rf *.o *~ *% core $(EXECS) bogus $(TEMPLATE_REP) SunWS_cache

.cc.o:
	$(CXX) -c $*.cc
