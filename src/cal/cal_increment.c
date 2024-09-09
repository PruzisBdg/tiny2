/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "cal.h"
#include "links.h"      // Link_SenderChanged()
#include "arith.h"      // ClipS16()

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

// ------------------------------------- eof -------------------------------------------------


