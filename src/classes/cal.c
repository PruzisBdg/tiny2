/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|  Pruzina 1/10/06
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "cal.h"
#include "flashblk.h"
#include "sysobj.h"     // T_ObjAddr, T_CodeAddr
#include "links.h"      // Link_SenderChanged()
#include "arith.h"      // ClipInt()

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



/*-----------------------------------------------------------------------------------
|
|  Cal_Read()
|
--------------------------------------------------------------------------------------*/

PUBLIC S16 Cal_Read( S_Cal RAM_IS *cal )
{
   return cal->current;
}


/*-----------------------------------------------------------------------------------
|
|  Cal_ReadStored()
|
|  Return 1 if read was successful, else 0.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Cal_ReadStored( S_Cal RAM_IS *cal, S16 *out )
{
   U8 rtn;

   rtn = FlashBlk_Read( (U8 RAM_IS*)out, cal->cfg->storeHdl, sizeof(S16) );
   return rtn;
}


/*-----------------------------------------------------------------------------------
|
|  Cal_Write()
|
|  Set the current value of 'cal' to 'n', clipped to the cal limits.
|
--------------------------------------------------------------------------------------*/

PUBLIC void Cal_Write( S_Cal RAM_IS *cal, S16 n )
{
   cal->current = ClipS16(n, cal->cfg->min, cal->cfg->max);
   Link_SenderChanged((T_ObjAddr)cal);
}


/*-----------------------------------------------------------------------------------
|
|  Cal_Increment()
|
|  Adjust the current value of 'cal' to 'n', clipped to the cal limits.
|
--------------------------------------------------------------------------------------*/

PUBLIC void Cal_Increment( S_Cal RAM_IS *cal, S16 n )
{
   cal->current = ClipS16(AplusBS16(cal->current, n), cal->cfg->min, cal->cfg->max);
   Link_SenderChanged((T_ObjAddr)cal);
}


/*-----------------------------------------------------------------------------------
|
|  Cal_ToDefault()
|
--------------------------------------------------------------------------------------*/

PUBLIC void Cal_ToDefault( S_Cal RAM_IS *cal )
{
   cal->current = cal->cfg->dflt;
   Link_SenderChanged((T_ObjAddr)cal);
}


/*-----------------------------------------------------------------------------------
|
|  Cal_Store()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Cal_Store( S_Cal RAM_IS *cal )
{
   return FlashBlk_Write( (U8 RAM_IS*)&cal->current, cal->cfg->storeHdl, sizeof(S16) );
}


/*-----------------------------------------------------------------------------------
|
|  Cal_Recall()
|
|  Copy the stored value of 'cal' (in Flash) onto the current value.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Cal_Recall( S_Cal RAM_IS *cal )
{
   U8 rtn;

   rtn = FlashBlk_Read( (U8 RAM_IS*)&cal->current, cal->cfg->storeHdl, sizeof(S16) );
   Link_SenderChanged((T_ObjAddr)cal);
   return rtn;
}



// ------------------------------------- eof -------------------------------------------------


