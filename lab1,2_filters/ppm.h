#include <stdio.h>

void ppm_error(char *str);

/* Read a character from a ppm-file. Remove all comments */
char ppm_readchar(FILE *file);

/* Read the magic number in a ppm-file */
int ppm_readmagicnumber(FILE *file);
 
/* Read an ASCII integer from a ppm-file */
int ppm_readint(FILE *file);
