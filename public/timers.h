/*---------------------------------------------------------------------------
|
|                                Timers
|
|  An array of timers for producing delayed actions in increments of the system
|  tick.
|
|
|--------------------------------------------------------------------------*/

#ifndef TIMERS_H
#define TIMERS_H

#include "common.h"
#include "systime.h"

PUBLIC BIT Timer_Run(T_LTime delay, void(*callback)(U16), U16 parm);
PUBLIC BIT Timer_RunShort(T_STime shortDelay, void(*callback)(void));
PUBLIC BIT Timer_RunRepeat_NoParm( T_LTime delay, void(*callback)(U16), U8 repeatCnt);
PUBLIC BIT Timer_RunRepeat( T_LTime delay, void(*callback)(U16,U8), U16 parm, U8 repeatCnt);

#define _Timer_NoRepeat 0xFF

#endif // TIMERS_H 
