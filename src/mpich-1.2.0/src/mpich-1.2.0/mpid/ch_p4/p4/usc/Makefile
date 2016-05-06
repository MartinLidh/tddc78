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

SHELL = /bin/sh
top_srcdir  = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4/p4
srcdir      = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4/p4/usc
libbuild_dir = /home/TDDB78/src/mpich-1.2.0/lib

VPATH=.:$(srcdir)

CFLAGS = -fast -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)    -DHAVE_LIBSOCKET=1 -DHAVE_LIBNSL=1 -DHAVE_RPC_RPC_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_TERMIOS_H=1 -DBOTH_STRING_INCS=1 -DUSE_STDARG=1 -DHAVE_SIGACTION=1 -DSEMUN_UNDEFINED=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DNONBLOCKING_READ_WORKS=1 -DNO_ECHO=1 -DHAS_RSHCOMMAND=1 -DHAVE_XDRMEM_CREATE=1 \
	-I${srcdir} -I../include

FFLAGS = -fast -I$(FORTLIBDIR) $(MDEP_FFLAGS)
P4_HOME_DIR = ${top_srcdir}
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib
LIBS = $(MDEP_LIBS)

USER_CFLAGS =  -D$(P4ARCH)

USC_HDR = usc_sys.h usc.h
USC_SRC = usc.c
USC_OBJ = usc.o

.SUFFIXES:
.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $<

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core ${srcdir}/*.o

default:	
	$(MAKE) $(LIBDIR)/libp4.a

all:	
	$(MAKE) $(LIBDIR)/libp4.a

usc.o:$(P)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c ${srcdir}/usc.c 

$(USC_OBJ):  $(USC_SRC) $(USC_HDR)


$(LIBDIR)/libp4.a:  $(USC_OBJ)
	$(AR) $(LIBDIR)/libp4.a $?
	$(RANLIB) $(LIBDIR)/libp4.a
#	$(AR) $@ $?
#	$(RANLIB) $@

p4inmpi:$(P) $(USC_OBJ)
	$(AR) ${libbuild_dir}/lib$(MPILIBNAME).a $?
	$(RANLIB) ${libbuild_dir}/lib$(MPILIBNAME).a

realclean:	clean
	$(RM) -f $(LIBDIR)/libp4.a Makefile
	@cd usctest; $(MAKE) clean

