/*---------------------------------------------------------------------------
|
|                 Dynamically-allocated Timers
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "timers.h"

/*--------------------------------------------------------------------------------
|
|  Timer_Run()
|
|  Call 'callback('parm')' after 'delay'.
|
|  Returns 0 if no timer was free, else 1
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Timer_Run( T_LTime delay, void(*callback)(U16), U16 parm)
{
   return Timer_RunRepeat(delay, (void(*)(U16,U8))callback, parm, _Timer_NoRepeat);
}

// ---------------------------- eof -------------------------------------- 
