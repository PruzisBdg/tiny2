/*--------------------------------------------------------------------------------
|
|  NanoSM_GetMode()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "nano_statem.h"

PUBLIC U8 NanoSM_GetMode( S_NanoSM RAM_IS *sm )
{
   if( BSET(sm->flags, NanoSM_Flags_Halt) )
      { return NanoSM_Mode_Stop; }
   else
      { return NanoSM_Mode_Run; }
}

// -------------------------------- eof -------------------------------------



 
