/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "outpin.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()

#include "romstring.h"

extern void OutPin_UpdatePin(S_OutPin *p, U8 n);


/*--------------------------------------------------------------------------------
|
|  OutPin_HandleUI()
|
-----------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_OutPin_Help[]  = 
"Usage:\r\n\
    <pin name> <action = 'read' | ['write' [0|1|on|off |'toggle']] | 'reset' | 'lock' | 'free' | 'report' >\r\n\
\r\n\
   outPin valve3 read      - print current setting of valve3\r\n\
   outPin valve3 write on  - valve3 <- on \r\n\
   ..   ..       write 1   - ditto\r\n\
   ..   ..       toggle    - toggle valve 3\r\n\
   ..   ..       reset     - puts valve3 into reset state\r\n\
   ..   ..       lock      - so application can't write valve3\r\n\
   ..   ..       free      - undoes lock\r\n\
   ..   ..       report    - current setting and lock status\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PRIVATE U8 CONST actionList[] = "read write reset lock free report";
typedef enum { action_Read, action_Write, action_Reset, action_Lock, action_Free, action_Report } E_Actions;

PRIVATE U8 CONST writeActions[] = "0 off clear 1 on set toggle";
#define _IdxFirstWordForOn 3     // off actions are indices 0,1,2
#define _IdxToggle         6     // 'toggle' is index = 6


PUBLIC U8 OutPin_HandleUI( U8 *args )
{
   S_OutPin *pin;
   S_Obj CONST * obj;
   U8  action;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_OutPin)) )
   {
      return 0;
   }
   else
   {
      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         pin = (S_OutPin*)obj->addr;
         
         switch(action)
         {
            case action_Read:
               UI_PrintScalar( OutPin_Read(pin), 0, _UI_PrintScalar_NoUnits);
               break;
               
            case action_Write:
               if( (action = Str_FindWord(_StrConst(writeActions), Str_GetNthWord(args, 2))) == _Str_NoMatch )
               {
                  WrStrLiteral("say off, 0, 1 on\r\n"); 
                  return 0;
               }
               else                                   // got a legal parm for 'write'
               {
                  OutPin_UpdatePin(pin,               // then update 'pin' with
                     action < _IdxFirstWordForOn      // '0' 'off', 'clear'?
                        ? 0                           // then write to zero
                        : (action < _IdxToggle        // else '1', 'on' or 'set'
                           ? 1                        // then 1
                           : !OutPin_Read(pin)));     // else toggle, set to inverse of current pin setting.
               }
               break;
               
            case action_Reset:
               OutPin_UpdatePin(pin, 0);
               break;
               
            case action_Lock:
               pin->locked = 1;
               break;
               
            case action_Free:
               pin->locked = 0;
               break;
               
            case action_Report:
               sprintf(PrintBuf.buf, "value = %d locked = %d", OutPin_Read(pin), pin->locked);
               PrintBuffer();
               break;
            
            default:
               break;
         }
         return 1;
      }
   }
}


// --------------------------------- eof ------------------------------------------- 
