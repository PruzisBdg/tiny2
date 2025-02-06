/*---------------------------------------------------------------------------
|
|                  Tiny Lib UI - On/Off Controls (OutPort)
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "outport.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()

#include "romstring.h"

extern void OutPort_Update(S_OutPort *p, U16 n);


/*--------------------------------------------------------------------------------
|
|  OutPort_HandleUI()
|
-----------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST UI_OutPort_Help[]  =
"Usage:\r\n\
    <port name> <action = 'read' | 'write' val | 'reset' | 'lock' | 'free' | 'report' >\r\n\
\r\n\
   outPin valves read      - print current setting of valve3\r\n\
   outPin valves write off  - valve3 <- on \r\n\
   ..   ..       write    - ditto\r\n\
   ..   ..       reset     - puts valve3 into reset state\r\n\
   ..   ..       lock      - so application can't write valve3\r\n\
   ..   ..       free      - undoes lock\r\n\
   ..   ..       report    - current setting and lock status\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PRIVATE C8 CONST actionList[] = "read write reset lock free report";
typedef enum { action_Read, action_Write, action_Reset, action_Lock, action_Free, action_Report } E_Actions;

PUBLIC U8 OutPort_HandleUI( C8 *args )
{
   S_OutPort *port;
   S_Obj CONST * obj;
   U8  action;
   U16 n;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_OutPort)) )
   {
      return 0;
   }
   else
   {
      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         port = (S_OutPort*)obj->addr;
         
         switch(action)
         {
            case action_Read:
               UI_PrintScalar( OutPort_Read(port), 0, _UI_PrintScalar_NoUnits);
               break;
               
            case action_Write:
               if( Str_FindWord(args, (C8 CONST *)_StrLit("off")) == 0 )
               {
                  OutPort_Update(port, 0);
               }
               else
               {
                  n = (U16)UI_GetScalarArg( args, 2, 0 );
                  OutPort_Update(port, n);
               }
               break;
               
            case action_Reset:
               OutPort_Update(port, 0);
               break;
               
            case action_Lock:
               port->locked = 1;
               break;
               
            case action_Free:
               port->locked = 0;
               break;
               
            case action_Report:
               sprintf(PrintBuf.buf, "value = %d locked = %d", OutPort_Read(port), port->locked);
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
