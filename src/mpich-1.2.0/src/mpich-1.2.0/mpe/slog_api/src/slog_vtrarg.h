#if !defined( _SLOG )
#include "slog.h"
#endif  /*  if !defined( _SLOG )  */

SLOG_vtrarg_t *SLOG_Varg_Create( const SLOG_N_args_t  N_args,
                                 const SLOG_arg_t    *args );

void SLOG_Varg_Free( SLOG_vtrarg_t *vtr_arg );

SLOG_vtrarg_t *SLOG_Varg_Copy( const SLOG_vtrarg_t *src );

int SLOG_Varg_Print( const SLOG_vtrarg_t *vtr_arg,  FILE *outfd );

int SLOG_Varg_IsEqualTo( const SLOG_vtrarg_t *vtr1, const SLOG_vtrarg_t *vtr2 );

SLOG_N_vtrargs_t SLOG_Vargs_NumOfVtrs( const SLOG_vtrarg_lptr_t  *vtrs );

int SLOG_Vargs_AreEqualTo( const SLOG_vtrarg_lptr_t  *vtrs1,
                           const SLOG_vtrarg_lptr_t  *vtrs2 );
