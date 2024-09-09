/*---------------------------------------------------------------------------
|
|         Test Fixture Support - To Send data out the TTY UART port
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "hostcomms.h"
#include "lineedit.h"         // LineEdit_AddCh(), LineEdit_CurrentBuf()
#include "tty_ui.h"           // S_UICmdSpec


/*-----------------------------------------------------------------------------------------
|
|  Comms_WrStr
|
|  Write null-terminated 'str' to the UART0 TX buffer. The NULL is not sent. 
|
|  Note that 'str' is a GENERIC pointer so can write const and non const strings.
|
|  If the string is longer than the free space in the buffer the routine waits for the Tx
|  interrupt to free up space. However, in case the UART isn't active, the routine won't
|  wait longer than 3 byte-sends before returning (with the remainder of 'str' unsent).
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Comms_WroteAStr;                           // Is set when a string is written

PUBLIC U8 WrStr( C8 const GENERIC *str, U8 flags )
{
   C8 const GENERIC *p;
   U8 ch;
   BIT wroteCR;
   
   if( BCLR(flags, _WrStr_Gated) ||                   // not a 'gated' write? OR
       (LineEdit_HostIsQuiet() && !UI_BeQuiet() ))    // (is a gated write) AND Host not sending AND not in 'quiet' mode?
   {                                                  // then OK to print
      for( p = str, wroteCR = 0; *p != 0; p++ )       // for each char to send
      {
         ch = *p;
         if( BSET(flags, _WrStr_AddLF) )              // Convert any lone CR's to CRLF?
         {     
            if( ch == '\r' )                          // CR?                 
               { wroteCR = 1; }                       // then note that
            else                                      // else not a CR
            {
               if( wroteCR )                          // but the last ch was a CR
                  { Comms_PutChar('\n'); }            // so add a LF
               wroteCR = 0;                           // this ch not a CR so clear 'mark'
            }
         }
         Comms_PutChar(ch);                           // Having added LF if required, now write char to port
      }
   }
   Comms_WroteAStr = 1;                               // Set this global flag, for anyone who is using it.
   return 1;                                          // success!
}

PUBLIC void Comms_WrStrConst( C8 CONST *str )
   { WrStr(str, 0); }

PUBLIC void Comms_WrStrConst_Gated( C8 CONST *str )
   { WrStr(str, _WrStr_Gated); }

PUBLIC U8 Comms_WrStr( C8 const GENERIC *str )
   { return WrStr(str,0); }

PUBLIC U8 Comms_WrStr_Gated( C8 const GENERIC *str )
   { return WrStr(str,_WrStr_Gated); }


// --------------------------------- eof --------------------------------------------




