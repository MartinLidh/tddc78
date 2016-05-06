#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "str_util.h"


/*
    Replace the specified string's all src_char's by the dest_char
*/
char *SLOG_str_replace( char *str, const char src_char, const char dest_char )
{
    char *cptr;
    for ( cptr = str; *cptr != '\0'; cptr ++ ) {
       if ( *cptr == src_char )
           *cptr = dest_char;
    }

    return str;
}



/*
    trim all the leading and trailing non-alphanumeric and non-underscore
    characters in the given string
*/
char *SLOG_str_trim( char *str )
{
    static char  *valid_chars = "abcdefghijklmnopqrstvvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789_";
           char  *str_beg, *str_end;
           int    str_lgth;

    /*  Locate the 1st valid char  */
    str_beg = strpbrk( str, valid_chars );
    /*  Locate the last valid char  */
    if ( str_beg != NULL ) {
        /*  str_end = strrpbrk( str, valid_chars );  */
        for (  str_end  = str_beg + strlen( str_beg ) - 1;
              *str_end != '\0'; str_end-- ) {
            if ( isalnum( (int) *str_end ) || *str_end == '_' )
                break;
        }
    }

    if ( str_end != NULL ) {
        str_lgth = str_end - str_beg + 1;
        if ( str_beg[ str_lgth ] != '\0' ) str_beg[ str_lgth ] = '\0';
    }

    return str_beg;
}



void SLOG_str_ncopy_set( char *dest, const char *src, const int string_size )
{
    const char          delimiter = '\t';
    const char          repl_char = ' ';
    const char          null_char = '\0';
          int           lgth;

    /*  The last 2 characters of char[string_size] are null & delimiter  */
    lgth = string_size - 2;

    strncpy( dest, src, lgth );
    dest[ lgth ] = null_char;
    SLOG_str_trim( dest );
    SLOG_str_replace( dest, delimiter, repl_char );
}
