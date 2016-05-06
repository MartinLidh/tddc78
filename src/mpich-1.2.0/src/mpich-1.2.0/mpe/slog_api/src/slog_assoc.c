#include "slog_assoc.h"

int SLOG_Assoc_IsEqualTo( const SLOG_N_assocs_t   Nassocs1,
                          const SLOG_assoc_t     *assocs1,
                          const SLOG_N_assocs_t   Nassocs2,
                          const SLOG_assoc_t     *assocs2 )
{
    int ii;

    if ( Nassocs1 != Nassocs2 )
        return SLOG_FALSE;

    if ( assocs1 != assocs2 ) {
        for ( ii = 0; ii < Nassocs1; ii++ ) {
            if ( assocs1[ ii ] != assocs2[ ii ] )
                return SLOG_FALSE;
        }
    }

    return SLOG_TRUE;
}
