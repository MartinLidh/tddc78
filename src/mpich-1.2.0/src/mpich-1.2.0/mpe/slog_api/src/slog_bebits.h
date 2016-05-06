#if !defined( _SLOG )
#include "slog.h"
#endif   /*  if !defined( _SLOG )  */



        /* SLOG_bebit(s)_xxxx methods */

int SLOG_bebit_valid( const SLOG_bebit_t bebit );

SLOG_bebit_t SLOG_bebit_conv( const SLOG_bebit_t bebit );

int SLOG_bebits_valid( const SLOG_bebits_t bebits );

SLOG_bebits_t SLOG_bebits_encode( const SLOG_bebit_t bebit_0,
                                  const SLOG_bebit_t bebit_1 );

int SLOG_bebits_decode( const SLOG_bebits_t bebits, SLOG_bebit_t *bebit_ary );
