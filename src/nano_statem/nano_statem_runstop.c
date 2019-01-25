/*--------------------------------------------------------------------------------
|
|  NanoSM_Run/Stop()
|
|  Run/halt a state machine
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

extern void nanoSM_ClearAllEvents(S_NanoSM RAM_IS *sm);

PUBLIC void NanoSM_Run( S_NanoSM RAM_IS *sm )
{
   CLRB( sm->flags, NanoSM_Flags_Halt );
   nanoSM_ClearAllEvents( sm );
}

PUBLIC void NanoSM_Stop( S_NanoSM RAM_IS *sm )
{
   SETB( sm->flags, NanoSM_Flags_Halt );
}

// --------------------------------- eof --------------------------------------- 
