#include "ppm.h"

void ppm_error(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

/* Read a character from a ppm-file. Remove all comments */
char ppm_readchar(FILE *file)
{
    char ch;
    ch = getc(file);
    if (ch==EOF)
	ppm_error("read error");
    if (ch=='#') do {
	ch = getc(file);
	if (ch==EOF)
	    ppm_error("read error");
    } while (ch != '\n');
    
    return ch;
}

/* Read the magic number in a ppm-file */
int ppm_readmagicnumber(FILE *file)
{
    int ch1, ch2;
    
    ch1 = getc(file);
    if (ch1==EOF)
	ppm_error("read error");
    ch2 = getc(file);
    if (ch2==EOF)
	ppm_error("read error");
    return ch1 * 256 + ch2;
}
 
/* Read an ASCII integer from a ppm-file */
int ppm_readint(FILE *file)
{
    char ch;
    int i;

    do 
	ch = ppm_readchar(file);
    while (ch == ' ' || ch == '\t' || ch == '\n');

    if (ch < '0' || ch > '9')
	ppm_error("error in readint");
    i = 0;
    do {
	i = i*10 + (ch - '0');
	ch = ppm_readchar(file);
    } while (ch >= '0' && ch <= '9');
    return i;
}
