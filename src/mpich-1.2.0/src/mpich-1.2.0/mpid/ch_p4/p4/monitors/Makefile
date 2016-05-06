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
	@if [ $(P4ARCH) = IPSC860 -o $(P4ARCH) = DELTA -o $(P4ARCH) = IPSC860_SOCKETS -o $(P4ARCH) = PARAGON ] ;\
	then\
		echo NOT making any monitors pgms on $(P4ARCH) ;\
	else\
		$(MAKE) $(MFLAGS) addtwo.ask addtwo.gs grid.shmem ;\
	fi

all:$(P)
	$(MAKE) $(MFLAGS) addtwo.ask addtwo.gs adquad grid.shmem gridlog.shmem

grid.shmem:$(P)	grid.shmem.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o grid.shmem grid.shmem.o $(LIBS)

gridlog.shmem:$(P)	gridlog.shmem.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o gridlog.shmem gridlog.shmem.o $(LIBS)

addtwo.ask:$(P)	addtwo.ask.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o addtwo.ask addtwo.ask.o $(LIBS) -lm

addtwo.gs:$(P)	addtwo.gs.o  $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o addtwo.gs addtwo.gs.o $(LIBS) -lm

adquad:$(P)	adquad.o $(LIBDIR)/libp4.a
	$(CLINKER) $(CFLAGS) -o adquad adquad.o $(LIBS) -lm

realclean: clean
	$(RM) -f addtwo.ask addtwo.gs adquad grid.shmem gridlog.shmem \
		Makefile alogfile.p*
