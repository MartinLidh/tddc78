# Generated automatically from Makefile.in by configure.


SHELL		= /usr/bin/sh
CXX		= /home/TDDB78/src/mpich-1.2.0/bin/mpiCC
TEMPLATE_REP	= SunWS_cache  

# The ../.. includes the configuration file for VPATH builds
CXXFLAGS	= -O -I../..
LDFLAGS		= -O

srcdir=/home/TDDB78/src/mpich-1.2.0/MPI-2-C++/contrib/test_suite
top_srcdir=/home/TDDB78/src/mpich-1.2.0/MPI-2-C++
VPATH=.:$(srcdir)

#
# MPI2C++ Test suite
#
# 	

TEST_SRC	= mpi2c++_test.cc \
		messages.cc \
		stack.cc \
		signal.cc \
		allgather.cc \
		allreduce.cc \
		alltoall.cc \
		attr.cc \
		badbuf.cc \
		bcast.cc \
		bcast_struct.cc \
		bottom.cc \
		bsend.cc \
		buffer.cc \
		cancel.cc \
		cartcomm.cc \
		commdup.cc \
		commfree.cc \
		compare.cc \
		dims.cc \
		dup_test.cc \
		errhandler.cc \
		gather.cc \
		getcount.cc \
		getel.cc \
		graphcomm.cc \
		group.cc \
		groupfree.cc \
		initialized1.cc \
		initialized2.cc \
		intercomm.cc \
		interf.cc \
		iprobe.cc \
		isend.cc \
		op_test.cc \
		pack_test.cc \
		pcontrol.cc \
		probe.cc \
		procname.cc \
		rank_size.cc \
		reduce.cc \
		reduce_scatter.cc \
		request1.cc \
		rsend.cc \
		scan.cc \
		scatter.cc \
		send.cc \
		sendrecv.cc \
		sendrecv_rep.cc \
		split.cc \
		ssend.cc \
		start.cc \
		startall.cc \
		status_test.cc \
		struct_gatherv.cc \
		test1.cc \
		test3.cc \
		testall.cc \
		testany.cc \
		testsome.cc \
		topo.cc \
		waitall.cc \
		waitany.cc \
		waitsome.cc \
		wtime.cc

TEST_OBJ	= $(TEST_SRC:.cc=.o)
TEST_BIN	= mpi2c++_test


######################################################################

#
# General rules
#

.SUFFIXES: .cc .o

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<

######################################################################

#
# Rules
#

all: $(TEST_BIN)

#
# The actual test binary
#

$(TEST_BIN): $(TEST_OBJ)
	$(CXX) $(TEST_OBJ) -o $(TEST_BIN) $(LDFLAGS)


#
# clean
# On some weird Unix's, rm -f will complain if there are no files to 
# delete...
#

ALL_SRC		= $(TEST_SRC)
ALL_BIN		= $(TEST_BIN)

clean:
	@ touch bogus.o
	rm -rf *.o *~ *% core $(ALL_BIN) bogus $(TEMPLATE_REP)

distclean: clean
	@ touch bogus.o
	rm -rf Makefile

#
# depend
#

depend: $(ALL_SRC)
	$(CXX) -M $(DEPFLAGS) $(ALL_SRC) > makedep
	@ echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@ echo '$$r makedep' >>eddep
	@ echo 'w' >>eddep
	@ cp Makefile Makefile.bak
	@ ed - Makefile < eddep
	@ /bin/rm eddep makedep
	@ echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@ echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@ echo '# see make depend above' >> Makefile

# The following 4 (yes 4) lines must be in all the subdirectory makefiles
#-----------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
