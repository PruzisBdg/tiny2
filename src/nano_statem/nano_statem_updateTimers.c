/*---------------------------------------------------------------------------
|
|                        Nano State Machine
|
|  A state machine may have:
|
|     - Up to 254 states
|     - Optional entry function which is run before the state function
|     - fixed length event list (set by TinySM_MaxEvents)
|     - event codes may be 1.. 0xFE
|
|  Public:
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

/*--------------------------------------------------------------------------------
|
|  NanoSM_UpdateTimers()
|
|  Run the timers for all state machines in 'list'. Set a tick event if any have
|  timed out. The timers are in a separate array which can be in 8051 data space
|  while the state machines are in idata.
|
|  N.B There's an in-line vesrion of this function in 'nano_statem.h'; for use in
|  interrupts.
|
-----------------------------------------------------------------------------------*/

PUBLIC void NanoSM_UpdateTimers(void )
{
   U8 c;
   
   for( c = 0; c < NanoSM_List.numStateMs; c++ )                     // For each state machine
   {
      if( NanoSM_List.stateMs[c]->cfg->freeRunIntvl )                // If state machine has a free-run interval
      {
         if(NanoSM_Timers[c])                                        // If it's timer > 0
         {
            NanoSM_Timers[c]--;                                      // then decrement the timer
         }
         else                                                        // else timer == 0
         {                                                           // so reset the timer
            NanoSM_Timers[c] = NanoSM_List.stateMs[c]->cfg->freeRunIntvl - 1;    // '-1' for correct interval
            NanoSM_SendEvent(NanoSM_List.stateMs[c], NanoSM_Event_Tick);   // and send a tick to the state machine
         }
      }
   }
}



// ---------------------------------- eof -----------------------------------------

 
