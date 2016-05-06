# Generated automatically from Makefile.in by configure.
# -*- makefile -*-
#
# Copyright 1997-1999, University of Notre Dame.
# Authors: Jeremy G. Siek, Michael P. McNally, Jeffery M. Squyres,
#          Andrew Lumsdaine

# This file is part of the Notre Dame C++ bindings for MPI

# You should have received a copy of the License Agreement for the
# Notre Dame C++ bindings for MPI along with the software;  see the
# file LICENSE.  If not, contact Office of Research, University of Notre
# Dame, Notre Dame, IN  46556.

# Permission to modify the code and to distribute modified code is
# granted, provided the text of this NOTICE is retained, a notice that
# the code was modified is included with the above COPYRIGHT NOTICE and
# with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
# file is distributed with the modified code.

# LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
# By way of example, but not limitation, Licensor MAKES NO
# REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
# PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
# OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
# OR OTHER RIGHTS.
#

SHELL		= /usr/bin/sh
CXX		= /home/TDDB78/src/mpich-1.2.0/bin/mpiCC
TEMPLATE_REP	= SunWS_cache  
MPILIB_DIR      = /home/TDDB78/src/mpich-1.2.0/lib
NOPRO_OR_PRO	= profile

srcdir=/home/TDDB78/src/mpich-1.2.0/MPI-2-C++/src
top_srcdir=/home/TDDB78/src/mpich-1.2.0/MPI-2-C++
VPATH=.:$(srcdir)
#
# Library functions
#

AR		= ar
RANLIB		= ranlib
# The configuration header file is in the upper directory
CXXFLAGS	=   -I..

#
# Source files
#

LIBMPIPP_SRCS	= mpi++.cc

LIBPMPIPP_SRCS	= mpi++.cc \
			pmpi++.cc \
			op.cc \
			comm.cc \
			intracomm.cc \
			topology.cc \
			intercomm.cc \
			request.cc \
			group.cc \
			datatype.cc \
			status.cc \
			errhandler.cc \
			exception.cc \
			functions.cc

LIBMPIPP_OBJS	= $(LIBMPIPP_SRCS:.cc=.o)

LIBPMPIPP_OBJS	= $(LIBPMPIPP_SRCS:.cc=.o)


#
# General compilation rule
#

.SUFFIXES: .cc .o

.cc.o:
	$(CXX) $< -c $(CXXFLAGS)


#
# Build rules
#

all: $(NOPRO_OR_PRO)

noprofile: $(LIBMPIPP_OBJS)
	$(AR) vur libmpi++.a $(LIBMPIPP_OBJS)
	$(RANLIB) libmpi++.a
	cp -p libmpi++.a $(MPILIB_DIR)/libmpich++.a

profile: $(LIBPMPIPP_OBJS) $(LIBMPIPP_OBJS)
	$(AR) vur libmpi++.a $(LIBPMPIPP_OBJS)
	$(RANLIB) libmpi++.a
	cp -p libmpi++.a $(MPILIB_DIR)/libmpich++.a

#
# clean
# On some weird Unix's, rm -f will complain if there are no files to 
# delete...
#

clean:
	@ touch bogus
	rm -rf core *.o *~ *.a bogus $(TEMPLATE_REP)

distclean: clean
	@ touch bogus
	rm -f *.a Makefile config.h bogus

#
# You may need to customize this for your system -- the
# Solaris "makedepend" command does not do nice things
# for C++ source files. :-(
#
# Note that the Solaris CC compiler needs "-xM" to generate
# a dependency list.  Your compiler may be different!
#

depend: $(LIBMPIPP_SRCS) $(LIBPMPIPP_SRCS)
	$(CXX) -xM $(CXXFLAGS) $(LIBMPIPP_SRCS) $(LIBPMPIPP_SRCS) > makedep
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

