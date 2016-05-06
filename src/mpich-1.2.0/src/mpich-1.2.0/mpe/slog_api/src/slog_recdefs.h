#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif


int SLOG_RDEF_IsDuplicated( const SLOG_recdefs_table_t  *recdefs,
                            const SLOG_recdef_t         *in_def );

int SLOG_RDEF_Open( SLOG_STREAM  *slog );

int SLOG_RDEF_AddRecDef(       SLOG_STREAM          *slog,
                         const SLOG_intvltype_t      intvltype,
                         const SLOG_bebit_t          bebit_0,
                         const SLOG_bebit_t          bebit_1,
                         const SLOG_N_assocs_t       Nassocs,
                         const SLOG_N_args_t         Nargs );

int SLOG_RDEF_Close( SLOG_STREAM  *slog );

int SLOG_RDEF_SetExtraNumOfRecDefs(       SLOG_STREAM  *slog,
                                    const SLOG_uint32   Nentries_extra );

int SLOG_RDEF_AddExtraRecDef(       SLOG_STREAM          *slog,
                              const SLOG_intvltype_t      intvltype,
                              const SLOG_bebit_t          bebit_0,
                              const SLOG_bebit_t          bebit_1,
                              const SLOG_N_assocs_t       Nassocs,
                              const SLOG_N_args_t         Nargs );

SLOG_recdef_t *SLOG_RDEF_GetRecDef(       SLOG_recdefs_table_t   *recdefs,
                                    const SLOG_intvltype_t        intvltype,
                                    const SLOG_bebit_t            bebit_0,
                                    const SLOG_bebit_t            bebit_1 );

int SLOG_RDEF_CompressTable( SLOG_recdefs_table_t *recdefs );

int SLOG_RDEF_CompressedTableToFile( SLOG_STREAM  *slog );

void SLOG_RDEF_Free( SLOG_recdefs_table_t *recdefs );

int SLOG_RDEF_ReadRecDefs( SLOG_STREAM  *slog );

void SLOG_RDEF_Print( FILE* fd, const SLOG_recdefs_table_t *recdefs );



void SLOG_RecDef_Assign(       SLOG_recdef_t        *recdef,
                         const SLOG_intvltype_t      in_intvltype,
                         const SLOG_bebit_t          in_bebit_0,
                         const SLOG_bebit_t          in_bebit_1,
                         const SLOG_N_assocs_t       in_Nassocs,
                         const SLOG_N_args_t         in_Nargs );

void SLOG_RecDef_Print( const SLOG_recdef_t *def, FILE *fd );

int SLOG_RecDef_WriteToFile( const SLOG_recdef_t *def, FILE *fd );

int SLOG_RecDef_ReadFromFile( SLOG_recdef_t *def, FILE *fd );

int SLOG_RecDef_IsKeyEqualTo( const SLOG_recdef_t *def1,
                              const SLOG_recdef_t *def2 );

int SLOG_RecDef_IsValueEqualTo( const SLOG_recdef_t *def1,
                                const SLOG_recdef_t *def2 );

int SLOG_RecDef_IsEqualTo( const SLOG_recdef_t *def1,
                           const SLOG_recdef_t *def2 );


/*  SLOG_N_assocs_t  */
SLOG_int32
SLOG_RecDef_NumOfAssocs( const SLOG_recdefs_table_t   *recdefs,
                         const SLOG_intvltype_t        intvltype,
                         const SLOG_bebit_t            bebit_0,
                         const SLOG_bebit_t            bebit_1 );

/*  SLOG_N_args_t  */
SLOG_int32
SLOG_RecDef_NumOfArgs( const SLOG_recdefs_table_t   *recdefs,
                       const SLOG_intvltype_t        intvltype,
                       const SLOG_bebit_t            bebit_0,
                       const SLOG_bebit_t            bebit_1 );

SLOG_int32
SLOG_RecDef_SizeOfIrecInFile( const SLOG_recdefs_table_t   *recdefs,
                              const SLOG_rectype_t          rectype,
                              const SLOG_intvltype_t        intvltype,
                              const SLOG_bebit_t            bebit_0,
                              const SLOG_bebit_t            bebit_1 );


void SLOG_RecDef_SetUsed(       SLOG_recdef_t *def,
                          const int            boolean_flag );
