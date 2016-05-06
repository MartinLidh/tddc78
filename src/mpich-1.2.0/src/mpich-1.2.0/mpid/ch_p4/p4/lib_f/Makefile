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

FORTLIBDIR = $(P4_HOME_DIR)/lib_f

P4_COMM_OBJ = p4_fargs.o p4_fort.o p4_fmaster.o

P4_CUBENODE_OBJ = $(P4_COMM_OBJ)

P4_CUBEHOST_OBJ = $(P4_COMM_OBJ) $(P4_SOCK_OBJ)

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $*.c 

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core

default:$(P) all

all:$(P) libp4_f.a 

libp4_f.a:$(P) $(P4_OBJ)
	$(AR) ../lib_f/$@ $?
	$(RANLIB) ../lib_f/$@

realclean: clean
	$(RM) -f Makefile libp4_f.a
