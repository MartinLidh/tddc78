# Generated automatically from Makefile.in by configure.
#  (C) 1993 by Argonne National Laboratory and Mississipi State University.
#      All rights reserved.  See COPYRIGHT in top-level directory.
#

##### User configurable options #####

CC          = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
CLINKER     = $(CC)
MAKE        = make --no-print-directory
AR          = ar -r
RANLIB      = ranlib
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR)
OPTFLAGS    = 
LIB_PATH    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11 
LOG_LIB     = -L/home/TDDB78/src/mpich-1.2.0/lib -llmpi -lmpe -lX11
PROF_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11

### End User configurable options ###

CFLAGS =  -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(OPTFLAGS) $(INCLUDE_DIR) 

default: ../wrappergen/wrappergen log_wrappers.c time_wrappers.c visual_mess.c

../wrappergen/wrappergen:
	(cd ../wrappergen; $(MAKE) wrappergen; )

log_wrappers.c: log_wrappers.w
	../wrappergen/wrappergen -w log_wrappers.w -o log_wrappers.c 

time_wrappers.c: time_wrappers.w
	../wrappergen/wrappergen -w time_wrappers.w -o time_wrappers.c 

visual_mess.c: visual_mess.w
	../wrappergen/wrappergen -w visual_mess.w -w message_prof.w \
	-o visual_mess.c 

.c.o:
	$(CC) $(CFLAGS) -c $*.c

clean:
	/bin/rm -rf *.o *.c *~

