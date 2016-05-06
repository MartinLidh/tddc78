/*
   Color stuff for Upshot

   Ed Karrels
   Argonne National Laboratory
*/


#include "colors.h"

static char *color_list[] = {
  "red", "blue", "green", "cyan", "yellow", "magenta", "orange3",
  "maroon", "gray45", "gray75", "purple4", "darkgreen", "white",
  "black", 0
};

static int color_no = 0;

int Color_Reset() {
  color_no = 0;
  return 0;
}

char *Color_Get() {
  char *color;

  color = color_list[color_no++];
  if (!color_list[color_no])
    color_no = 0;

  return color;
}
