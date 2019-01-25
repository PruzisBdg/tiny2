/*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|--------------------------------------------------------------------------*/


#include "common.h"
#include "tiny_statem.h"
#include "systime.h"

/*--------------------------------------------------------------------------------
|
|  TinySM_SinceNewState()
|
|  Returns time since ''sm' started or changed state.
|
-----------------------------------------------------------------------------------*/

PUBLIC T_Time TinySM_SinceNewState( S_TinySM RAM_IS *sm )
{
   return Elapsed( &sm->sinceNewState );
}

// -------------------------------- eof --------------------------------------- 
