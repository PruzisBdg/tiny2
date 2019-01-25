/*---------------------------------------------------------------------------
|
|        Tiny Lib - Analog outputs (Actuators) - User Interface.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "actuator.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()

#include "arith.h"
#include "romstring.h"


extern void Actuator_WrAlways(S_Actuator *a, S16 n);


/*--------------------------------------------------------------------------------
|
|  Actuator_HandleUI()
|
-----------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_Actuator_Help[]  = 
"Usage:\r\n\
    <actuator name> <action = 'read' [raw]| 'write' value [raw ]| 'reset' | 'lock' | 'free' |'report' >\r\n\
\r\n\
   actuator pumpV read            - prints current value e.g 2.8V\r\n\
   ..   ..   ..   read raw        - prints raw value 2800\r\n\
   ..   ..   ..   write 3.3       - pumpV <- 3.3V \r\n\
   ..   ..   ..   write 3300 raw  - pumpV <- 3.3V \r\n\
   ..   ..        reset           - resets to default\r\n\
   ..   ..        lock            - so application can't write pumpV\r\n\
   ..   ..        free            - undoes lock\r\n\
   ..   ..        report          - current value, limits and lock status\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PRIVATE U8 CONST actionList[] = "read write reset lock free report";
typedef enum E_Actions { action_Read, action_Write, action_Reset, action_Lock, action_Free, action_Report } E_T_Actions;


PUBLIC U8 Actuator_HandleUI( U8 *args )
{
   S_Actuator *a;
   S_Obj CONST * obj;
   S_ObjIO CONST * io;
   U8  action;
   float f1;
   BIT doRaw;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_Actuator)) )
   {
      return 0;
   }
   else
   {
      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         a = (S_Actuator*)obj->addr;
         io = GetObjIO(_StrConst(obj->name));                 // cache the IO format, if one exists
         doRaw = UI_RawInArgList(args);

         switch(action)
         {
            case action_Read:
               if( doRaw )
               {
                  UI_PrintScalar( Actuator_Read(a), 0, _UI_PrintScalar_NoUnits);
               }
               else
               {
                  UI_PrintScalar( Actuator_Read(a), io, _UI_PrintScalar_AppendUnits);
               }
               break;
               
            case action_Write:
               
               if( Scanf_NoArgs(sscanf( (C8*)Str_GetNthWord(args, 2), "%f", &f1 ))) 
               {
                  WrStrLiteral("value?\r\n"); 
               }
               else
               {
                  if( io && !doRaw )
                  {
                     f1 /= io->outScale;
                  }
                  Actuator_WrAlways(a, ClipFloatToInt(f1) );
               }
               break;
               
            case action_Reset:
               Actuator_WrAlways(a, a->cfg->dflt);
               break;
               
            case action_Lock:
               a->locked = 1;
               break;
               
            case action_Free:
               a->locked = 0;
               break;
               
            case action_Report:
               WrStrLiteral(" value = " );
               UI_PrintScalar(a->value, io, _UI_PrintScalar_NoUnits);
               sprintf(PrintBuf.buf, "  locked = %d  limits: ", a->locked);
               PrintBuffer();
               UI_PrintScalar(a->cfg->min, io, _UI_PrintScalar_NoUnits);
               UI_PrintScalar(a->cfg->max, io, _UI_PrintScalar_AppendUnits);
               
               break;
            
            default:
               break;
         }
         return 1;
      }
   }
}

// ------------------------------ eof -----------------------------------------


 
