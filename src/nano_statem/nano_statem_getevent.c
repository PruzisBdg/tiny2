/*---------------------------------------------------------------------------
|
|                        Nano State Machine
|
-------------------------------------------------------------------------*/


#include "common.h"
#include "nano_statem.h"

/*--------------------------------------------------------------------------------
|
|  NanoSM_GetEvent()
|
|  Searches for either a specific event(s) in the event list of the currently
|  active state machine.
|
|  The requested event(s) are deleted from the list.
|
|  PARAMETERS: 'reqEvent'  - event to get 
|
|  RETURNS:  an event OR 0 if no events or requested event not on list
|
|  12/20/06   SPJ    Handles _Tick requests correctly
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT NanoSM_GetEvent( U8 reqEvent )
{
   if( BSET(NanoSM_CurrentSM->events, reqEvent) )
   {
      CLRB(NanoSM_CurrentSM->events, reqEvent);
      return 1;
   }
   else
   {
      CLRB(NanoSM_CurrentSM->events, reqEvent);
      return 0;
   }
}

// ------------------------------ eof ------------------------------------

 
