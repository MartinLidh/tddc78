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

CFLAGS = -g -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)
#CFLAGS = -O -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)

FFLAGS = -g -I$(FORTLIBDIR) $(MDEP_FFLAGS)
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

default:

all: bcast typedbcast sendtyped sendtyped2
	$(MAKE) $(MFLAGS) lm_master lm1_master
	$(MAKE) $(MFLAGS) simpglob
	$(MAKE) $(MFLAGS) lm_slave lm1_slave

simpglob:$(P) simpglob.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o simpglob simpglob.o  $(LIBS)

bcast:$(P) bcast.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o bcast bcast.o  $(LIBS)

timertest:$(P) timertest.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o timertest timertest.o  $(LIBS)

typedbcast:$(P) typedbcast.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o typedbcast typedbcast.o  $(LIBS)

sendtyped:$(P) sendtyped.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o sendtyped sendtyped.o  $(LIBS)

sendtyped2:$(P) sendtyped2.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o sendtyped2 sendtyped2.o  $(LIBS)

lm_master:$(P) lm_master.o lm_slave.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o lm_master lm_master.o lm_slave.o $(LIBS)

lm_slave:$(P) lm_slave.o slave_main.o $(LIBDIR)/libp4.a 
	$(CLINKER) $(CFLAGS) -o lm_slave slave_main.o lm_slave.o $(LIBS)

lm1_master:$(P) lm1_master.o lm1_slave.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o lm1_master lm1_master.o lm1_slave.o $(LIBS)

lm1_slave:$(P) lm1_slave.o slave_main.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o lm1_slave slave_main.o lm1_slave.o $(LIBS)

realclean: clean
	$(RM) -f lm_master lm_slave lm1_master lm1_slave \
	simpglob bcast Makefile alogfile.p*

