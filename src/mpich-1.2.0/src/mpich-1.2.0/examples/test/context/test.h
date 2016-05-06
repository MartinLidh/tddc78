/* Header for testing procedures */

#ifndef _INCLUDED_TEST_H_
#define _INCLUDED_TEST_H_

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

#if defined(NEEDS_STDLIB_PROTOTYPES)
#include "protofix.h"
#endif

void Test_Init ANSI_ARGS((char *, int));
#ifdef USE_STDARG
void Test_Printf ANSI_ARGS((char *, ...));
#else
/* No prototype */
void Test_Printf();
#endif
void Test_Message ANSI_ARGS((char *));
void Test_Failed ANSI_ARGS((char *));
void Test_Passed ANSI_ARGS((char *));
int Summarize_Test_Results ANSI_ARGS((void));
void Test_Finalize ANSI_ARGS((void));
void Test_Waitforall ANSI_ARGS((void));

#endif
