/*---------------------------------------------------------------------------
|
|                        Nano State Machine
|
-------------------------------------------------------------------------*/


#include "common.h"
#include "nano_statem.h"

/*--------------------------------------------------------------------------------
|
|  NanoSM_NumEvents()
|
|  Returns the number of events pending for 'sm', excluding ticks.
|
|  PARAMETERS:    'sm' - the state machine
|
|  RETURNS:  event count
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 NanoSM_NumEvents( S_NanoSM RAM_IS *sm ) 
{
   return sm->events;
}

// -------------------------------- eof --------------------------------------- 
