/*---------------------------------------------------------------------------
|
|            Test Fixture Support - User and Service Mode
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "svc.h"
#include "hostcomms.h"     // Comms_WrStr()
#include "common.h"
         // sscanf()
#include "wordlist.h"
#include <string.h>        // strcat()
#include "flashblk.h"
//#include "std_nvaddr.h"    // _nvaddr_ServicePassword
#define _nvaddr_ServicePassword 22
#include "romstring.h"
#include "tty_ui.h"

extern BIT ServiceMode;     

extern U8 CONST DefaultPassword[];

#define _MaxPasswordChars 8
#define _MaxPasswordStr (_MaxPasswordChars + 1)

extern U8 CurrentPassword[_MaxPasswordStr];

/*-----------------------------------------------------------------------------------------
|
|  Svc_HandleUI()
|
|  Command handler to set user/service modes. Command options are:
|
|     svc                  - returns current mode, user or service; also the current
|                            password if in service mode
|
|     svc on 'password'    - go to service mode if 'password' is correct
|
|     svc off              - goes to user mode
|
|     svc password 'new_password' - set the password to 'new_password' if the unit
|                                   is already in service mode.
|     
|
--------------------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST Svc_Help[] =
"svc [mode = 'on' password | 'off' |'setpass' new_password (8 letters max)\r\n\
    Sets user or service mode. With no args, returns current settings";
   #endif // INCLUDE_HELP_TEXT


typedef enum { action_On, action_Off, action_SetPassword } E_Actions;
PRIVATE C8 CONST actionList[] = "on off setpass";

PUBLIC U8 Svc_HandleUI( U8 *args )
{
   U8 argCnt,
      action;
   C8 actionStr[10];
   C8 newPassword[20];

   argCnt = sscanf( (C8*)args, "%s %s", actionStr, newPassword );

   if( Scanf_NoArgs(argCnt) )                            // no args?
   {                                                     // so print the current mode (and password)
      if( ServiceMode == 0 )
         { sprintf(PrintBuf.buf, "mode = user\r\n"); }
      else
         { sprintf(PrintBuf.buf, "mode = service password = %s\r\n", CurrentPassword); }
      PrintBuffer();
   }
   else                                                  // else at least one arg
   {
      if( (action = Str_FindWord(_StrConst(actionList), actionStr)) == _Str_NoMatch )   // not a legal arg?
      {
         WrStrLiteral("Cal option must be 'on', 'off', 'setpass'\r\n");              // so message
      }
      else                                                                          // else got a legal arg.
      {
         switch(action)                                                             // do action based on arg
         {
            case action_Off:
               ServiceMode = 0;
               break;

            case action_On:
               if( argCnt == 2 )                                        // user supplied password?
               {
                  if( (strcmp((C8*)CurrentPassword, newPassword) == 0) )     // password is correct?
                  {
                     ServiceMode = 1;                                   // so goto service mode   
                     WrStrLiteral("in service mode\r\n");
                  }
                  else                                                  // else wrong password
                     { WrStrLiteral("Wrong password\r\n"); }
               }
               else                                                     // else password not supplied
                  { WrStrLiteral( "password required\r\n" ); }
               break;

            case action_SetPassword:
               if( ServiceMode == 1 )                                   // in service mode?
               {
                  if( argCnt == 2 )                                     // got a password?
                  {
                     if( strlen(newPassword) > _MaxPasswordChars )
                        { WrStrLiteral("password 8 chars max\r\n"); }
                     else
                     {
                        strcpy((C8*)CurrentPassword, newPassword);

                        if( !FlashBlk_Write( CurrentPassword, _nvaddr_ServicePassword, strlen((C8*)CurrentPassword)+1) )
                           { WrStrLiteral("couldn't store new password\r\n"); }
                        else
                           { WrStrLiteral("new password stored\r\n"); }
                     }
                  }
                  else                                                  
                     { WrStrLiteral( "password required\r\n" ); }
               }
               else                                                     // else not in service mode
                  { WrStrLiteral( "Must be in service mode to change password\r\n" ); }
               break;
         }
      }
   }
   return 1;
}


// --------------------------- eof ------------------------------------------ 
