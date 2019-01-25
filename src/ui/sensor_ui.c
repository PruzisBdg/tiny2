/*---------------------------------------------------------------------------
|
|                  Tiny Lib - Sensors UI
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "sensor.h"
#include "class.h"
#include "wordlist.h"
#include "arith.h"

#include "tty_ui.h"
#include "romstring.h"

extern S16 Sensor_ReadRaw(S_Sensor *s);
extern U8 Sensor_GetCfgFlags(S_Sensor *s);

/*--------------------------------------------------------------------------------
|
|  printCalOrConst()
|
-----------------------------------------------------------------------------------*/

PRIVATE void printCalOrConst( U8 CONST *name, S_CalOrConst CONST *k, U8 printCtl )
{
   if( k->isCal )
   {
      sprintf(PrintBuf.buf, " %s = ", _StrConst(name) );
      PrintBuffer();
      UI_PrintObject(GetObjBase((T_ObjAddr)k->val.asCal), printCtl);     
   }
   else
   {
      sprintf(PrintBuf.buf, " %s = %d ", _StrConst(name), k->val.asNum);
      PrintBuffer();
   }
}



/*--------------------------------------------------------------------------------
|
|  Sensor_HandleUI()
|
-----------------------------------------------------------------------------------*/

extern void UI_PrintObject( S_Obj CONST *obj, U8 printRaw );

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_Sensor_Help[]  = 
"Usage:\r\n\
    <sensor name> <action = ('read' [src] | 'report') [raw] >\r\n\
\r\n\
   sensor Pres1 read       - pressure sensor 'Pres1' in kPa\r\n\
   ..   ..      read src   - returns raw input to sensor\r\n\
   ..   ..      read raw   - pressure sensor 'Pres1' in internal units\r\n\
   ..   ..      report     - sensor OK or faulty\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PRIVATE U8 CONST actionList[] = "read report";
typedef enum { action_Read, action_Report } E_Actions;

PUBLIC U8 Sensor_HandleUI( U8 *args )
{
   S_Obj CONST *obj;
   S_Sensor *s;
   U8  action, printArgs;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_Sensor)) )
   {
      return 0;
   }
   else
   {
      s = (S_Sensor*)obj->addr;

      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         printArgs = ((U8)UI_RawInArgList(args) * _UI_PrintObject_Raw) | _UI_PrintObject_AppendUnits | _UI_PrintObject_PrependName;
         
         switch(action)
         {
            case action_Read:
               if( Str_WordInStr(args, _U8Ptr("src")) )           // read raw input to sensor?
                  { UI_PrintScalar( Sensor_ReadRaw(s), 0, 0); }   // then return that.
               else
                  { UI_PrintObject(obj, printArgs); }             // else return conditioned output
               break;
               
            case action_Report:
               UI_PrintObject(obj, printArgs );                   // print value, same as 'read'
                                                                  // Also print....
               sprintf(PrintBuf.buf, " raw = %d src = %d flags = %d", 
                  Sensor_Read(s),                                 // the 'raw' internal integer representation
                  Sensor_ReadRaw(s),                              // the unscaled A/D reading 'src'
                  (S16)s->flags);                                 // and the run-time flags
               PrintBuffer();
            
               // If the sensor has a full configuration then print that too

               if( Sensor_GetCfgFlags(s) != 0 )
               { 
                  WrStrLiteral("\r\n     Config: ");

                  printCalOrConst(_RomStr("offset"), &s->cfg->full.scale.ofs, printArgs);
                  printCalOrConst(_RomStr("numerator"), &s->cfg->full.scale.num, printArgs);
                  printCalOrConst(_RomStr("denominator"), &s->cfg->full.scale.den, printArgs);

                  sprintf(PrintBuf.buf, " flags = %x", (U16)Sensor_GetCfgFlags(s) );
                  PrintBuffer();
               }
               break;
            
            default:
               break;
         }
         return 1;
      }
   }
}

// ------------------------------- eof ------------------------------------------

