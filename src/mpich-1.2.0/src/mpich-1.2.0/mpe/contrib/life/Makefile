# Generated automatically from Makefile.in by configure.

##### User configurable options #####

CC          = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
CLINKER     = $(CC)
OPTFLAGS    =  -g
LIB_PATH    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11 
MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
LOG_LIB     = -L/home/TDDB78/src/mpich-1.2.0/lib -llmpi -lmpe -lX11
PROF_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR)
LDFLAGS     = 

### End User configurable options ###

CFLAGS  =   -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(OPTFLAGS) $(INCLUDE_DIR) 
EXECS =  life_g

default: $(EXECS)

life_g:	life_g.o $(MPE_DIR)/mpe.h
	$(CLINKER) $(LDFLAGS) $(OPTFLAGS)  -o life_g life_g.o \
	$(LIB_PATH) -lm

clean:
	/bin/rm -f *.o *~ PI* $(EXECS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f


