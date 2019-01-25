/*---------------------------------------------------------------------------
|
|                 Dynamically-allocated Timers
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "timers.h"

/*--------------------------------------------------------------------------------
|
|  Timer_RunRepeat_NoParm()
|
|  Like Timer_RunRepeat() but no 'parm'. Just pass zero (which will probably be
|  ignored by callback().
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Timer_RunRepeat_NoParm( T_LTime delay, void(*callback)(U16), U8 repeatCnt)
{
   return Timer_RunRepeat(delay, (void(*)(U16,U8))callback, 0, repeatCnt);
}


// ---------------------------- eof --------------------------------------   
