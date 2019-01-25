/*---------------------------------------------------------------------------
|
|         Test Fixture Support - To Send data out the TTY UART port
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "hostcomms.h"


/*-----------------------------------------------------------------------------------------
|
|  PrintBuffer()
|
|  Write null-terminated 'str' to the UART0 TX buffer. The NULL is not sent. 
|
|  Note that 'str' is a GENERIC pointer so can write const and non const strings.
|
|  If the string is longer than the free space in the buffer the routine waits for the Tx
|  interrupt to free up space. However, in case the UART isn't active, the routine won't
|  wait longer than 3 byte-sends before returning (with the remainder of 'str' unsent).
|
|  PARAMETERS:
|
|  RETURNS:    1 if all 'str' was written to Tx buffer / sent; else 0
|
------------------------------------------------------------------------------------------*/

PUBLIC S_PrintBuf PrintBuf = {_PrintBufLen, {0}};

PUBLIC void PrintBuffer(void) 
{ 
   Comms_WrStr(PrintBuf.buf); 
}

PUBLIC void PrintBuffer_Gated(void) 
{ 
   Comms_WrStr_Gated(PrintBuf.buf);
}

// --------------------------------- eof --------------------------------------------




