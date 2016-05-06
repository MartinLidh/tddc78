# Generated automatically from Makefile.in by configure.
ALL: default
##### User configurable options #####

CC          = /home/TDDB78/src/mpich-1.2.0/lib/../bin/mpicc
CLINKER     = $(CC)
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR)
OPTFLAGS    = 
MPE_GRAPH   = -DMPE_GRAPHICS
MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
LIB_PATH    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11  -lm
LOG_LIB     = -L/home/TDDB78/src/mpich-1.2.0/lib -llmpi -lmpe -lX11 -lm
PROF_LIB    = -L/home/TDDB78/src/mpich-1.2.0/lib -lmpe -lX11 -lm
LDFLAGS     = 

### End User configurable options ###

CFLAGS  =  -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(OPTFLAGS) $(INCLUDE_DIR) 
EXECS = pmandel pmandel_log

default: $(EXECS)

all: default

fract_gen.o: fract_gen.h pmandel.h

pm_genproc.o: pmandel.h fract_gen.h

pmandel.o: pmandel.h fract_gen.h lists.h pm_genproc.h

pm_separate_rect.o: pmandel.h pm_genproc.h

pmandel: pmandel.o pm_genproc.o fract_gen.o pm_separate_rect.o lists.o args.o
	$(CLINKER) $(LDFLAGS) -o pmandel pmandel.o pm_genproc.o \
	  fract_gen.o pm_separate_rect.o lists.o args.o \
	  $(PROF_LIB) 

pmandel_log: pmandel.o pm_genproc.o fract_gen.o pm_separate_rect.o lists.o args.o
	$(CLINKER) $(LDFLAGS) -o pmandel_log pmandel.o pm_genproc.o \
	  fract_gen.o pm_separate_rect.o lists.o args.o \
	  $(PROF_LIB) -mpilog

clean:
	/bin/rm -f *.o *~ PI* $(EXECS) *.log *.out

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f

lint:
	lint -chv  -DMPI_solaris -DFORTRANUNDERSCORE -DUSE_STDARG -DHAVE_PROTOTYPES $(MPE_GRAPH) \
		$(INCLUDE_DIR) pmandel.c \
		pm_genproc.c fract_gen.c pm_separate_rect.c lists.c args.c
