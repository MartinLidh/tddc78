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
MDEP_LIBS = -lcrypt  -lsocket -lnsl 
MDEP_CFLAGS = 
MDEP_FFLAGS = 
MDEP_LFLAGS = 
P4_OBJ = $(P4_COMM_OBJ) $(P4_SOCK_OBJ) $(P4_SHMEM_OBJ)
SHELL = /bin/sh
top_srcdir  = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4/p4
srcdir      = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4/p4/servers
libbuild_dir = /home/TDDB78/src/mpich-1.2.0/lib

VPATH=.:$(srcdir)


CFLAGS = -fast -I$(INCLUDEDIR) -I../../../../include $(MDEP_CFLAGS) $(USER_CFLAGS)  -DHAVE_LIBSOCKET=1 -DHAVE_LIBNSL=1 -DHAVE_RPC_RPC_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_TERMIOS_H=1 -DBOTH_STRING_INCS=1 -DUSE_STDARG=1 -DHAVE_SIGACTION=1 -DSEMUN_UNDEFINED=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DNONBLOCKING_READ_WORKS=1 -DNO_ECHO=1 -DHAS_RSHCOMMAND=1 -DHAVE_XDRMEM_CREATE=1    -DHAVE_LIBSOCKET=1 -DHAVE_LIBNSL=1 -DHAVE_RPC_RPC_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_TERMIOS_H=1 -DBOTH_STRING_INCS=1 -DUSE_STDARG=1 -DHAVE_SIGACTION=1 -DSEMUN_UNDEFINED=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DNONBLOCKING_READ_WORKS=1 -DNO_ECHO=1 -DHAS_RSHCOMMAND=1 -DHAVE_XDRMEM_CREATE=1 -I${top_srcdir}/include -I..

FFLAGS = -fast -I$(FORTLIBDIR) $(MDEP_FFLAGS)
SERVER_HOME = /etc

P4_PORT = 753
P4_PORT_NAME = serv_p4

CHMOD = chmod
CHOWN = chown
INSTALL = /bin/cp -i
SED   = sed

P4_HOME_DIR = ${top_srcdir}
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = @MPIR_HOME@/lib/@ARCH@/@COMM@

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $<

.f.o:$(P)
	$(FC) $(FFLAGS) -c $<

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core ${srcdir}/*.o

default:
	@echo not making server on $(P4ARCH)

all: serv_p4 example_configs iserv_p4 killiserv_p4

example_configs: inetd.entry services.entry

serv_p4: serv_p4.o
	$(CLINKER) $(CFLAGS) -o serv_p4 serv_p4.o $(MDEP_LIBS)
	/bin/mv serv_p4 ../bin

iserv_p4:$(P)  iserv_p4.o
	$(CLINKER) $(CFLAGS) -o iserv_p4 iserv_p4.o $(LIBDIR)/libmpi.a
	/bin/mv iserv_p4 ../bin

killiserv_p4:$(P)  killiserv_p4.o
	$(CLINKER) $(CFLAGS) -o killiserv_p4 killiserv_p4.o $(LIBDIR)/libmpi.a
	/bin/mv killiserv_p4 ../bin

inetd.entry: inetd.entry.base
	$(SED) -e 's,P4_PORT_NAME,$(P4_PORT_NAME),g' \
	    -e 's,P4_PORT,$(P4_PORT),g' \
	    -e 's,SERVER_HOME,$(SERVER_HOME),g' \
	    inetd.entry.base > inetd.entry

services.entry: services.entry.base
	$(SED) -e 's,P4_PORT_NAME,$(P4_PORT_NAME),g' \
	    -e 's,P4_PORT,$(P4_PORT),g' \
	    -e 's,SERVER_HOME,$(SERVER_HOME),g' \
	    services.entry.base > services.entry

install: serv_p4
	$(INSTALL) ../bin/serv_p4 $(SERVER_HOME)
	$(CHOWN) root $(SERVER_HOME)/serv_p4
	$(CHMOD) 700 $(SERVER_HOME)/serv_p4

realclean:	clean
	$(RM)  -f *~ \#* inetd.entry services.entry \
		     serv_p4 Makefile P4Server.Log.* \
		     iserv_p4 killiserv_p4
