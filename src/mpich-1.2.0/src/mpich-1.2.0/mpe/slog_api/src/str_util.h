#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif

char *SLOG_str_replace( char *str, const char src_char, const char dest_char );

char *SLOG_str_trim( char *str );

void SLOG_str_ncopy_set( char *dest, const char *src, const int string_size );
