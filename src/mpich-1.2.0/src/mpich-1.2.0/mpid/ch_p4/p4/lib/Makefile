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
srcdir      = /home/TDDB78/src/mpich-1.2.0/mpid/ch_p4/p4/lib
libbuild_dir = /home/TDDB78/src/mpich-1.2.0/lib

VPATH=.:$(srcdir)


# RSHCOMMAND is used ONLY is HAS_RSHCOMMAND is also defined.
CFLAGS = -fast -I../../../../include $(MDEP_CFLAGS) $(USER_CFLAGS)  -DHAVE_LIBSOCKET=1 -DHAVE_LIBNSL=1 -DHAVE_RPC_RPC_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_TERMIOS_H=1 -DBOTH_STRING_INCS=1 -DUSE_STDARG=1 -DHAVE_SIGACTION=1 -DSEMUN_UNDEFINED=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DNONBLOCKING_READ_WORKS=1 -DNO_ECHO=1 -DHAS_RSHCOMMAND=1 -DHAVE_XDRMEM_CREATE=1    -DHAVE_LIBSOCKET=1 -DHAVE_LIBNSL=1 -DHAVE_RPC_RPC_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_TERMIOS_H=1 -DBOTH_STRING_INCS=1 -DUSE_STDARG=1 -DHAVE_SIGACTION=1 -DSEMUN_UNDEFINED=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DNONBLOCKING_READ_WORKS=1 -DNO_ECHO=1 -DHAS_RSHCOMMAND=1 -DHAVE_XDRMEM_CREATE=1 -DRSHCOMMAND='"ssh"' -I${top_srcdir}/include -I.. -I../include

FFLAGS = -fast -I$(FORTLIBDIR) $(MDEP_FFLAGS)

P4_HOME_DIR = ${top_srcdir}
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib

P4_HDRS = p4.h p4_sys.h p4_defs.h p4_macros.h  \
          p4_sock_util.h p4_MD.h p4_funcs.h p4_sys_funcs.h \
	  p4_mon.h p4_sr.h p4_globals.h ../p4_config.h

# add p4_fc.o to next line for fiber channel
P4_COMM_OBJ	= 	p4_globals.o p4_MD.o p4_error.o \
			p4_args.o p4_alloc.o p4_debug.o \
			p4_procgroup.o p4_tsr.o \
			p4_mon.o p4_broadcast.o p4_utils.o p4_bm.o
# add for fiber channel p4_fc.o

P4_SOCK_OBJ	=	p4_rm.o      p4_sock_util.o \
			p4_sock_cr.o p4_sock_conn.o \
			p4_sock_sr.o p4_sock_list.o \
			p4_secure.o

P4_CUBENODE_OBJ	=	$(P4_COMM_OBJ) p4_node_rm.o

P4_CUBEHOST_OBJ	=	$(P4_COMM_OBJ) $(P4_SOCK_OBJ)

P4_SHMEM_OBJ	=	p4_shmem_sr.o

LIBS = $(LIBDIR)/libp4.a $(MDEP_LIBS) 

.SUFFIXES:
.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $<

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core ${srcdir}/*.o

$(P4_OBJ): $(P4_HDRS)

listener_p4.o: $(P4_HDRS)

default:$(P) 
	$(MAKE) $(MFLAGS) libp4.a

all:$(P) 
	$(MAKE) $(MFLAGS) libp4.a listener_p4

libp4.a:$(P) $(P4_OBJ)
	$(AR) $@ $?
	$(RANLIB) $@

p4inmpi:$(P) $(P4_OBJ)
	$(AR) ${libbuild_dir}/lib$(MPILIBNAME).a $?
	$(RANLIB) ${libbuild_dir}/lib$(MPILIBNAME).a

p4mdeplibs:
	@echo $(MDEP_LIBS)

p4mdepcflags:
	@echo $(MDEP_CFLAGS)

p4mdepfflags:
	@echo $(MDEP_FFLAGS)

listener_p4:$(P) listener_p4.o $(P4_OBJ)
	$(CC) $(CFLAGS) -o listener_p4 listener_p4.o $(LIBS)
	/bin/mv listener_p4 ../bin

p4_lock.o: p4_lock.s
	as -P -DSUN_SOLARIS p4_lock.s

realclean: clean
	$(RM) -f Makefile libp4.a alog*.h usc*.h

#p4_funcs.h:
#	-mv p4_funcs.h p4_funcs.h.bak
#	csh make_funcdecls p4*.c > p4_funcs.h

#lintlib llib-lp4.ln:	lint_p4.c
#	@if [ $(UNIX) != P4BSD ] ;\
#	then\
#		echo lint $(LINTFLAGS) -x -op4 lint_p4.c ;\
#		lint $(LINTFLAGS) -x -op4 lint_p4.c ;\
#	else\
#		echo lint $(LINTFLAGS) -Cp4 lint_p4.c ;\
#		lint $(LINTFLAGS) -Cp4 lint_p4.c ;\
#	fi

