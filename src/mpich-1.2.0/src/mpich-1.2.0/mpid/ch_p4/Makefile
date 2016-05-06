# Generated automatically from Makefile.in by configure.
#
#  $Id: Makefile.in,v 1.24 1999/11/29 18:34:40 gropp Exp $
#
#  {C} 1993,1995 by Argonne National Laboratory and Mississipi State University.
#      All rights reserved.  See COPYRIGHT in top-level directory.
#

CC          = cc
AR          = ar cr
RANLIB      = ranlib
OPTFLAGS    = -fast
SHELL       = /bin/sh
ASMFILES_O  = 
prefix      = /home/TDDB78/mpich-1.2.0/
top_srcdir  = /home/TDDB78/src/mpich-1.2.0
srcdir      = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4
libbuild_dir = /home/TDDB78/src/mpich-1.2.0/lib
DEFS        = -DHAVE_CONFIG_H -I. -I${srcdir} -I/home/TDDB78/src/mpich-1.2.0/include -I/home/TDDB78/src/mpich-1.2.0/include -I/home/TDDB78/src/mpich-1.2.0/mpid/ch_p4 -DHAVE_MPICH_MPID_H \
		-DMPID_DEVICE_CODE  -DHAVE_UNAME=1 -DHAVE_NETDB_H=1 -DHAVE_GETHOSTNAME=1 -DHAVE_SYSINFO=1 -DHAVE_SYS_SYSTEMINFO_H=1  -DMPID_DEBUG_NONE -DMPID_STAT_NONE 

VPATH=.:$(srcdir)

#ALL: default
ALL: libdev

P4_HOME  = ./p4
LIBNAME  = ${libbuild_dir}/libmpich.a
LIBDEV   = libmpid.a
# You need the ../ch_p4 for some broken compilers (versions of gcc)
# that make all of these paths relative to the directory an include is
# found in, not the directory where the compiler is running (!)
# Trailing / causes problems for some versions of gcc (was -I./)
# Was -I../.. -I../ch_p4
CFLAGS   =  -DFORTRANUNDERSCORE -DHAVE_MPICHCONF_H -I${top_srcdir}/mpid/ch2 -I${srcdir} -I${top_srcdir}/mpid/util -Ip4 -Ip4/include

# See mpid/ch_adi2/Makefile.in for a full list
SOURCE   = adi2recv.c adi2send.c adi2init.c adi2probe.c adi2ssend.c \
	   adi2hsend.c adi2hrecv.c adi2pack.c adi2mpack.c \
	   adi2hssend.c bswap2.c adi2req.c adi2cancel.c \
	   chbeager.c chbrndv.c chdebug.c chchkdev.c chshort.c chflow.c \
	   chinit.c chnodename.c chhetero.c chtick.c p4priv.c \
	   queue.c cmnargs.c mprerr.c tr2.c sbcnst2.c objtrace.c \
	   chpackflow.c calltrace.c chcancel.c 
SOURCEC  = ${SOURCE}
SOURCEF  =
MPI_OBJECTS = adi2recv.o adi2send.o adi2init.o adi2probe.o adi2ssend.o \
	      adi2hsend.o adi2hrecv.o adi2pack.o adi2mpack.o \
	      adi2hssend.o bswap2.o adi2req.o adi2cancel.o \
	      chbeager.o chbrndv.o chdebug.o chchkdev.o chshort.o chflow.o \
	      chinit.o chnodename.o chhetero.o chtick.o p4priv.o \
	      ${ASMFILES_O} queue.o cmnargs.o tr2.o sbcnst2.o \
	      chpackflow.o objtrace.o calltrace.o chcancel.o 

default: ${LIBNAME}
libdev: ${LIBDEV}

${MPI_OBJECTS}: ${srcdir}/chdef.h

# default_all is the same as default, but without the RANLIB.  This
# can speed up the build (ranlibs can take a LONG time).  profile_all 
# is the same, but for the profile library
default_all: setlinks p4inmpi default_all_files
default_all_files: ${MPI_OBJECTS}
	${AR} ${LIBNAME} $?
profile_all: 

# We need to clean the srcdir of any .o files because they can confuse the
# VPATH build process
clean:
	rm -f *~ ${MPI_OBJECTS} libmpid.a mprerr.o
	rm -f ${srcdir}/*.o
	(cd ${P4_HOME}; if [ -s Makefile ] ; then ${MAKE} clean; \
	rm -f lib/libp4.a ; fi ) 
#	rm -f Makefile lib/p4_config.h lib/libp4.a ; fi ;)

# 
# This doesn't remove the symbolic links, because there is no general easy
# way to identify that a filename is really a link (test -l works only on a 
# few systems; test -h works on more but not all).
distclean: 
	@for file in ${SOURCE} ; do  \
	    if [ $$file != p4priv.c ] ; then \
	        if [ -h $$file ] ; then \
                    /bin/rm -f $$file ; fi \
	    fi \
	done
	@for file in ${MPIDHEADERS} ; do \
	    if [ -h $$file ] ; then \
	        /bin/rm -f $$file ; fi \
	done
	@if [ -h mpiddev.h ] ; then \
	    /bin/rm -f mpiddev.h ; fi 
	@if [ -h sbcnst2.h ] ; then \
	    /bin/rm -f sbcnst2.h ; fi
	/bin/rm -f Makefile

MPIDHEADERS = mpid.h req.h comm.h datatype.h chhetero.h attach.h objtrace.h \
	      calltrace.h cookie.h mpid_bind.h reqalloc.h tr2.h mpimem.h \
	      chpackflow.h
setlinks:
	@for file in ${SOURCE} ; do  \
	    if [ ! -s $$file -a -s ${top_srcdir}/mpid/ch2/$$file ] ; then \
	 	ln -s ${top_srcdir}/mpid/ch2/$$file $$file ; fi ; \
	    if [ ! -s $$file -a -s ${top_srcdir}/mpid/util/$$file ] ; then \
		ln -s ${top_srcdir}/mpid/util/$$file $$file ; fi ; \
	done
	@for file in ${MPIDHEADERS} ; do \
	    if [ ! -s $$file -a -s ${top_srcdir}/mpid/ch2/$$file ] ; then \
		ln -s ${top_srcdir}/mpid/ch2/$$file $$file ; \
	    elif [ ! -s $$file -a -s ${top_srcdir}/mpid/util/$$file ] ; then \
		ln -s ${top_srcdir}/mpid/util/$$file $$file ; fi ; \
	done
	@if [ ! -s mpiddev.h ] ; then \
	    ln -s ${top_srcdir}/mpid/ch2/mpiddevbase.h mpiddev.h ; fi
	@if [ ! -s sbcnst2.h ] ; then \
	    ln -s ${top_srcdir}/mpid/util/sbcnst2.h sbcnst2.h ; fi

.c.o:
	${CC} ${DEFS} ${CFLAGS} ${BASEOPT} -c $< ${MPI_PROFILE}

#.c.s:
#	${CC} ${DEFS} ${CFLAGS} ${BASEOPT} -c -S $< ${MPI_PROFILE}

${LIBNAME}: ${MPI_OBJECTS} 
	${AR} $@ $?
	${RANLIB} $@

libmpid.a: ${MPI_OBJECTS} mprerr.o
	${AR} $@ $?
	${RANLIB} $@

configure: configure.in ../../aclocal.m4
	autoheader --localdir=../..
	autoconf   --localdir=../..

# Make sure that the make is the last statement to ensure that a problem
# in that make causes this make to abort
p4inmpi:
	(cd ${P4_HOME}; \
	${MAKE} p4inmpi MPIARCH=solaris MPILIBNAME=mpich)

