#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"

/* Pixel Declaration */
#define PIXEL(image,x,y) ((image)+((y)*(xsize)+(x)))

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;


pixel *allocate_image(int size)
{
    pixel *image;
    image = (pixel *)malloc(sizeof(pixel)*size);
    if (!image) {
	fprintf(stderr, "malloc failed");
	exit(1);
    }
    return image;
}


int main(int argc, char **argv)
{
    FILE *infile, *outfile;
    int magic, ok;
    int radius;
    int xsize, ysize, colmax;
    pixel *image;
    int x,y;
    int avg, pval, psum, sum = 0;


    /* Take care of the arguments */

    if (argc != 3) {
	fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
	exit(2);
    }
    if (!(infile = fopen(argv[1], "r"))) {
	fprintf(stderr, "Error when opening %s\n", argv[1]);
	exit(1);
    } 
    if (!(outfile = fopen(argv[2], "w"))) {
	fprintf(stderr, "Error when opening %s\n", argv[2]);
	exit(1);
    }


    /* read file */

    magic = ppm_readmagicnumber(infile);
    if (magic != 'P'*256+'6') {
	fprintf(stderr, "Wrong magic number\n");
	exit(1);
    }
    xsize = ppm_readint(infile);
    ysize = ppm_readint(infile);
    colmax = ppm_readint(infile);
    if (colmax > 255) {
	fprintf(stderr, "Too large maximum color-component value\n");
	exit(1);
    }
    
    image = allocate_image(xsize*ysize);
    if (!fread(image, sizeof(pixel), xsize*ysize, infile)) {
	fprintf(stderr, "error in fread\n");
	exit(1);
    }


    /* filter */

    for (y=0; y<ysize; y++) {
	for (x=0; x<xsize; x++) {
	    sum += PIXEL(image,x,y)->r;
	    sum += PIXEL(image,x,y)->g;
	    sum += PIXEL(image,x,y)->b;
	}
    }
    avg = sum/(xsize*ysize);
    for (y=0; y<ysize; y++) {
	for (x=0; x<xsize; x++) {
	    psum = PIXEL(image,x,y)->r;
	    psum += PIXEL(image,x,y)->g;
	    psum += PIXEL(image,x,y)->b;
	    if (psum > avg)
		pval = colmax;
	    else
		pval = 0;
	    PIXEL(image,x,y)->r = pval;
	    PIXEL(image,x,y)->g = pval;
	    PIXEL(image,x,y)->b = pval;
	}
    }

    /* write result */
    
    fprintf(outfile, "P6 %d %d %d\n", xsize, ysize, colmax);
    if (!fwrite(image, sizeof(pixel), xsize*ysize, outfile)) {
	fprintf(stderr, "error in fwrite");
	exit(1);
    }

    exit(0);
}



