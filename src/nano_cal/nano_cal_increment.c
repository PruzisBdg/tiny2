/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "nano_cal.h"
#include "arith.h"      // ClipInt()

PRIVATE S16 getCurrent(S_NanoCal CONST *cal) { return *(cal->current); }

/*-----------------------------------------------------------------------------------
|
|  nCal_Increment()
|
|  Adjust the current value of 'cal' to 'n', clipped to the cal limits.
|
--------------------------------------------------------------------------------------*/

PUBLIC void nCal_Increment( S_NanoCal CONST *cal, S16 n )
{
   *(cal->current) = ClipInt(AddTwoS16(nCal_Read(cal), n), cal->min, cal->max);
}

// ------------------------------------- eof -------------------------------------------------


