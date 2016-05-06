
#ifndef _LISTS_H_
#define _LISTS_H_

typedef struct xpand_list_Strings_ {
  char **list;
  int nused;
  int size;
} xpand_list_Strings;

typedef struct xpand_list_Int_ {
  int *list;
  int nused;
  int size;
} xpand_list_Int;


typedef struct xpand_list_String_ {
  char *list;
  int nused;
  int size;
} xpand_list_String;

/* I got this trick from the Tcl implementation */
#ifdef _ANSI_ARGS_
#undef _ANSI_ARGS_
#endif

#ifdef __STDC__
#define _ANSI_ARGS_(x) x
#else
#define _ANSI_ARGS_(x) ()
#endif

xpand_list_Strings *Strings_CreateList _ANSI_ARGS_((int initialLen));

xpand_list_String *String_CreateList _ANSI_ARGS_((int initialLen));

xpand_list_Int *Int_CreateList _ANSI_ARGS_((int initialLen));

#define ListItem( listPtr, idx ) ( (listPtr)->list[(idx)] )

#define ListHeadPtr( listPtr ) ( (listPtr)->list )

#define ListDestroy( listPtr ) \
  {free( listPtr->list ); free( listPtr );}

#define ListSize( listPtr ) ( (listPtr)->nused )

#define ListClose( listPtr, headPtr, nitems ) { \
  headPtr = ListHeadPtr( listPtr ); \
  nitems = ListSize( listPtr ); \
  free( listPtr ); \
}

#define ListClear( listPtr ) {(listPtr)->nused=0;}

#endif
/* _LISTS_H_ */
