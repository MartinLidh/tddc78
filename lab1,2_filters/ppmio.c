/*
  File: ppmio.c

  Implementation of PPM image file IO functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppmio.h"

int read_ppm (const char * fname, 
	       int * xpix, int * ypix, int * max, char * data) {
  char ftype[40];
  char ctype[40] = "P6";
  char line[80];
  int errno;

  FILE * fp;
  errno = 0;

  if (fname == NULL) fname = "\0";
  fp = fopen (fname, "r");
  if (fp == NULL) {
    fprintf (stderr, "read_ppm failed to open %s: %s\n", fname,
	     strerror (errno));
    return 1;
  }
  
  fgets(line, 80, fp);
  sscanf(line, "%s", ftype);

  while (fgets(line, 80, fp) && (line[0] == '#'));

  sscanf(line, "%d%d", xpix, ypix);
  fscanf(fp, "%d\n", max);

  if(*xpix * *ypix > MAX_PIXELS) {
     fprintf (stderr, "Image size is too big\n");
    return 4;
 };

  if (strncmp(ftype, ctype, 2) == 0) {
    if (fread (data, sizeof (char), *xpix * *ypix * 3, fp) != 
	*xpix * *ypix * 3) {
      perror ("Read failed");
      return 2;
    }
  } else {
    fprintf (stderr, "Wrong file format: %s\n", ftype);
  }

  if (fclose (fp) == EOF) {
    perror ("Close failed");
    return 3;
  }
  return 0;

}


int write_ppm (const char * fname, int xpix, int ypix, char * data) {

  FILE * fp;
  int errno = 0;

  if (fname == NULL) fname = "\0";
  fp = fopen (fname, "w");
  if (fp == NULL) {
    fprintf (stderr, "write_ppm failed to open %s: %s\n", fname,
	     strerror (errno));
    return 1;
  }
  
  fprintf (fp, "P6\n");
  fprintf (fp, "%d %d 255\n", xpix, ypix);
  if (fwrite (data, sizeof (char), xpix*ypix*3, fp) != xpix*ypix*3) {
    perror ("Write failed");
    return 2;
  }
  if (fclose (fp) == EOF) {
    perror ("Close failed");
    return 3;
  }
  return 0;
}
