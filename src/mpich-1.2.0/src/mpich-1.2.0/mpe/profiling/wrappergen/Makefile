# Generated automatically from Makefile.in by configure.
#  (C) 1993 by Argonne National Laboratory and Mississipi State University.
#

MPE_DIR     = /home/TDDB78/src/mpich-1.2.0/mpe
OPTFLAGS    = 
CLINKER     = $(CC)
INCLUDE_DIR = -I/home/TDDB78/src/mpich-1.2.0/lib/../include -I/home/TDDB78/src/mpich-1.2.0/include -I$(MPE_DIR)

EXECS = wrappergen write_proto
MPI_PROTO = '-DPROTO_FILE="$(MPE_DIR)/profiling/wrappergen/mpi_proto"'

CFLAGS = $(OPTFLAGS) $(INCLUDE_DIR)
default: wrappergen

all: $(EXECS)

readproto.o: readproto.c
	$(CC) $(CFLAGS) $(MPI_PROTO) -c $*.c

wrappergen: readproto.o wrappergen.o args.o
	$(CLINKER) -o wrappergen $(CFLAGS) $(BASEOPT) readproto.o \
	   wrappergen.o args.o

write_proto: prof_wrapper.o driver.o doc.o write_proto.o system.o
	$(CLINKER) -o write_proto $(CFLAGS) $(BASEOPT) driver.o \
	   prof_wrapper.o doc.o write_proto.o system.o -lm $(SLIB)

.c.o:
	$(CC) $(CFLAGS) -c $*.c

clean:
	/bin/rm -rf $(EXECS) *.o *~
