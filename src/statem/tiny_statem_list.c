/*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|  A state machine may have:
|
|     - Up to 254 states
|     - Optional entry function which is run before the state function
|     - fixed length event list (set by TinySM_MaxEvents)
|     - event codes may be 1.. 0xFE
|
|  Public:
|
|     TinySM_Init()        - Set a state machine
|     TinySM_Execute()     - run state machine
|     TinySM_SendEvent()   - external process Adds event to list
|     TinySM_GetEvent()    - state machine queries any or specific events from list
|
|--------------------------------------------------------------------------*/


#include "common.h"
#include "tiny_statem.h"

/* The Tiny_StateM module calls state machines via function pointers. 8051 compilers typically
   overlay automatic variables. To do this they must determine the function call tree for the
   entire application. The Raisonance compiler misses some functions and so overlays when
   it shouldn't. Avoid this by disabling overlays for this module.
*/
#if _TOOL_IS == TOOL_RIDE_8051
#pragma NOOVERLAY
#endif

/*--------------------------------------------------------------------------------
|
|  TinySM_InitList()
|
|  Initialise / restart the state machines in 'list'
|
|  PARAMETERS:    'list' - list of state machines
|
|  RETURNS:  FALSE if init failed for any state machine
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 TinySM_InitList( S_TinySM_List CONST *list )
{
   U8 rIDATA c;

   for( c = 0; c < list->numStateMs; c++ )
   {
      if( !TinySM_Init( list->stateMs[c], list->cfgs[c]  ) ) 
      {
         return FALSE;
      }
   }

   return TRUE;
}

/*--------------------------------------------------------------------------------
|
|  TinySM_RunList()
|
|  Run the state machines in 'list'
|
|  PARAMETERS:    'list' - list of state machines
|
|  RETURNS:  TRUE if any state machine ran
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	----------------------------------------------------------
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT TinySM_RunList( S_TinySM_List CONST *list )
{
   U8 rIDATA c;
   BIT      ranAnySM,
            ranSMThisPass;
            
   c = 0;
   ranSMThisPass = FALSE;                       // Until at least one state machine is active

   while( c < list->numStateMs )                // until end of list
   {
      if( TinySM_Execute( list->stateMs[c] ) )  // run state machine; was active?
      {
         ranSMThisPass = TRUE;                  // then we have at least one active SM so far
         ranAnySM = TRUE;
      }
      if( c >= list->loopHWM && ranSMThisPass ) // below loop high water mark AND at least one SM above was active?
      {
         c = 0;                                 // then back to top of thread list
         ranSMThisPass = FALSE;                 // and clear 'active' flag
         continue;
      }
      else                                      // else continue down thread list
      {
         c++;
      }
   }
   return ranAnySM;
}

// ---------------------------------- eof -----------------------------------------

 
