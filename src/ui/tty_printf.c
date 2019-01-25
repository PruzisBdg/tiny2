/* ----------------------------------------------------------------------------------
|
|  tty_printf()
|
|  printf() for the text ui. Text UI is queued via PrintBuf
|
------------------------------------------------------------------------------------ */

#include "libs_support.h"
#include "hostcomms.h"     // PrintBuffer()

#include <stdarg.h>

int tty_printf(const char GENERIC *format, ...)
{
   int ret;
   va_list ap;

   va_start(ap, format);
   ret = sprintf(PrintBuf.buf, format, va_arg(ap, const char GENERIC *) );
   Comms_WrStr(PrintBuf.buf);
   va_end(ap);
   
   return ret;
}

// ----------------------------------- eof ---------------------------------------------- 
