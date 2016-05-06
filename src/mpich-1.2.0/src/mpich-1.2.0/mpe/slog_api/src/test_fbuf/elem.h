#include "../fbuf.h"

enum boolean { false = 0, true = 1 };

typedef struct {
        double  felem;
        int     icount;
        int    *iary;
} MixedElem;

MixedElem *elem_create( const double dval, const int ival );

void elem_free( MixedElem *elem );

char *elem2ptr( char *buf_ptr, const MixedElem *elem );

int elem2fbuf( filebuf_t *fbuf, const MixedElem *elem );

MixedElem *fbuf2elem( filebuf_t *fbuf );
