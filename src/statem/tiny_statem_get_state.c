/*--------------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|  TinySM_Set/GetState()
|
-----------------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"

PUBLIC TinySM_State TinySM_GetState( S_TinySM RAM_IS *sm )
{
   return sm->state;
}


// --------------------------- eof ------------------------------- 
