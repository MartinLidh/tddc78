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

LIB = $(LIBDIR)/libp4.a
FORTLIBDIR = $(P4_HOME_DIR)/lib_f
FORTLIB = $(FORTLIBDIR)/libp4_f.a

LIBS =  $(FORTLIB) $(LIB) $(MDEP_LIBS)

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $*.c 

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core

default:$(P)
	$(MAKE) $(MFLAGS) sr_test simpglob

all:$(P)
	$(MAKE) $(MFLAGS) sr_test sr_master sr1_master
	$(MAKE) $(MFLAGS) sr_slave sr1_slave

sr_test: sr_test.o $(FORTLIB) $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr_test sr_test.o $(LIBS)
	
sr_log: sr_log.o $(FORTLIB) $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr_log sr_log.o $(LIBS)
	
simpglob: simpglob.o $(FORTLIB) $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o simpglob simpglob.o $(LIBS)
	
sr_master: sr_master.o sr_slave.o  $(FORTLIB) $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr_master sr_master.o sr_slave.o $(LIBS)
	
sr_slave: sr_slave.o  slave_main.o $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr_slave slave_main.o sr_slave.o $(LIBS)

sr1_master: sr1_master.o sr1_slave.o $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr1_master sr1_master.o sr1_slave.o $(LIBS)
	
sr1_slave: sr1_slave.o  slave_main.o $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o sr1_slave slave_main.o sr1_slave.o $(LIBS)

realclean: clean
	$(RM) -f sr_test simpglob sr_master sr_slave alogfile.p* \
		sr1_master sr1_slave sr_log Makefile p4f.h
