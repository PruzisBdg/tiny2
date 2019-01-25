/*--------------------------------------------------------------------------------
|
|  TinySM_SetState()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

PUBLIC void TinySM_SetState( S_TinySM RAM_IS *sm, TinySM_State newState )
{
   if( newState <= sm->cfg->numStates && newState > 0 ) {   // legal new state?
      if(newState != sm->state) {                           // Is a new state
         _ResetTimer(sm->sinceNewState);                    // then mark that we changed.
         sm->state = newState;                              // then apply it.
         }
      }
}

// --------------------------- eof -------------------------------  
