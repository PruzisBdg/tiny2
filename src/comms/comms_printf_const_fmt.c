/*---------------------------------------------------------------------------
|
|         Test Fixture Support - To Send data out the TTY UART port
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include <stdarg.h>
#include "common.h"
#include "hostcomms.h"


/*-----------------------------------------------------------------------------------------
|
|  Comms_Printf_ConstFmt()
|
|  Printf() out Tiny1's serial port. The format string is 'const code' rather than generic.
|
------------------------------------------------------------------------------------------*/

extern S16 tprintf_internal( void(*putChParm)(U8), C8 CONST *fmt, va_list arg);
extern void TPrint_PutCh(U8 ch);


PUBLIC void Comms_Printf_ConstFmt(C8 CONST *fmt, ...) 
{ 
   va_list  ap;

   va_start(ap, fmt); 
   tprintf_internal(TPrint_PutCh, fmt, ap);   
   va_end(ap);
}

// --------------------------------- eof --------------------------------------------



 
