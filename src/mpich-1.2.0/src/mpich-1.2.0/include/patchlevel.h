#define PATCHLEVEL 1.2
#define PATCHLEVEL_MAJOR 1
#define PATCHLEVEL_MINOR 2
#define PATCHLEVEL_SUBMINOR 0
#define PATCHLEVEL_RELEASE_KIND ""
#ifndef PATCHLEVEL_RELEASE_DATE 
#ifdef RELEASE_DATE
#define PATCHLEVEL_RELEASE_DATE RELEASE_DATE
#else
#define PATCHLEVEL_RELEASE_DATE "$Date: 1999/12/02 13:22:11$"
#endif
#endif
