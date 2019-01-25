/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_cal.h"
#include "sysobj.h"     // T_ObjAddr, T_CodeAddr
#include "arith.h"

/*-----------------------------------------------------------------------------------
|
|  nCal_Init()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 nCal_Init( T_ObjAddr obj, T_CodeAddr init)
{
   if( nCal_ReadStored( (S_NanoCal CONST*)init) == 0xFFFF )                      // Flash holds 0xFFFF?
   {
      *(((S_NanoCal CONST*)init)->current) = ((S_NanoCal CONST*)init)->dflt;          // then set to default
   }
   else
   {
      *(((S_NanoCal CONST*)init)->current) = nCal_ReadStored( (S_NanoCal CONST*)init);     // else use stored value
   }
   return 1;
}

// ------------------------------------- eof -------------------------------------------------


