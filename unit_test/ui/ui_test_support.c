/* ---------------------------------------------------------------------------------------
|
| Support for the test harness to check the various build-variants of tiny1_printf.c
|
|
------------------------------------------------------------------------------------------ */

#include <stdlib.h>
#include <string.h>
#include "libs_support.h"
#include "ui_test_support.h"

// Prints to an out-stream go here; to be displayed or compared by a test.
#define _OStreamBufSize 500

typedef struct {
   U8 buf[_OStreamBufSize];
   U16 put, get;
} S_OStream;

PUBLIC S_OStream os;

PUBLIC void OStream_Reset(void) {
   os.put = 0;
   os.get = 0;
   memset(os.buf, 0, _OStreamBufSize); }

PUBLIC void OStream_Print(void) {
   printf("OStream: <%s>\r\n", os.buf); }

PUBLIC U8 const * OStream_Get(void)
   { return os.buf; }

// =============================== ui needs these ==============================

PUBLIC U8 Comms_PutChar( U8 ch ) {
   if(os.put < _OStreamBufSize-1) {
      os.buf[os.put++] = ch;
      return 1; }
   else {
      return 0; }
}

// ----------------------------------------- eof --------------------------------------------
