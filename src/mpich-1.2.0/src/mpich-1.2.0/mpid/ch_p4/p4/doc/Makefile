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

CFLAGS = -g -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)
#CFLAGS = -O -I$(INCLUDEDIR) $(MDEP_CFLAGS) $(USER_CFLAGS)

FFLAGS = -g -I$(FORTLIBDIR) $(MDEP_FFLAGS)
P4_HOME_DIR = ..
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib
LIBS = $(MDEP_LIBS)
LIBLINK = ../bin

.SUFFIXES:	.o .c .f .h

.c.o:$(P)
	$(CC) $(CFLAGS) -c $*.c 

.f.o:$(P)
	$(FC) $(FFLAGS) -c $*.f 

dummy:	default

clean:
	$(RM) -f *~ *.o *.bak tags TAGS core

default:	

all:	p4-manual

p4-manual:
	latex p4
	bibtex p4
	latex p4
	texindex p4.fn p4.cp
	latex p4
	dvips p4

p4.html:
	/home/gropp/bin/sun4/tohtml -cvtlatex -cvttables p4.tex
	/home/gropp/bin/sun4/tohtml -cvtlatex -cvttables -useimg p4.tex
	@echo "p4.html now contains whole manual in one Mosiac page"
	@echo "It refers to the files *.xbm, which have been created"

p4.html.split:
	/home/gropp/bin/sun4/tohtml -split 2 -cvtlatex -cvttables p4.tex
	/home/gropp/bin/sun4/tohtml -split 2 -cvtlatex -cvttables -useimg p4.tex
	@echo "The p4 subdirectory now contains a multiple page Mosaic version"

realclean:	clean
	$(RM) -f Makefile *.bak *.dvi p4.ps *.log *.aux *~ *.bbl *.blg \
	  p4.toc p4.hux p4.cp* p4.fn* latex.err
	$(RM) -rf p4

