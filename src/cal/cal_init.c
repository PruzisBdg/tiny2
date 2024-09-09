/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "cal.h"
#include "flashblk.h"
#include "sysobj.h"     // T_ObjAddr, T_CodeAddr
#include "links.h"      // Link_SenderChanged()
#include "arith.h"      // ClipS16()

/*-----------------------------------------------------------------------------------
|
|  Cal_Init()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Cal_Init( T_ObjAddr obj, T_CodeAddr init)
{
   ((S_Cal*)obj)->cfg = (S_CalCfg CONST *)init;

   if( !Cal_Recall( (S_Cal RAM_IS*)obj ))           // no stored value?
      { Cal_ToDefault( (S_Cal RAM_IS*)obj ); }      // then set default

   ((S_Cal*)obj)->valid = 1;
   return 1;
}

// ------------------------------------- eof -------------------------------------------------


