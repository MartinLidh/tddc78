# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

SHELL = /bin/sh
ARCH         = 
DEVICE       = 
MPIR_HOME    = /home/TDDB78/src/mpich-1.2.0/examples/perftest
CC           = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
CLINKER      = $(CC)
F77          = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
FLINKER      = $(F77)
OPTFLAGS     = 
F77_OPTFLAGS = 
### End User configurable options ###

VPATH=.:$(srcdir)
srcdir     = /home/TDDB78/src/mpich-1.2.0/examples/perftest


CFLAGS  = $(OPTFLAGS)
CCFLAGS = $(CFLAGS)
FFLAGS = $(OPTFLAGS)
LIBS =  -lm
EXECS = mpptest goptest vectest stress buflimit ctest cluster

default: $(EXECS)

OBJS = gopf.o grph.o ovlap.o pair.o pattern.o util.o getopts.o \
	rate.o mpe_seq.o copy.o
#OBJS = gopf.o grph.o ovlap.o pair.o pattern.o util.o getopts.o \
#	tstauto.o rate.o mpe_seq.o copy.o

mpptest: mpptest.o $(OBJS)
	$(CLINKER) $(OPTFLAGS) -o ./mpptest mpptest.o $(OBJS) $(LIBS)

goptest: goptest.o $(OBJS)
	$(CLINKER) $(OPTFLAGS) -o ./goptest goptest.o $(OBJS) $(LIBS)

copytest: copytest.o
	$(CLINKER) $(OPTFLAGS) -o ./copytest copytest.o $(LIBS)

stress: stress.o daytime.o getopts.o
	$(CLINKER) $(OPTFLAGS) -o ./stress stress.o daytime.o \
		getopts.o $(LIBS)

buflimit: buflimit.o
	$(CLINKER) $(OPTFLAGS) -o ./buflimit buflimit.o $(LIBS)

tunepkt: tunepkt.o
	$(CLINKER) $(OPTFLAGS) -o ./tunepkt tunepkt.o $(LIBS)

pktuse: pktuse.o
	$(CLINKER) $(OPTFLAGS) -o ./pktuse pktuse.o $(LIBS)

ctest: ctest.o
	$(CLINKER) $(OPTFLAGS) -o ./ctest ctest.o $(LIBS)

cluster: cluster.o
	$(CLINKER) $(OPTFLAGS) -o ./cluster cluster.o $(LIBS)

vectest: vectest.o
	$(FLINKER) $(OPTFLAGS) -o ./vectest vectest.o  $(LIBS)

clean:
	@-/bin/rm -f *.o *~ $(EXECS) tunepkt pktuse core TAGS .foo *.gpl \
	           *.log gop.ps *.gpl *.gcm makeplot info.txt basetest.tar \
	           copytest gmon.* config.status

#
# We'd like to eliminate aclocal.m4 from the directory, but the old autoconf
# can't handle this. Sigh
configure: configure.in
	/home/MPI/maint/makeconfigs 

.c.o:
	$(CC) $(CFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) $(F77_OPTFLAGS) -c $<



