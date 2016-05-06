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

LIB = $(LIBDIR)/libp4.a
FORTLIBDIR = $(P4_HOME_DIR)/lib_f
FORTLIB = $(FORTLIBDIR)/libp4_f.a

MAIN = fslave_main.o

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

all:$(P)
	$(MAKE) $(MFLAGS) norm_master norm_slave
	$(MAKE) $(MFLAGS) comm_master comm_slave
	$(MAKE) $(MFLAGS) pi

pi: pi.o $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o pi pi.o $(LIBS)
	
norm_master: norm_master.o norm_slave.o $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o norm_master norm_master.o norm_slave.o $(LIBS)
	
norm_slave: norm_slave.o  $(MAIN) $(FORTLIB)  $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o norm_slave $(MAIN) norm_slave.o $(LIBS)

comm_master: comm_master.o comm_slave.o $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o comm_master comm_master.o comm_slave.o $(LIBS)
	
comm_slave: comm_slave.o  $(MAIN) $(LIB)
	$(FLINKER) $(MDEP_FFLAGS) -o comm_slave $(MAIN) comm_slave.o $(LIBS)

realclean: clean
	$(RM) -f norm_master norm_slave comm_master comm_slave pi \
	Makefile p4f.h alogfile.p*
