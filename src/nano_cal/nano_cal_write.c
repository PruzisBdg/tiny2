/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_cal.h"
#include "arith.h"      // ClipInt()

/*-----------------------------------------------------------------------------------
|
|  nCal_Write()
|
|  Set the current value of 'cal' to 'n', clipped to the cal limits.
|
--------------------------------------------------------------------------------------*/

PUBLIC void nCal_Write( S_NanoCal CONST *cal, S16 n )
{
   *(cal->current) = ClipInt(n, cal->min, cal->max);
}


// ------------------------------------- eof -------------------------------------------------


