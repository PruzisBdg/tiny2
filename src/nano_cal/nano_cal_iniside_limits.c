/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "nano_cal.h"
#include "arith.h"      // ClipInt()

/*-----------------------------------------------------------------------------------
|
|  nCal_InsideLimits()
|
|  Return TRUE if 'n' is inside the limits for 'cal'.
|
--------------------------------------------------------------------------------------*/

PUBLIC BIT nCal_InsideLimits( S_NanoCal CONST *cal, S16 n )
{
   return InsideLimitsS16(n, cal->min, cal->max);
}


// ------------------------------------- eof -------------------------------------------------

 
