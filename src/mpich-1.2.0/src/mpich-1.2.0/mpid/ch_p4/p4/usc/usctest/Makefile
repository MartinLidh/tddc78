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
P4_HOME_DIR = ../..
LIBDIR = $(P4_HOME_DIR)/lib
LIBS =  $(MDEP_LIBS)
INCLUDEDIR = /usr/include

USER_CFLAGS = -I$(INCLUDEDIR) -D$(P4ARCH)

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $*.c 

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core

default:

usctest: usctest.o
	$(CLINKER) $(CFLAGS) -o usctest usctest.o ../usc.o $(LIBS)

usctest2: usctest2.o
	$(CLINKER) $(CFLAGS) -o usctest2 usctest2.o ../usc.o $(LIBS)

capcell: capcell.o
	$(CLINKER) $(CFLAGS) -o capcell capcell.o ../usc.o $(LIBS)

realclean:	clean
	$(RM) -f *~ *.o usctest usctest2 capcell Makefile

