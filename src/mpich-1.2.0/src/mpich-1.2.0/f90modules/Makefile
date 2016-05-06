# Generated automatically from Makefile.in by configure.
#
# 
SHELL = /bin/sh
#
F90 = f90
F90LINKER = f90
F90SUFFIX = f90
# Module suffix.  M and mod are known
MOD = mod
SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .o .$(MOD) .$(F90SUFFIX) .f
#
prefix = /usr/local
includedir = ${prefix}/include
includechoice_dir = ${includedir}/f90choice
includenochoice_dir = ${includedir}/f90base
mpiincdir  = ../include
libbuild_dir = /home/TDDB78/src/mpich-1.2.0/lib
includebuild_dir = /home/TDDB78/src/mpich-1.2.0/include
top_srcdir = /home/TDDB78/src/mpich-1.2.0/f90modules
srcdir     = /home/TDDB78/src/mpich-1.2.0/f90modules

VPATH=.:$(srcdir)
#
INSTALL_DATA = ${INSTALL} -m 644
#
#
all: mpi.$(MOD) mpi-base.$(MOD)
#
# There isn't a single suffix that all compilers accept.  Many take f90 and
# a few insist on it, so that is our default.  Some (e.g., IBM) refuse
# to take anything but f, so we handle that here.
# Because of VPATH, we may also need to make a copy of a few files to
# the local directory as well.
copysrcs:
	for file in mpi1 mpi2 mpi_constants ; do \
	    if [ ! -s $${file}.${F90SUFFIX} ] ; then \
		cp ${srcdir}/$$file.f90 $${file}.${F90SUFFIX} ; fi ; \
	done
mpi.$(MOD): copysrcs \
	mpi.$(F90SUFFIX) mpi_constants.$(MOD) \
	mpi1.$(MOD) mpi2.$(MOD) \
	choice_modules
	if [ -s mpi.$(F90SUFFIX) ] ; then \
	    $(F90) -c $(FFLAGS) mpi.$(F90SUFFIX) ; \
	else \
	    $(F90) -c $(FFLAGS) ${srcdir}/mpi.$(F90SUFFIX) ; fi
	ar cr ${libbuild_dir}/libmpichf90.a ./mpi.o	
	if [ ! -d ${includebuild_dir}/f90choice ] ; then \
		mkdir ${includebuild_dir}/f90choice ; fi
	cp mpi*.$(MOD) ${includebuild_dir}/f90choice
mpi-base.$(MOD): mpi.$(F90SUFFIX) \
	mpi_constants.$(MOD) mpi1.$(MOD) mpi2.$(MOD)
	if [ ! -d .tmp ] ; then mkdir .tmp ; fi
	cp mpi-nochoice.$(F90SUFFIX) .tmp/mpi.$(F90SUFFIX)
	rm -f .tmp/mpi2.$(MOD) .tmp/mpi1.$(MOD) .tmp/mpi_constants.$(MOD)
	(cd .tmp ; ln -s ../mpi1.$(MOD) mpi1.$(MOD) ;\
	ln -s ../mpi2.$(MOD) mpi2.$(MOD) ;\
	ln -s ../mpi_constants.$(MOD) mpi_constants.$(MOD) ;\
	$(F90) -c $(FFLAGS) ./mpi.$(F90SUFFIX) )
	ar cr ${libbuild_dir}/libmpichf90nc.a .tmp/mpi.o mpi1.o mpi2.o \
		mpi_constants.o
	cp .tmp/mpi.$(MOD) mpi-base.$(MOD)
	rm -f .tmp/*
	if [ -d .tmp ] ; then rmdir .tmp ; fi
	if [ ! -d ${includebuild_dir}/f90base ] ; then \
		mkdir ${includebuild_dir}/f90base ; fi
	cp mpi-base.${MOD} ${includebuild_dir}/f90base
	cp mpi_constants.${MOD} ${includebuild_dir}/f90base
	cp mpi1.${MOD} ${includebuild_dir}/f90base
	cp mpi2.${MOD} ${includebuild_dir}/f90base

mpi_constants.$(MOD): mpi_constants.$(F90SUFFIX) $(mpiincdir)/mpif.h
	$(F90) $(FFLAGS) -c -I$(mpiincdir) mpi_constants.$(F90SUFFIX)
	ar cr ${libbuild_dir}/libmpichf90.a mpi_constants.o
# Systems that make the module name upper case when the file name is lower
# case make this harder than it should be.
mpi1.$(MOD): mpi1.$(F90SUFFIX)
	$(F90) $(FFLAGS) -c mpi1.${F90SUFFIX}
	ar cr ${libbuild_dir}/libmpichf90.a mpi1.o
mpi2.$(MOD): mpi2.$(F90SUFFIX)
	$(F90) $(FFLAGS) -c mpi2.${F90SUFFIX}
	ar cr ${libbuild_dir}/libmpichf90.a mpi2.o

# These are for the maintainers only
ExtractRawBindings:
CreateModuleSrc:
mpi1f.h: ExtractRawBindings
	if [ -x ExtractRawBindings ] ; then ./ExtractRawBindings ; fi
	touch mpi1f.h
mpi1__type_s.f90 mpi1.f90 mpi2.f90: mpi1f.h CreateModuleSrc
	if [ -x ./CreateModuleSrc ] ; then \
	    ./CreateModuleSrc -exclude=${srcdir}/exclude ; fi
# to here

# This installation target is suitable when installing Fortran 90 
# support for non-MPICH implementations of MPI.  
install: mpi.$(MOD) mpi-base.$(MOD)
	if [ ! -d $(includechoice_dir) ] ; then \
		mkdir $(includechoice_dir ; fi
	for file in (*.$(MOD)) ; do \
	        $(INSTALL_DATA) $$file $(includechoice_dir) ; done
	if [ ! -d $(includenochoice_dir) ] ; then \
		mkdir $(includenochoice_dir) ; fi
	$(INSTALL_DATA) mpi-base.$(MOD) $(includenochoice_dir)/mpi.$(MOD)
	$(INSTALL_DATA) mpi1.$(MOD) $(includenochoice_dir)
	$(INSTALL_DATA) mpi2.$(MOD) $(includenochoice_dir)
	$(INSTALL_DATA) mpi_constants.$(MOD) $(includenochoice_dir)

uninstall:


#
# Use -traditional to get the integer*2 types if you perfer that
# Use -types="integer8 real2" if you want to specify specific types
mpi.$(F90SUFFIX): mpi1__type_s.f90
	( MAKE="make --no-print-directory" ; export MAKE ; \
	${srcdir}/CreateChoiceSrc -kind  -f90suffix=f90 )

#
# This MUST use f90 as the suffix, because we use this to find the
# files.  The make command will find the correct file when the suffix is
# different from f90
# This must *not* use srcdir, since each system will generate slightly
# different (based on available data kinds) files
choice_modules: 
	for file in `ls mpi[12]__[cilr][a-z0-9]*_[vs]*.${F90SUFFIX}` ; do \
	    filename=`basename $$file .${F90SUFFIX}` ; \
	    make --no-print-directory -f Makefile $$filename.$(MOD) ; done

f90modflag:
	@echo "-M"
f90suffix:
	@echo "f90"
#
# Add the module's object file to the module library as well.
.$(F90SUFFIX).$(MOD): 
	$(F90) $(FFLAGS) -c $<
	ar cr ${libbuild_dir}/libmpichf90.a $*.o

# If there is ANOTHER f90 suffix, add it here rather than using
# $(F90SUFFIX) in case .f or .f90 is the F90SUFFIX
.f90:
	$(F90) -c $(FFLAGS) $<
	$(F90LINKER) -o $* $(FFLAGS) $*.o
.f:
	$(F90) -c $(FFLAGS) $<
	$(F90LINKER) -o $* $(FFLAGS) $*.o
.f.o:
	$(F90) -c $(FFLAGS) $<

configure: configure.in
	autoconf --local_dir=..

# Remove both .f90 and .f incase the Fortran90 compiler does not accept f90
# Ditto for .M and .mod
clean:
	@-rm -f *.o *.M conftest* ${srcdir}/*.M ${srcdir}/*.o *.mod \
		${srcdir}/*.mod ${srcdir}/mpi.f90
	@-rm -f  mpi1__[cilr][a-z0-9]*_[vs]*.f90 \
	       mpi2__[cilr][a-z0-9]*_[vs]*.f90 \
	       mpi.add mpi.f90 mpi-nochoice.f90 \
	       mpi1__[cilr][a-z0-9]*_[vs]*.f \
	       mpi2__[cilr][a-z0-9]*_[vs]*.f \
	       mpi.f mpi-nochoice.f
distclean: clean
maintainer-clean: distclean
	rm -f  mpi1__type_[sv]*.f90 mpi2__type_[sv]*.f90 mpi1.f90 mpi2.f90
	rm -f mpi1f.h mpi1fv.h mpi2f.h mpi2fv.h mpi1fvs.h mpi1fsv.h \
		mpi1fs.h mpi2fvs.h mpi2fsv.h mpi2fs.h
