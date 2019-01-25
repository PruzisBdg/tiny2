/*--------------------------------------------------------------------------------
|
|  TinySM_Set/GetState()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

PUBLIC void TinySM_SetState( S_TinySM RAM_IS *sm, TinySM_State newState )
{
   if( newState <= sm->cfg->numStates && newState > 0 ) {   // legal new state?
      sm->state = newState;                                 // then apply it.
      }
}

PUBLIC TinySM_State TinySM_GetState( S_TinySM RAM_IS *sm )
{
   return sm->state;
}


// --------------------------- eof ------------------------------- 
