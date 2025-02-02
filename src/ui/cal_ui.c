/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|  Pruzina 1/10/06
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "cal.h"
         // sprintf()
#include <string.h>        // strcat()
#include "wordlist.h"      // Str_GetNthWord()
#include "hostcomms.h"     // Comms_WrStr()
#include "sysobj.h"        // S_ObjIO, S_Obj, GetObj() etc
#include "class.h"         // _Class_Cal
#include "tty_ui.h"        // UI_GetObj_MsgIfFail(), UI_PrintScalar()
#include "svc.h"           // InServiceMode()
#include "arith.h"         // ClipFloatToInt()
#include "romstring.h"     // _RomStr()
#include "util.h"


/*-----------------------------------------------------------------------------------
|
|  Cal_AppendFormatSpec()
|
|  Append to 'str', 'varName' followed by the io formatting read from 'io'. The resulting
|  string is a printf() formatter for the variable
|
--------------------------------------------------------------------------------------*/

PUBLIC void Cal_AppendFormatSpec( U8 *str, U8 GENERIC *varName, S_ObjIO CONST * io )
{
   strcat((C8*)str, (C8 GENERIC*)varName);                    // variable name e.g  "PeakAmpl"
   strcat((C8*)str, (C8 GENERIC*)_StrConst(io->outFormat));   // the format e.g "%f4.2"
   strcat((C8*)str, (C8 GENERIC*)_StrConst(io->units));       // followed by units e.g "mA dc"
}



/*-----------------------------------------------------------------------------------
|
|  Cal_HandleUI()
|
|  Handler for the user interface 'Cal' command. Format is:
|
|     Cal <cal_name> 
|         <action = 'read' | 'write' | 'store' | 'recall' | 'default' | 'report'>
|         ['raw']
|
--------------------------------------------------------------------------------------*/

typedef enum E_Actions { action_Read, action_Write, action_Store, action_Recall, action_Dflt, action_Report } E_T_Actions;
PUBLIC U8 CONST Cal_ActionList[] = "read write store recall default report";
PUBLIC U8 CONST Cal_Permissions[] = {1,0,0,0,0,1};

PUBLIC BIT Cal_CommandPermitted( U8 action )
{
   BIT allowed;

   if( InServiceMode() )
      { return 1; }                             // all commands allowed in service mode
   else                                         // else user mode
   { 
      allowed = Cal_Permissions[action];    // so read from permissions table
      if( !allowed )
         { WrStrLiteral("Not allowed in user mode\r\n"); }
      return allowed;                           // return the permission
   }   
}


   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_Cal_Help[]  = 
"Usage:\r\n\
    <calibration name> <action = 'read' | 'write' value_to_write | 'store' | 'recall' | 'dflt' | 'report' > ['raw']\r\n\
\r\n\
   cal n1 read               - print value of 'n1' with units\r\n\
   cal n1 read raw           - print raw value\r\n\
   ..   ..   write 3.85      - set to 3.85 (scaled)\r\n\
   ..   ..   write 3850 raw  - same as above but write raw value\r\n\
   ..   ..   default         - reset current value to default\r\n\
   ..   ..   store           - copy current value to NV store\r\n\
   ..   ..   recall          - copy NV value to current\r\n\
   ..   ..   report          - show all data on 'n1'\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PUBLIC U8 Cal_HandleUI( U8 *args )
{
   U8    b1[90],              // must be long enough to hold the 'report' format string
         b2[25],
         action;

   S_Cal *cal;
   float f1;
   S16   n;
   BIT   gotStored, doRaw;

   S_Obj   CONST * rIDATA obj;
   S_ObjIO CONST * rIDATA io;

   
   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_Cal)) )        // 1st arg not a cal?
   {
      return 0;
   }
   else
   {                                                           // 2nd arg not an action?
      if( (action = UI_GetAction_MsgIfFail(args, Cal_ActionList)) == _UI_NoActionMatched )
      {
         return 0;
      }
      else
      {
         doRaw = UI_RawInArgList(args);                        // specified 'raw'?
      
         if( !Cal_CommandPermitted(action) )                    // permitted (service mode)?
         {
            return 0;
         }
         else
         {
            cal = (S_Cal *)obj->addr;                          // cache the cal
            io = GetObjIO(_StrConst(obj->name));               // cache the IO format, if one exists

            switch( action )
            {
               case action_Read:
                   
                  if( doRaw ) { io = 0; }                      // force raw print?
                  UI_PrintScalar( Cal_Read(cal), io, _UI_PrintScalar_AppendUnits );     // show the current value of the Calibration
               break;

               case action_Write:                                       // 3rd arg isn't a number?
                  if( !GotFloatFromASCII(Str_GetNthWord(args, 2), &f1 ))
                  {
                     WrStrLiteral("value required\r\n");                // then say so
                  }
                  else                                                  // else got a value to write
                  {
                     if( !doRaw && io)                                  // no 'raw' AND io format exists for this cal?
                     {
                        f1 /= io->outScale;                             // then scale the input
                     }
                     Cal_Write(cal, ClipFloatToInt(f1));                // write the (scaled?) value
                  }
                  break;

               case action_Store: 
                  if( !Cal_Store(cal) )
                     { WrStrLiteral("Cal store failed\r\n"); }
                  break;

               case action_Recall: 
                  if( !Cal_Recall(cal) ) 
                     { WrStrLiteral("Cal recall failed\r\n"); }
                  break;

               case action_Dflt:
                  Cal_ToDefault(cal);
                  break;

               case action_Report:
 
                  if( !(gotStored = Cal_ReadStored(cal, &n)))     // failed attempt to read stored?
                     { strcpy( (char*)b2, "none" ); }                    // then will report "stored = none"

                  if( doRaw || !io)                               // raw print OR no io format for this variable?
                  {                                               // then print values raw
                     if( gotStored )                              // read stored into 'n' (above)?
                        { sprintf((C8*)b2, "%i", n); }            // converted stored (integer) value to string, overwriting "none" (above)

                        sprintf( PrintBuf.buf, _StrLit("%s: hdl = %i\r\n   val = %i stored = %s\r\n   min = %i max = %i dflt = %i"),  
                        _StrConst(GetObjName((T_ObjAddr)cal)),
                        (S16)cal->cfg->storeHdl,
                        cal->current,
                        b2, 
                        cal->cfg->min, 
                        cal->cfg->max, 
                        cal->cfg->dflt );
                  }
                  else                                                     // else print values scaled and formatted
                  {
                     if( gotStored )                                       // read stored into 'n' (above)?                             
                     {
                        b1[0] = '\0';                                      // start a string
                        Cal_AppendFormatSpec(b1, _StrLit(""),   io);       // make format specifier for value of 'stored'
                        sprintf( (C8*)b2, (C8*)b1, io->outScale*(float)n );// and converted stored value to foramtted string
                     }

                     strcpy( (char*)b1, _StrLit("%s: hdl = %i\r\n"));
                     Cal_AppendFormatSpec(b1, _StrLit("   val = "),     io);
                     Cal_AppendFormatSpec(b1, _StrLit("  stored = %s\r\n   min = "), io);  // stored value is string, created above
                     Cal_AppendFormatSpec(b1, _StrLit("  max = "),   io);
                     Cal_AppendFormatSpec(b1, _StrLit("  dflt = "),  io);

                     sprintf( PrintBuf.buf, (C8*)b1, 
                        _StrConst(GetObjName((T_ObjAddr)cal)),
                        (S16)cal->cfg->storeHdl,
                        io->outScale * (float)cal->current,
                        b2,
                        io->outScale * (float)cal->cfg->min,
                        io->outScale * (float)cal->cfg->max,
                        io->outScale * (float)cal->cfg->dflt
                        );
                  }
                  PrintBuffer();
                  break;

               default:
                  break;
            }
            return 1;
         }
      }
   }
}

// ------------------------------------- eof ------------------------------------------------- 
