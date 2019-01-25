/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Quiet mode control
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"        // WrStrLiteral
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"

extern BIT quiet;

/*-----------------------------------------------------------------------------------------
|
|  UI_Quiet()
|
|  To block unprompted printouts which get in the way of typing
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 CONST UI_Quiet_Help[]  = "quiet 1|on |0|off - block unprompted prints to Host";

PRIVATE U8 CONST actionList[] = "0 off 1 on";

PUBLIC U8 UI_Quiet( U8 * args )
{
   U8 action;

   if( Str_WordCnt(args) == 0 )                    // no args?
   {
      if( quiet )                                  // then report current status
         { WrStrLiteral("is on" ); }
      else
         { WrStrLiteral("is off" ); }
   }
   else
   {
      action = Str_FindWord(_StrConst(actionList), args); // else get arg
      
      switch(action)
      {
         case 0:                                   // '0'
         case 1:                                   // or 'off'
            quiet = 0;
            break;
         case 2:                                   // '1'
         case 3:                                   // or 'on'
            quiet = 1;
            break;
         default:                                  // unrecognised arg?
            WrStrLiteral("'0'/'off' OR '1'/'on'");  // so prompt the user
      }
   }
   return 1;
}

// -------------------------------------- eof ------------------------------------------------ 
