/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibration Vectors
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "calv.h"
#include "flashblk.h"

#include <string.h>
#include <stdlib.h>
#include "wordlist.h"
#include "hostcomms.h"     // Comms_WrStr()
#include "sysobj.h"
#include "class.h"
#include "links.h"
#include "tty_ui.h"        // UI_PrintVec()
#include "svc.h"           // InServiceMode()
#include "arith.h"
#include "romstring.h"


extern S_Vec * CalV_GetVec( S_CalV *cal );
extern S16 CalV_GetMin( S_CalV *cal );
extern S16 CalV_GetMax( S_CalV *cal );

#define getVec CalV_GetVec
#define getMin CalV_GetMin
#define getMax CalV_GetMax


/*-----------------------------------------------------------------------------------
|
|  CalV_HandleUI()
|
|  Handler for the user interface 'calv' command. Format is:
|
|     calv <cal_name> 
|         <action = 'read' | 'write' | 'store' | 'recall' | 'default' | 'report'>
|         ['raw']
|
|     'report' returns a printout of all details of the cal
|     'raw'    selects unscaled numbers
|
|                 
|
--------------------------------------------------------------------------------------*/

typedef enum { action_Read, action_Write, action_Store, action_Recall, action_Dflt, action_Report } E_Actions;
extern U8 CONST Cal_ActionList[];
extern U8 CONST Cal_Permissions[];
extern BIT Cal_CommandPermitted( U8 action );
extern void Cal_AppendFormatSpec( U8 *str, U8 CONST *varName, S_ObjIO CONST * io );

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_CalV_Help[] = 
"Usage\r\n\
   <cal_vector_name> <action = 'read' | 'write' index_to_write value_to_write | 'store' | 'recall' | 'dflt' | 'report' >  ['raw']\r\n\
\r\n\
Examples:\r\n\
   calv vec1 read               - print values of 'vec1'\r\n\
   calv vec1 read raw           - print raw values\r\n\
   ..   ..   write 2 2.77       - set element 2 to 2.27 (scaled)\r\n\
   ..   ..   write 2 2770 raw   - same as above but write raw value\r\n\
   ..   ..   default            - reset current value to default\r\n\
   ..   ..   recall             - copy stored value to current\r\n\
   ..   ..   report             - show all data on 'vec1'\r\n\
";
   #endif // INCLUDE_HELP_TEXT



PUBLIC U8 CalV_HandleUI( U8 *args )
{
   U8    b1[90],
         action,
         c;

   S_CalV *cal;
   float f1;
   S16   n;
   BIT   doRaw;

   S_Obj   CONST * rIDATA obj;
   S_ObjIO CONST * rIDATA io;

   
   doRaw = UI_RawInArgList(args);

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_CalVec)) )
   {
      return 0;
   }

   if( (action = UI_GetAction_MsgIfFail(args, Cal_ActionList)) == _Str_NoMatch)
   {
      return 0;
   }
   else
   {
      cal = (S_CalV *)obj->addr;
   
      if( Cal_CommandPermitted(action) )
      {
         io = GetObjIO(_StrConst(obj->name));                  // cache the IO format, if one exists

         switch( action )
         {
            case action_Read:                                  // show the current value of the cal vector

               if( doRaw )                                     // 3rd arg is 'raw'?
                  { io = 0; }                                  // then force raw print
               UI_PrintVector( getVec(cal), io, _UI_PrintVector_AppendUnits );   // Will append units if not 'raw'      
               break;


            case action_Write:

               if( Str_WordCnt(args) < 4 )
               {
                  WrStrLiteral( "both index and value-to-write required\r\n" );
               }
               else
               {
                  f1 = atof( (C8*)Str_GetNthWord(args, 3) );

                  if( !doRaw && io)                                  // AND io format exists for this cal
                  {
                     f1 /= io->outScale;                             // then scale the input
                  }
                  CalV_Write1(cal, ClipFloatToInt(f1), 0, (T_VecCols)atoi( (C8*)Str_GetNthWord(args, 2) ));
               }
               break;

            case action_Store: 
               if( !CalV_Store(cal) )
               {
                  WrStrLiteral("Error storing cal\r\n");
               }
               break;

            case action_Recall: 
               if( !CalV_Recall(cal) )
               {
                  WrStrLiteral("No stored cal\r\n");
               }
               break;

            case action_Dflt:
               CalV_ToDefault(cal);
               break;

            case action_Report:

               Comms_WrStrConst( GetObjName((T_ObjAddr)cal) );
               sprintf(PrintBuf.buf, ": hdl = %i ", cal->cfg->cfg.storeHdl );
               PrintBuffer();

               if( doRaw || !io)                               // OR no io format for this variable?
               {                                               // then print values raw
                  sprintf(PrintBuf.buf, "min = %i max = %i dflt = %i\r\nval = ",  
                     getMin(cal), 
                     getMax(cal), 
                     cal->cfg->cfg.dflt );
                  PrintBuffer();
                  UI_PrintVec( getVec(cal), _RomStr("%i "), 0 );
               }                            
               else                                            // else print values scaled and formatted
               {
                  b1[0] = '\0';
                  Cal_AppendFormatSpec(b1, (U8 CONST *)_RomStr("  min = "), io);  
                  Cal_AppendFormatSpec(b1, (U8 CONST *)_RomStr("  max = "),   io);
                  Cal_AppendFormatSpec(b1, (U8 CONST *)_RomStr("  dflt = "),  io);
                  strcat( (C8*)b1, "\r\nval = " );

                  sprintf( PrintBuf.buf, (C8*)b1, 
                     io->outScale * (float)getMin(cal),
                     io->outScale * (float)getMax(cal),
                     io->outScale * (float)cal->cfg->cfg.dflt
                     );
                  PrintBuffer();
                  UI_PrintVector( getVec(cal), io, _UI_PrintVector_AppendUnits );
               }
               // Print out the stored value, if it exists

               if( !CalV_GotStored(cal))                                // no stored cal?
               { 
                  WrStrLiteral("\r\nno stored cal\r\n"); 
               }
               else                                                     // else print stored values
               {
                  if( doRaw ) { io = 0; }

                  WrStrLiteral("\r\nstored = "); 

                  for( c = 0; c < Vec_Size(getVec(cal)); c++ )          // for each cal vector element
                  {                                                     // read and print the stored value
                     FlashBlk_ReadOfs( (U8*)&n, cal->cfg->cfg.storeHdl, sizeof(S16), sizeof(S16)*c );
                     UI_PrintScalar(n, io, _UI_PrintScalar_NoUnits);
                  }
                  if(io) { Comms_WrStrConst(io->units); }                    // if printed scaled then append units
               }
               break;
         }
      }
   }
   return 1;
}


// ------------------------------- eof --------------------------------------- 
