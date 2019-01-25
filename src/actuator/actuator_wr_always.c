/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "actuator.h"
#include "arith.h"


/*--------------------------------------------------------------------------------
|
|  Actuator_WrAlways()
|
|  Write Actuator, bypassing the lock. 
|
-----------------------------------------------------------------------------------*/

PUBLIC void Actuator_WrAlways(S_Actuator RAM_IS *a, S16 n)
{
   a->value = ClipInt(n, a->cfg->min, a->cfg->max);    // Clip new value to current limits

   if( a->cfg->write )                    // there's a write function? (there should be!!)
      { (*a->cfg->write)(a->value); }     // then run it.
}

// ----------------------------- eof ----------------------------------------


 
