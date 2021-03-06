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

#####################################################################

SHELL		= /usr/bin/sh
INSTALL		= /home/pelab/pkg/gnu/bin/install -c
INSTALLDIR	= /usr/local
RANLIB		= ranlib
LN_S		= ln -s
NAMEDIR		= mpi2c++

#####################################################################


all: clean lib examples

lib:
	@ (cd src; $(MAKE))

examples:
	@ (cd contrib/test_suite; $(MAKE))
	@ (cd contrib/examples; $(MAKE))

install: install-dirs
	$(INSTALL) -m 755 balky/balky $(INSTALLDIR)/bin
	$(INSTALL) -m 644 src/libmpi++.a $(INSTALLDIR)/lib
	$(RANLIB) $(INSTALLDIR)/lib/libmpi++.a
	ver=`cat VERSION | cut -d\  -f5'`; \
	for file in `ls src/*.h`; do \
		$(INSTALL) -m 644 $$file $(INSTALLDIR)/include/$(NAMEDIR)-$$ver; \
	done; \
	if test ! -f $(INSTALLDIR)/include/$(NAMEDIR)-$$ver/mpi++.h; then \
		$(LN_S) $(INSTALLDIR)/include/$(NAMEDIR)-$$ver/mpi++.h $(INSTALLDIR)/include; \
	fi; \
	cd $(INSTALLDIR)/include; rm -f $(NAMEDIR); $(LN_S) $(NAMEDIR)-$$ver $(NAMEDIR)

install-dirs:
	@ if test ! -d $(INSTALLDIR)/lib; then \
	  echo "Making directory: $(INSTALLDIR)/lib"; \
	  mkdir -p $(INSTALLDIR)/lib; \
	fi
	@ if test ! -d $(INSTALLDIR)/bin; then \
	  echo "Making directory: $(INSTALLDIR)/bin"; \
	  mkdir -p $(INSTALLDIR)/bin; \
	fi
	@ if test ! -d $(INSTALLDIR)/include; then \
	  echo "Making directory: $(INSTALLDIR)/include"; \
	  mkdir -p $(INSTALLDIR)/include; \
	fi
	@ ver=`cat VERSION | cut -d\  -f5'`; \
	if test ! -d $(INSTALLDIR)/include/$(NAMEDIR)-$$ver; then \
	  echo "Making directory: $(INSTALLDIR)/include/$(NAMEDIR)-$$ver"; \
	  mkdir -p $(INSTALLDIR)/include/$(NAMEDIR)-$$ver; \
	fi

uninstall:
	ver=`cat VERSION | cut -d\  -f5'`; \
	if test -d $(INSTALLDIR)/include/$(NAMEDIR); then \
		yow=`pwd`; cd $(INSTALLDIR)/include/$(NAMEDIR); \
		p=`pwd`; cd $$yow; \
		if test "$$p" = "$(INSTALLDIR)/include/$(NAMEDIR)-$$ver"; then \
			rm -rf $(INSTALLDIR)/include/$(NAMEDIR); \
		fi ; \
	fi ; \
	rm -rf $(INSTALLDIR)/lib/libmpi++.a $(INSTALLDIR)/include/mpi++.h \
	        $(INSTALLDIR)/include/$(NAMEDIR)-$$ver $(INSTALLDIR)/bin/balky

clean:
	rm -rf *.bak *~ *% core balky/*~ balky/*% balky/*.bak
	@ subdirs="src contrib/examples contrib/test_suite"; \
	for dir in $$subdirs; do \
		(cd $$dir; $(MAKE) clean); \
	done

distclean:
	@ subdirs="src contrib/examples contrib/test_suite"; \
	for dir in $$subdirs; do \
		(cd $$dir; $(MAKE) distclean); \
	done
	rm -f balky/balky balky/mini-balky
	rm -rf config.cache config.log config.status Makefile

dist: distclean
	(mydir=`pwd`; \
         last=`basename $$mydir`; \
	 ver=`cat VERSION | cut -d\  -f5'`; \
	 if test "$$last" != "MPI-2-C++-$$ver"; then \
           (cd ..; mv $$mydir MPI-2-C++-$$ver; ); \
         fi; \
	 (cd ..; \
	  rm -f rcs_dirs; \
          find MPI-2-C++-$$ver -name RCS -print > rcs_dirs; \
          find MPI-2-C++-$$ver -name \*.o -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name \*.ii -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name \*.bak -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name \*.old -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name \*.gz -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name \*.Z -print >> rcs_dirs; \
          find MPI-2-C++-$$ver -name Template\* -print >> rcs_dirs; \
	  find MPI-2-C++-$$ver -name auto_test\* -print >> rcs_dirs; \
	  tar cvfX MPI-2-C++-$$ver.tar rcs_dirs MPI-2-C++-$$ver; \
	  gzip MPI-2-C++-$$ver.tar; \
	  tar cvfX MPI-2-C++-$$ver.tar rcs_dirs MPI-2-C++-$$ver; \
	  compress MPI-2-C++-$$ver.tar; \
	  rm -f rcs_dirs;); \
	 mv ../MPI-2-C++-$$ver.tar.gz .; \
	 mv ../MPI-2-C++-$$ver.tar.Z .;)
