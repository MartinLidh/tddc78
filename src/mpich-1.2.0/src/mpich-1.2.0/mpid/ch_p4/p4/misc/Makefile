# Generated automatically from Makefile.in by configure.
P4ARCH = SUN_SOLARIS
MAKE = make --no-print-directory
CC = cc
FC = f77
CLINKER = cc
FLINKER = f77
RANLIB = true
AR = ar ruv
RM = /bin/rm
MDEP_LIBS =  -lsocket -lnsl
MDEP_CFLAGS = 
MDEP_FFLAGS = 
MDEP_LFLAGS = 
P4_OBJ = $(P4_COMM_OBJ) $(P4_SOCK_OBJ) $(P4_SHMEM_OBJ)

CFLAGS = -fast -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)

FFLAGS = -fast -I$(FORTLIBDIR) $(MDEP_FFLAGS)
P4_HOME_DIR = ..
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib

LIBS = $(LIBDIR)/libp4.a $(MDEP_LIBS)

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $*.c 

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core

default:$(P)

sockperf:$(P) sockperf.o
	$(CLINKER) $(CFLAGS) -o sockperf sockperf.o

mutual:$(P) mutual.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o mutual mutual.o $(LIBDIR)/libp4.a

dirtest:$(P) dirtest.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o dirtest dirtest.o $(LIBDIR)/libp4.a

bug:$(P) bug.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o bug bug.o $(LIBDIR)/libp4.a

buftest:$(P) slave_main.o sr_slave.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o buftest buftest.o buftest.o $(LIBS)

sr_testlog:$(P) slave_main.o sr_slave.o $(LIBDIR)/libp4.a
	$(FLINKER) $(FFLAGS) -o sr_testlog.o $(LIBDIR)/libp4_f.a $(LIBDIR)/libp4.a

argtest:$(P) slave_main.o sr_slave.o $(LIBDIR)/libp4.a
	$(FLINKER) $(FFLAGS) -o argtest.o $(LIBDIR)/libp4_f.a $(LIBDIR)/libp4.a

realclean: clean
	$(RM) -f sockperf mutual dirtest bug buftest sr_testlog argtest \
	Makefile
