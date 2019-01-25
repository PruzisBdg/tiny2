/*---------------------------------------------------------------------------
|
|         Test Fixture Support - To Send data out the TTY UART port
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "hostcomms.h"


/*-----------------------------------------------------------------------------------------
|
|  Comms_WrLine
|
|  Write a line to the UART0 TX buffer. Terminate with CRLF.
|
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Comms_WrLine( U8 const GENERIC *str )
{
   U8 const GENERIC *p;
   U8 ch;
   
   for( p = str;; p++ )                       // for each char to send
   {
      ch = *p;
      
      if(ch == '\0' || ch == '\r' || ch == '\n' )  // end of string or CR or LF?
         { break; }                                // then goto terminate with CRLF
      else
         { Comms_PutChar(ch); }
   }
   Comms_PutChar('\r');
   Comms_PutChar('\n');
   return 1;                                      // success!
}


// --------------------------------- eof --------------------------------------------




