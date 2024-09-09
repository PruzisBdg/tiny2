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
|  Cal_Write_NoLink()
|
|  Set the current value of 'cal' to 'n', clipped to the cal limits. Unlike Cal_Write()
|  don't send a message to the links list to execute any actions tabled there.
|
|  This special verion of Cal_Write() was made for OxiSim, which needs to update it's
|  'SatPcent' and 'SatRatio' Cals in tandem without recursion via the links list.
|
--------------------------------------------------------------------------------------*/

PUBLIC void Cal_Write_NoLink( S_Cal RAM_IS *cal, S16 n )
{
   cal->current = ClipS16(n, cal->cfg->min, cal->cfg->max);
}


// ------------------------------------- eof -------------------------------------------------

 
