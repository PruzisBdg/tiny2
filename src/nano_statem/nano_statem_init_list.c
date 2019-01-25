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
|  NanoSM_InitList()
|
|  Initialise / restart the state machines in 'list'
|
|  PARAMETERS:    'list' - list of state machines
|
|  RETURNS:  FALSE if init failed for any state machine
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT NanoSM_InitList( void )
{
   U8 c;

   for( c = 0; c < NanoSM_List.numStateMs; c++ )
   {
      if( !NanoSM_Init( NanoSM_List.stateMs[c], NanoSM_List.cfgs[c]  ) ) 
      {
         return 0;
      }
   }
   return 1;
}

// ---------------------------------- eof -----------------------------------------

 
