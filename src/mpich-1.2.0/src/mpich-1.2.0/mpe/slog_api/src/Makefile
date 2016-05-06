# Generated automatically from Makefile.in by configure.
#  (C) 1993 by Argonne National Laboratory and Mississipi State University.
#      All rights reserved.  See COPYRIGHT in top-level directory.
#

##### User configurable options #####

SLOGNAME     = slog
SLOG_HOME    = /home/TDDB78/src/mpich-1.2.0/mpe/slog_api
CC           = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
CLINKER      = $(CC)
AR           = ar
ARFLAGS      = crl
RANLIB       = ranlib
MAKE         = make --no-print-directory
RM           = /bin/rm
LN_S         = ln -s
OPTFLAGS     = 
SLOG_CFLAGS  = -DNOWHERE -DCHECKTIMEORDER -DCOMPRESSION   -DMPI_solaris -DHAVE_SLOGCONF_H
DEBUG_LIBS   = 
MPICH        = 

### End User configurable options ###
SHELL = /bin/sh

VPATH=.:$(srcdir)
# Installation directories
exec_prefix      = ${prefix}
prefix           = /home/TDDB78/mpich-1.2.0/
bindir           = ${exec_prefix}/bin
includedir       = ${prefix}/include
libdir           = ${exec_prefix}/lib
# Build directories
libbuild_dir     = /home/TDDB78/src/mpich-1.2.0/lib
binbuild_dir     = /home/TDDB78/src/mpich-1.2.0/mpe/slog_api/bin
includebuild_dir = /home/TDDB78/src/mpich-1.2.0/mpe/slog_api/include
top_srcdir       = /home/TDDB78/src/mpich-1.2.0/mpe/slog_api
srcdir           = /home/TDDB78/src/mpich-1.2.0/mpe/slog_api/src
INC_DIR          = -I. -I$(SLOG_HOME) -I${includebuild_dir} -I..
LIB_DIR          = ${libbuild_dir}
BIN_DIR          = ${binbuild_dir}
TEST_DIR         = ../test



CFLAGS =  $(SLOG_CFLAGS) $(OPTFLAGS) 

MAIN_SRC = ts_incrEQendtime_resfnEQyes.c ts_incrEQendtime_resfnEQno.c \
           ts_incrEQstarttime.c \
           slog_printframes.c slog_printbbufs.c slog_readtest.c \
           slog_recdefs_test.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)
MAIN_EXE = $(MAIN_OBJ:.o=)

LIB_SRC  = bswp_fileio.c fbuf.c str_util.c slog_fileio.c \
           slog_header.c slog_preview.c slog_pstat.c slog_profile.c \
           slog_ttable.c slog_recdefs.c slog_tasklabel.c slog_impl.c \
           slog_bbuf.c slog_assoc.c slog_vtrarg.c slog_bebits.c \
           slog_irec_common.c slog_irec_write.c slog_irec_read.c
LIB_OBJ  = $(LIB_SRC:.c=.o)


.SUFFIXES:	.c .o .a

.c.o:
	$(CC) $(CFLAGS) -c $< $(INC_DIR)

.o:	
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB_DIR) -l$(SLOGNAME) $(DEBUG_LIBS)


default: $(LIB_DIR)/lib$(SLOGNAME).a prog slog_destlib

$(LIB_DIR)/lib$(SLOGNAME).a: $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

prog: $(MAIN_OBJ) $(MAIN_EXE)
	mv -f $(MAIN_EXE) $(BIN_DIR)
	@for exefile in $(MAIN_EXE); do \
	    if [ ! -h $(TEST_DIR)/$$exefile -a -s $(BIN_DIR)/$$exefile ]; then \
	        $(LN_S) $(BIN_DIR)/$$exefile $(TEST_DIR) ; \
	    fi ; \
	done

slog_destlib: $(LIB_OBJ)
	@if test "$(SLOG_DESTLIB)" != ""; then \
	    $(AR) $(ARFLAGS) $(SLOG_DESTLIB) $? ; \
	fi

rmlib:  
	$(RM) -f $(LIB_DIR)/lib$(SLOGNAME).a

rmprog:
	if test -d $(BIN_DIR) ; then \
	    cd $(BIN_DIR); $(RM) -f $(MAIN_EXE) core ; fi
	if test -d $(TEST_DIR) ; then \
	    cd $(TEST_DIR); $(RM) -f slogfile.data* core ; fi

clean:	
	-$(RM) -f *.o *~ $(MAIN_EXE) PI* core
	-for dir in test_fbuf test_misc ; do \
	    if test -s $$dir/Makefile ; then \
	        ( cd $$dir ; $(MAKE) clean ) ; \
	    fi; \
	done

distclean:  clean rmprog rmlib
	    -$(RM) -f a.out *.dat	      

