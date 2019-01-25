/*--------------------------------------------------------------------------------
|
|  TinySM_Restart()
|
|  Restart state machine 'sm'. 
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

extern void tinySM_ClearAllEvents(S_TinySM RAM_IS *sm);

PUBLIC void TinySM_Restart( S_TinySM RAM_IS *sm )
{
   sm->state = 1;                // set to start state
   sm->flags = 0;                // set to run etc.
   tinySM_ClearAllEvents(sm);
   _ResetTimer(sm->lastRun);
   _ResetTimer(sm->sinceNewState);
}

// --------------------------- eof ---------------------------------------- 
