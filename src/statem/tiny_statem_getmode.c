/*--------------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|  TinySM_GetMode()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

PUBLIC U8 TinySM_GetMode( S_TinySM RAM_IS *sm )
{
   if( BSET(sm->flags, TinySM_Flags_Halt) )
      { return TinySM_Mode_Stop; }
   else
      { return TinySM_Mode_Run; }
}

// -------------------------------- eof -------------------------------------



 
