/******************** clog2slog.c ****************************/
/*
  This program converts a clog file generated using MPE Logging calls into an 
  slog file.
*/

/*
  a clog file format:
  divided into chunks of 1024 bytes containing a CLOG block.
  the block contains several records of different types.
  a record consists of a header which contains the timestamp, record type and
  process id.
  the headers are the same for all record types but the records themselves are
  very much different. this converter only pays attention to the following 
  record types: 
  CLOG_STATEDEF,
  CLOG_RAWEVENT,
  CLOG_COMMEVENT.
  the rest are ignored. for more information on other record types, please look
  at clog.h in the MPE source.
*/


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "clog2slog.h"

main (int argc, char **argv) {

  long frame_size = C2S_FRAME_BYTE_SIZE,   /* default frame kilo-byte size.*/
       fixed_record_size = 0,              /* size of a single slog fixed  */
                                           /* interval record.             */
       num_frames = 0,    /* stores the no of frames per directory in slog.*/
       max_frame_sz = 4096, 
       kilo_byte = 1024,
       cal1,cal2; 

  int clogfd,             /* file descriptor for reading clog files.       */
      err_chk,            /* error check flag.                             */ 
      num_args = 0;       /* number of options specified on command line.  */

  extern char *optarg;
  extern int optind, opterr, optopt;

  double membuff[CLOG_BLOCK_SIZE/sizeof(double)];
                             /* buffer to read clog records into.*/

  char *clog_file     = NULL,/* name of clog file.*/
       *slog_file     = NULL,/* name of slog file.*/
       *optstring     = "d:f:h",
        optchar;
  
  optchar = getopt(argc,argv,optstring);
  while(strchr(optstring,optchar) != NULL) {
    if(num_args <= 2)
      switch(optchar) {
      case 'd':
	if((optarg != NULL) && (*optarg == '=')) {
	  cal1 = atol(optarg+1);
	  num_frames = cal1;
	}
	num_args++;
	break;
      case 'f':
	if((optarg != NULL) && (*optarg == '=')) {
	  cal1 = atol(optarg+1);
	  frame_size = cal1;
	}
	num_args++;
	break;
      default:
	printHelp();
	exit(0);
      }
    else {
      printHelp();
      exit(0);
    }
    optchar = getopt(argc,argv,optstring);
    
  }
  
  /*  checkForBigEndian();  */
  
  
  if((argc-num_args) > 1)
    clog_file = argv[optind];
  else {
    fprintf(stderr, "clog2slog.c:%d: No clog file specified in command line.\n",
	    __LINE__);
    printHelp();
    exit(1);
  }
 
  if(strstr(clog_file,".clog") == NULL) {
    fprintf(stderr, "clog2slog.c:%d: specified file is not a clog file.\n",
	    __LINE__);
    printHelp();
    exit(1);
  }
  
  if((clogfd = open(clog_file, O_RDONLY, 0)) == -1) {
    fprintf(stderr,"clog2slog.c:%d: Could not open clog file %s for"
	    " reading.\n",__LINE__,clog_file);
    exit(1);
  }

  if((err_chk = init_clog2slog(clog_file, &slog_file)) == C2S_ERROR) 
    exit(1);

  /*
    first pass to initialize the state definitions list using the 
    init_state_defs() function.
  */
  while
    ((err_chk = read(clogfd, membuff, CLOG_BLOCK_SIZE)) != -1) {
    if(err_chk != CLOG_BLOCK_SIZE) {
      fprintf(stderr,"clog2slog.c:%d: Unable to read %d bytes.\n",
	      __LINE__,CLOG_BLOCK_SIZE);
      exit(1);
    }
    if((err_chk = init_state_defs(membuff)) == CLOG_ENDLOG)
      break;
    else if(err_chk == C2S_ERROR) {
      freeStateInfo();
      exit(1);
    }
  }

  /* initialize slog tables */
  if((err_chk = init_SLOG(num_frames,frame_size,slog_file)) == C2S_ERROR) 
    exit(1);
  
  /*
    going back to the beginning of clog file for second pass
  */
  if(lseek(clogfd,(long)0,0) == -1){
    fprintf(stderr,"clog2slog.c:%d: Could not go to top of file\n",__LINE__);
    exit(1);
  }
  /*
    making second pass of clog file to log slog intervals using clog events.
    the function used here is makeSLOG()
  */
  while((err_chk = read(clogfd, membuff, CLOG_BLOCK_SIZE)) != -1) {
    if(err_chk != CLOG_BLOCK_SIZE) {
      fprintf(stderr,"clog2slog.c:%d: Unable to read %d bytes.\n",
	      __LINE__,CLOG_BLOCK_SIZE);
      exit(1);
    }

    if((err_chk = makeSLOG(membuff)) == CLOG_ENDLOG)
      break;
    else if(err_chk == C2S_ERROR)
      exit(1);
  }
  
 
  close(clogfd);
  free_resources();
}

