/*--------------------------------------------------------------------------------
|
|  TinySM_Run/Stop()
|
|  Run/halt a state machine
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

extern void tinySM_ClearAllEvents(S_TinySM RAM_IS *sm);

PUBLIC void TinySM_Run( S_TinySM RAM_IS *sm )
{
   CLRB( sm->flags, TinySM_Flags_Halt );
   tinySM_ClearAllEvents( sm );
}

PUBLIC void TinySM_Stop( S_TinySM RAM_IS *sm )
{
   SETB( sm->flags, TinySM_Flags_Halt );
}

// --------------------------------- eof --------------------------------------- 
