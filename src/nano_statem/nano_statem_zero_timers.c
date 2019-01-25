/*--------------------------------------------------------------------------------
|
|  NanoSM_ZeroTimers()
|
|  Zero all state machine timers, so each state machine will runs when
|  launched.
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"
#include "util.h"

PUBLIC void NanoSM_ZeroTimers(void)
{
   ZeroBytesU8((U8*)NanoSM_Timers, NanoSM_List.numStateMs * sizeof(T_NanoSM_Timer));
}
 
