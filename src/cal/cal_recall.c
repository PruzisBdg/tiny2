/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "cal.h"
#include "flashblk.h"
#include "links.h"      // Link_SenderChanged()

/*-----------------------------------------------------------------------------------
|
|  Cal_Recall()
|
|  Copy the stored value of 'cal' (in Flash) onto the current value.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Cal_Recall( S_Cal RAM_IS *cal )
{
   Link_SenderChanged((T_ObjAddr)cal);
   return FlashBlk_Read( (U8 RAM_IS*)&cal->current, cal->cfg->storeHdl, sizeof(S16) );
}



// ------------------------------------- eof -------------------------------------------------


