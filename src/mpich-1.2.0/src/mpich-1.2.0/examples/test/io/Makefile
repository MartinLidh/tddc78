# Generated automatically from Makefile.in by configure.
CC = /home/TDDB78/src/mpich-1.2.0/bin/mpicc
F77 = /home/TDDB78/src/mpich-1.2.0/bin/mpif77
INCLUDE_DIR = 
LIBS =  
USER_CFLAGS =  $(INCLUDE_DIR)
USER_FFLAGS =  $(INCLUDE_DIR)
srcdir=/home/TDDB78/src/mpich-1.2.0/romio/test

VPATH=.:$(srcdir)

all: c-test fperf fcoll_test fmisc pfcoll_test

c-test: simple perf async coll_test coll_perf misc file_info excl large_array \
     atomicity noncontig i_noncontig noncontig_coll split_coll shared_fp \
     large_file psimple

default: all

.c.o:
	$(CC) $(USER_CFLAGS) -c $<

.f.o:
	$(F77) $(USER_FFLAGS) -c $<

clean: 
	rm -f .P* PI* *.o simple async coll_test coll_perf misc perf fcoll_test fperf fmisc psimple pfcoll_test test* job* file_info excl large_array large_file noncontig i_noncontig noncontig_coll atomicity split_coll shared_fp *.out

simple: simple.o
	$(CC) $(USER_CFLAGS) -o simple simple.o $(LIBS)

psimple: psimple.o
	$(CC) $(USER_CFLAGS) -o psimple psimple.o $(LIBS)

perf: perf.o
	$(CC) $(USER_CFLAGS) -o perf perf.o $(LIBS)

excl: excl.o
	$(CC) $(USER_CFLAGS) -o excl excl.o $(LIBS)

async: async.o
	$(CC) $(USER_CFLAGS) -o async async.o $(LIBS)

atomicity: atomicity.o
	$(CC) $(USER_CFLAGS) -o atomicity atomicity.o $(LIBS)

coll_test: coll_test.o
	$(CC) $(USER_CFLAGS) -o coll_test coll_test.o $(LIBS)

coll_perf: coll_perf.o
	$(CC) $(USER_CFLAGS) -o coll_perf coll_perf.o $(LIBS)

misc: misc.o
	$(CC) $(USER_CFLAGS) -o misc misc.o $(LIBS)

noncontig: noncontig.o
	$(CC) $(USER_CFLAGS) -o noncontig noncontig.o $(LIBS)

i_noncontig: i_noncontig.o
	$(CC) $(USER_CFLAGS) -o i_noncontig i_noncontig.o $(LIBS)

noncontig_coll: noncontig_coll.o
	$(CC) $(USER_CFLAGS) -o noncontig_coll noncontig_coll.o $(LIBS)

file_info: file_info.o
	$(CC) $(USER_CFLAGS) -o file_info file_info.o $(LIBS)

shared_fp: shared_fp.o
	$(CC) $(USER_FFLAGS) -o shared_fp shared_fp.o $(LIBS)

split_coll: split_coll.o
	$(CC) $(USER_FFLAGS) -o split_coll split_coll.o $(LIBS)

fperf: fperf.o
	$(F77) $(USER_FFLAGS) -o fperf fperf.o $(LIBS)

fcoll_test: fcoll_test.o
	$(F77) $(USER_FFLAGS) -o fcoll_test fcoll_test.o $(LIBS)

pfcoll_test: pfcoll_test.o
	$(F77) $(USER_FFLAGS) -o pfcoll_test pfcoll_test.o $(LIBS)

fmisc: fmisc.o
	$(F77) $(USER_FFLAGS) -o fmisc fmisc.o $(LIBS)

finfo: finfo.o
	$(F77) $(USER_FFLAGS) -o finfo finfo.o $(LIBS)

info: info.o
	$(CC) $(USER_CFLAGS) -o info info.o $(LIBS)

large_file: large_file.o
	$(CC) $(USER_CFLAGS) -o large_file large_file.o $(LIBS)

large_array: large_array.o
	$(CC) $(USER_CFLAGS) -o large_array large_array.o $(LIBS)

testing: 
	-./runtests $(TESTARGS)
