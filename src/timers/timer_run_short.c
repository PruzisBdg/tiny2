/*---------------------------------------------------------------------------
|
|                 Dynamically-allocated Timers
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "timers.h"

/*--------------------------------------------------------------------------------
|
|  Timer_RunShort()
|
|  Economy version of Timer_Run(). No parm, no repeat, short-time only
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Timer_RunShort(T_STime shortDelay, void(*callback)(void))
{
   /* Note that the 'void callback(void)' is cast to one 'void callback(U16).
      That's OK, when the timer calls the function, it will pass it 0, which
      the function won't use (and shouldn't, it means nothing)
   */
   return Timer_Run((T_LTime)shortDelay, (void(*)(U16))callback, 0);
}

// ---------------------------- eof --------------------------------------  
