/*---------------------------------------------------------------------------
|
|                        Nano State Machine
|
-------------------------------------------------------------------------*/


#include "common.h"
#include "nano_statem.h"

/*--------------------------------------------------------------------------------
|
|  NanoSM_SendEvent()
|
|  Add 'event' to the event list of 'sm'
|
|  PARAMETERS: 'sm'     - the state machine
|              'event'  - event to add
|
-----------------------------------------------------------------------------------*/


PUBLIC void NanoSM_SendEvent( S_NanoSM RAM_IS *sm, U8 event )
{
   SETB(sm->events, event);
}

// ------------------------------- eof --------------------------------------- 
