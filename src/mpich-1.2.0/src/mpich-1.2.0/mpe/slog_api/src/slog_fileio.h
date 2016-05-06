#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */



int slog_fseek( FILE *stream, SLOG_fptr offset, int whence );

SLOG_fptr slog_ftell( FILE *stream );
