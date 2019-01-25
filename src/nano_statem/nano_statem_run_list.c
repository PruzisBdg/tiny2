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
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

// The currently active state machine, to be checked for events etc.
PUBLIC S_NanoSM RAM_IS *NanoSM_CurrentSM;


/*--------------------------------------------------------------------------------
|
|  NanoSM_RunList()
|
|  Run the state machines in 'list'
|
|  RETURNS:  TRUE if any state machine ran
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT NanoSM_RunList( void )
{
   U8    c;
   BIT   ranAnySM,
         ranSMThisPass;

   c = 0;
   ranSMThisPass = FALSE;                             // Until at least one state machine is active
   ranAnySM = FALSE;

   while( c < NanoSM_List.numStateMs )                // until end of list
   {
      NanoSM_CurrentSM = NanoSM_List.stateMs[c];      // Set the current state machine
      if( NanoSM_Execute( NanoSM_List.stateMs[c] ) )  // run state machine; was active?
      {
         ranSMThisPass = TRUE;                        // then we have at least one active SM so far
         ranAnySM = TRUE;
      }
      if( c >= NanoSM_List.loopHWM && ranSMThisPass ) // below loop high water mark AND at least one SM above was active?
      {
         c = 0;                                       // then back to top of thread list
         ranSMThisPass = FALSE;                       // and clear 'active' flag
         continue;
      }
      else                                            // else continue down thread list
      {
         c++;
      }
   }
   return ranAnySM;
}


// ---------------------------------- eof -----------------------------------------

 
