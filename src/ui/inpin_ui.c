/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "inpin.h"
#include "sysobj.h"
#include "class.h"
#include "wordlist.h"
#include "tty_ui.h"


/*--------------------------------------------------------------------------------
|
|  InPin_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 CONST UI_InPin_Help[]  = "Usage: InPin <pin name> <action = 'read'> ";

PRIVATE U8 CONST actionList[] = "read";
typedef enum { action_Read } E_Actions;

PUBLIC U8 InPin_HandleUI( U8 *args )
{
   S_InPin *pin;
   S_Obj CONST * obj;
   U8  action;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_InPin)) )
   {
      return 0;
   }
   else
   {
      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         pin = (S_InPin*)obj->addr;
      
         switch(action)
         {
            case action_Read:
               UI_PrintScalar( InPin_Read(pin), 0, _UI_PrintScalar_NoUnits);
               break;
               
            default:
               break;
         }
         return 1;
      }
   }
}

// -------------------------- eof ----------------------------------------- 
