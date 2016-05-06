/* mpe_graphics.c */
/* Custom Fortran interface file */
#include <stdio.h>
#include "mpeconf.h"
#include "mpe.h"

#include "mpetools.h"
#include "basex11.h"
#include "mpe.h"
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_open_graphics_ PMPE_OPEN_GRAPHICS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_open_graphics_ pmpe_open_graphics__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_open_graphics_ pmpe_open_graphics
#else
#define mpe_open_graphics_ pmpe_open_graphics_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_open_graphics_ MPE_OPEN_GRAPHICS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_open_graphics_ mpe_open_graphics__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_open_graphics_ mpe_open_graphics
#endif
#endif

#ifdef POINTER_64_BITS
extern void *MPIR_ToPointer();
extern int MPIR_FromPointer();
extern void MPIR_RmPointer();
#else
#define MPIR_ToPointer(a) (a)
#define MPIR_FromPointer(a) (int)(a)
#define MPIR_RmPointer(a)
#endif

/* In order to suppress warnings about prototypes, each routine is prototyped 
   right before the definition */
void mpe_open_graphics_ ANSI_ARGS(( MPE_XGraph *, MPI_Comm *, char *, int *, 
				    int *, int *, int *, int *, int * ));

void mpe_open_graphics_( handle, comm, display, x, y, w, h, is_collective, __ierr )
MPE_XGraph *handle;
MPI_Comm   *comm;
char       display[];
int*x,*y;
int*w,*h;
int*is_collective;
int *__ierr;
{
MPE_XGraph lhandle;
*__ierr = MPE_Open_graphics( &lhandle,*comm, display,*x,*y,*w,*h,
			    *is_collective);
*(int*)handle = MPIR_FromPointer(lhandle);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_capturefile_ PMPE_CAPTUREFILE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_capturefile_ pmpe_capturefile__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_capturefile_ pmpe_capturefile
#else
#define mpe_capturefile_ pmpe_capturefile_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_capturefile_ MPE_CAPTUREFILE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_capturefile_ mpe_capturefile__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_capturefile_ mpe_capturefile
#endif
#endif

void mpe_capturefile_ ANSI_ARGS(( MPE_XGraph *, char *, int *, int * ));
void mpe_capturefile_( handle, fname, freq, __ierr )
MPE_XGraph*handle;
char       *fname;
int*freq;
int *__ierr;
{
*__ierr = MPE_CaptureFile(*handle,fname,*freq);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_draw_point_ PMPE_DRAW_POINT
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_point_ pmpe_draw_point__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_point_ pmpe_draw_point
#else
#define mpe_draw_point_ pmpe_draw_point_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_draw_point_ MPE_DRAW_POINT
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_point_ mpe_draw_point__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_point_ mpe_draw_point
#endif
#endif

void mpe_draw_point_ ANSI_ARGS(( MPE_XGraph *, int *, int *, MPE_Color *, 
				 int * ));
void mpe_draw_point_( handle, x, y, color, __ierr )
MPE_XGraph*handle;
int*x,*y;
MPE_Color*color;
int *__ierr;
{
*__ierr = MPE_Draw_point(*handle,*x,*y,*color);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_draw_points_ PMPE_DRAW_POINTS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_points_ pmpe_draw_points__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_points_ pmpe_draw_points
#else
#define mpe_draw_points_ pmpe_draw_points_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_draw_points_ MPE_DRAW_POINTS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_points_ mpe_draw_points__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_points_ mpe_draw_points
#endif
#endif

void mpe_draw_points_ ANSI_ARGS(( MPE_XGraph *, MPE_Point *, int *, int * ));
void mpe_draw_points_( handle, points, npoints, __ierr )
MPE_XGraph*handle;
MPE_Point *points;
int*npoints;
int *__ierr;
{
*__ierr = MPE_Draw_points(*handle,
	(MPE_Point* )MPIR_ToPointer( *(int*)(points) ),*npoints);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_draw_line_ PMPE_DRAW_LINE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_line_ pmpe_draw_line__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_line_ pmpe_draw_line
#else
#define mpe_draw_line_ pmpe_draw_line_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_draw_line_ MPE_DRAW_LINE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_line_ mpe_draw_line__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_line_ mpe_draw_line
#endif
#endif

void mpe_draw_line_ ANSI_ARGS(( MPE_XGraph *, int *, int *, int *, int *, 
				MPE_Color *, int * ));
void mpe_draw_line_( handle, x1, y1, x2, y2, color, __ierr )
MPE_XGraph*handle;
int*x1,*y1,*x2,*y2;
MPE_Color*color;
int *__ierr;
{
*__ierr = MPE_Draw_line(*handle,*x1,*y1,*x2,*y2,*color);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_fill_rectangle_ PMPE_FILL_RECTANGLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_fill_rectangle_ pmpe_fill_rectangle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_fill_rectangle_ pmpe_fill_rectangle
#else
#define mpe_fill_rectangle_ pmpe_fill_rectangle_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_fill_rectangle_ MPE_FILL_RECTANGLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_fill_rectangle_ mpe_fill_rectangle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_fill_rectangle_ mpe_fill_rectangle
#endif
#endif

void mpe_fill_rectangle_ ANSI_ARGS(( MPE_XGraph *, int *, int *, int *, int *,
				     MPE_Color *, int * ));
void mpe_fill_rectangle_( handle, x, y, w, h, color, __ierr )
MPE_XGraph*handle;
int*x,*y,*w,*h;
MPE_Color*color;
int *__ierr;
{
*__ierr = MPE_Fill_rectangle(*handle,*x,*y,*w,*h,*color);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_update_ PMPE_UPDATE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_update_ pmpe_update__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_update_ pmpe_update
#else
#define mpe_update_ pmpe_update_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_update_ MPE_UPDATE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_update_ mpe_update__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_update_ mpe_update
#endif
#endif

void mpe_update_ ANSI_ARGS(( MPE_XGraph *, int * ));
void mpe_update_( handle, __ierr )
MPE_XGraph*handle;
int *__ierr;
{
*__ierr = MPE_Update(*handle);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_close_graphics_ PMPE_CLOSE_GRAPHICS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_close_graphics_ pmpe_close_graphics__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_close_graphics_ pmpe_close_graphics
#else
#define mpe_close_graphics_ pmpe_close_graphics_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_close_graphics_ MPE_CLOSE_GRAPHICS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_close_graphics_ mpe_close_graphics__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_close_graphics_ mpe_close_graphics
#endif
#endif

void mpe_close_graphics_ ANSI_ARGS(( MPE_XGraph *, int * ));
void mpe_close_graphics_( handle, __ierr )
MPE_XGraph *handle;
int *__ierr;
{
MPE_XGraph lhandle = (MPE_XGraph)MPIR_ToPointer( *(int*)(handle) );
*__ierr = MPE_Close_graphics( &lhandle );
if (!lhandle) {
    MPIR_RmPointer( *(int*)handle );
    }

*(int*)handle = 0;
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_make_color_array_ PMPE_MAKE_COLOR_ARRAY
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_make_color_array_ pmpe_make_color_array__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_make_color_array_ pmpe_make_color_array
#else
#define mpe_make_color_array_ pmpe_make_color_array_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_make_color_array_ MPE_MAKE_COLOR_ARRAY
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_make_color_array_ mpe_make_color_array__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_make_color_array_ mpe_make_color_array
#endif
#endif

void mpe_make_color_array_ ANSI_ARGS(( MPE_XGraph *, int *, MPE_Color [], 
				       int * ));
void mpe_make_color_array_( handle, ncolors, array, __ierr )
MPE_XGraph*handle;
int*ncolors;
MPE_Color  array[];
int *__ierr;
{
*__ierr = MPE_Make_color_array(*handle,*ncolors,
	(MPE_Color* )MPIR_ToPointer( *(int*)(array) ));
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_num_colors_ PMPE_NUM_COLORS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_num_colors_ pmpe_num_colors__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_num_colors_ pmpe_num_colors
#else
#define mpe_num_colors_ pmpe_num_colors_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_num_colors_ MPE_NUM_COLORS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_num_colors_ mpe_num_colors__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_num_colors_ mpe_num_colors
#endif
#endif

void mpe_num_colors_ ANSI_ARGS(( MPE_XGraph *, int *, int * ));
void mpe_num_colors_( handle, nc, __ierr )
MPE_XGraph*handle;
int        *nc;
int *__ierr;
{
*__ierr = MPE_Num_colors(*handle,nc);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_draw_circle_ PMPE_DRAW_CIRCLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_circle_ pmpe_draw_circle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_circle_ pmpe_draw_circle
#else
#define mpe_draw_circle_ pmpe_draw_circle_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_draw_circle_ MPE_DRAW_CIRCLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_circle_ mpe_draw_circle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_circle_ mpe_draw_circle
#endif
#endif

void mpe_draw_circle_ ANSI_ARGS(( MPE_XGraph *, int *, int *, int *, 
				  MPE_Color *, int * ));
void mpe_draw_circle_( graph, centerx, centery, radius, color, __ierr )
MPE_XGraph*graph;
int*centerx,*centery,*radius;
MPE_Color*color;
int *__ierr;
{
*__ierr = MPE_Draw_circle(*graph,*centerx,*centery,*radius,*color);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_fill_circle_ PMPE_FILL_CIRCLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_fill_circle_ pmpe_fill_circle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_fill_circle_ pmpe_fill_circle
#else
#define mpe_fill_circle_ pmpe_fill_circle_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_fill_circle_ MPE_FILL_CIRCLE
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_fill_circle_ mpe_fill_circle__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_fill_circle_ mpe_fill_circle
#endif
#endif

void mpe_fill_circle_ ANSI_ARGS(( MPE_XGraph *, int *, int *, int *, 
				  MPE_Color *, int * ));
void mpe_fill_circle_( graph, centerx, centery, radius, color, __ierr )
MPE_XGraph*graph;
int*centerx,*centery,*radius;
MPE_Color*color;
int *__ierr;
{
*__ierr = MPE_Fill_circle(*graph,*centerx,*centery,*radius,*color);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_draw_logic_ PMPE_DRAW_LOGIC
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_logic_ pmpe_draw_logic__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_logic_ pmpe_draw_logic
#else
#define mpe_draw_logic_ pmpe_draw_logic_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_draw_logic_ MPE_DRAW_LOGIC
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_draw_logic_ mpe_draw_logic__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_draw_logic_ mpe_draw_logic
#endif
#endif

void mpe_draw_logic_ ANSI_ARGS(( MPE_XGraph *, int *, int * ));
void mpe_draw_logic_( graph, function, __ierr )
MPE_XGraph*graph;
int*function;
int *__ierr;
{
*__ierr = MPE_Draw_logic(*graph,*function);
}
#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_line_thickness_ PMPE_LINE_THICKNESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_line_thickness_ pmpe_line_thickness__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_line_thickness_ pmpe_line_thickness
#else
#define mpe_line_thickness_ pmpe_line_thickness_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_line_thickness_ MPE_LINE_THICKNESS
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_line_thickness_ mpe_line_thickness__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_line_thickness_ mpe_line_thickness
#endif
#endif

void mpe_line_thickness_ ANSI_ARGS(( MPE_XGraph *, int *, int * ));
void mpe_line_thickness_( graph, thickness, __ierr )
MPE_XGraph*graph;
int*thickness;
int *__ierr;
{
*__ierr = MPE_Line_thickness(*graph,*thickness);
}

#ifdef MPI_BUILD_PROFILING
#ifdef FORTRANCAPS
#define mpe_add_rgb_color_ PMPE_ADD_RGB_COLOR
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_add_rgb_color_ pmpe_add_rgb_color__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_add_rgb_color_ pmpe_add_rgb_color
#else
#define mpe_add_rgb_color_ pmpe_add_rgb_color_
#endif
#else
#ifdef FORTRANCAPS
#define mpe_add_rgb_color_ MPE_ADD_RGB_COLOR
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpe_add_rgb_color_ mpe_add_rgb_color__
#elif !defined(FORTRANUNDERSCORE)
#define mpe_add_rgb_color_ mpe_add_rgb_color
#endif
#endif

void mpe_add_rgb_color_ ANSI_ARGS(( MPE_XGraph *, int *, int *, int *, 
				    MPE_Color *, int * ));
void mpe_add_rgb_color_( graph, red, green, blue, mapping, __ierr )
MPE_XGraph*graph;
int*red,*green,*blue;
MPE_Color *mapping;
int *__ierr;
{
*__ierr = MPE_Add_RGB_color(*graph,*red,*green,*blue,
	(MPE_Color* )MPIR_ToPointer( *(int*)(mapping) ));
}
