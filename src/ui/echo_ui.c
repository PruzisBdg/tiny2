/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Terminal echo control
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"
#include <string.h>
#include "sysobj.h"           // S_Obj
#include "wordlist.h"      // Str_FindWord()

/*-----------------------------------------------------------------------------------------
|
|  UI_EchoHdlr()
|
|  Handler for 'echo' command. Set command on or off. OR echo a single string back to the
|  terminal.  
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

extern BIT EchoOn;     // Defaults to on - for use at terminal

PUBLIC U8 CONST UI_Echo_Help[] = 
"Sets echo mode or echos some text\r\nUsage: echo <'on' | 'off' | string to echo>";

PUBLIC U8 UI_EchoHdlr( U8 *args )
{
   if( strlen((C8*)args) == 0 )                  // no args?
   {
      if( EchoOn )                              // then print current echo setting
         { WrStrLiteral("echo is on"); }
      else
         { WrStrLiteral("echo is off"); }
   }
   else                                         // else got arg string
   {
      // If 'on' or 'off' then set accordingly else echo the string at the terminal
      if( strcmp((C8*)args, "on") == 0 )
         { EchoOn = 1; }
      else if( strcmp( (C8*)args, "off" ) == 0 )
         { EchoOn = 0; }
      else
         { Comms_WrStr( args ); }
   }
   return 1;
}


// -------------------------------------- eof ------------------------------------------------ 
