/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Objects Listing
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"

#include <string.h>
#include "sysobj.h"           // S_Obj
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"




/*-----------------------------------------------------------------------------------------
|
|  UI_DoHelp
|
|  Show help for command passed in 'args'. If 'args' is in the comamnd list (user typed "help
|  <command>" then display associated help string if there is one. If 'args' is empty (user
|  typed just "help" then list available commands.
|
|  PARAMETERS:
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_Help_Help[] =
"For help on a command type help 'cmd'\r\n\
Help syntax is:\r\n\
  <arg> = required argument   [arg] = optional\r\n\
  () = grouping      '|' = OR (choices)";
   #endif // INCLUDE_HELP_TEXT

PUBLIC U8 UI_DoHelp( C8 * args )
{
   U8 c, lineLen;
   S_Obj CONST *obj;
   C8 buf[15];

   if( Scanf_NoArgs(sscanf( (C8*)args, "%s", buf )) )                            // No args? (user typed just "help")
   {
      WrStrLiteral("Commands are:\r\n");                                         // then will list all commands

      for( c = 0, lineLen = 0; c < UICmdList.numCmds; c++ )                      // for each command (in the list)
      {
         lineLen += strlen((C8 GENERIC *)_StrConst(UICmdList.cmdSpecs[c].cmd));  // how long will line be when command name is printed

         if( lineLen > 50 )                                                      // wrap?, if necessary
         {
            WrStrLiteral("\r\n");
            lineLen = 0;
         }
         Comms_WrStrConst(UICmdList.cmdSpecs[c].cmd);                            // now print the command name
         WrStrLiteral(", ");
      }
      WrStrLiteral( "\r\nFor help on a command type 'help (or ?) <command>'   [] = optional args,  | = OR " );
   }
   else                                                                          // else at least one arg
   {
      // First check to see if its a command, print help if found.

      for( c = 0; c < UICmdList.numCmds; c++ )                                   // For each entry in the command list
      {
         if( strcmp((C8 GENERIC*)_StrConst(UICmdList.cmdSpecs[c].cmd), (C8*)buf) == 0 )   // arg matches this command?
         {
            if( UICmdList.cmdSpecs[c].helpStr )                                  // get help for this command?
            {                                                                    // then print it
               // Quote the command name followed by the help string
               Comms_WrStr(buf);
               WrStrLiteral(": ");
               Comms_WrStrConst(UICmdList.cmdSpecs[c].helpStr);
            }
            else
            {
               WrStrLiteral( "No help on this command" );
            }
            break;
         }
      }

      // Then check to see if its an object; print description if one exists

      if( (obj = GetObj(buf)) )                                     // got obj?
      {
         if( obj->desc )                                          // it has a description? (then print it)
            { Comms_WrStrConst( obj->desc); }
         else
            { WrStrLiteral( "No description for this object" ); }
      }

      // else neither a command or and object; say so.

      else if( c >= UICmdList.numCmds )
      {
         WrStrLiteral("Command or object not found");
      }
   }
   WrStrLiteral("\r\n");
   return 1;
}


// -------------------------------------- eof ----------------------------------------------------
