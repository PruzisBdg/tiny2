/*--------------------------------------------------------------------------------
|
|  NanoSM_Restart()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

extern void nanoSM_ClearAllEvents(S_NanoSM RAM_IS *sm);

PUBLIC void NanoSM_Restart( S_NanoSM RAM_IS *sm )
{
   sm->state = 1;                // set to start state
   sm->flags = 0;                // set to run etc.
   nanoSM_ClearAllEvents(sm);
//   _ResetTimer(sm->lastRun);
}

// --------------------------- eof ---------------------------------------- 
