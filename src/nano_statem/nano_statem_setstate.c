/*--------------------------------------------------------------------------------
|
|  NanoSM_SetState()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

PUBLIC void NanoSM_SetState( S_NanoSM RAM_IS *sm, U8 newState )
{
   if( newState <= sm->cfg->numStates && newState > 0 ) {   // legal new state?
      sm->state = newState;                                 // then apply it.
      }
}

// --------------------------- eof -------------------------------  
