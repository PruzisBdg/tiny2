/*---------------------------------------------------------------------------
|
|                        Nano State Machine
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
|
-------------------------------------------------------------------------*/


#include "common.h"
#include "nano_statem.h"

/* =============================== PRIVATE =================================== */


/* ------------------------------ nanoSM_ClearAllEvents ------------------------------ */

PUBLIC void nanoSM_ClearAllEvents(S_NanoSM RAM_IS *sm)
{
   sm->events = 0;
}


/* =========================== end: PRIVATE =================================== */


/*--------------------------------------------------------------------------------
|
|  NanoSM_Init()
|
|  Init state machine 'sm'. If 'sm' has an init function run this.
|
|  PARAMETERS:  'sm'    - the new state machine
|               'cfg'   - SM setup
|
|  RETURNS:  FALSE if an init function failed
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 NanoSM_Init( S_NanoSM RAM_IS *sm, S_NanoSM_Cfg CONST *cfg ) 
{
   sm->cfg = cfg;                         // link to state function list etc.
   NanoSM_Restart(sm);                    // Set to initial state

   if( cfg->initFunc )                    // if there is an init function?
      { return (*cfg->initFunc)(); }      // then run it.
   else
      { return TRUE; }
}



/*--------------------------------------------------------------------------------
|
|  NanoSM_Execute()
|
|  Run state machine 'sm'. If 'sm' has an entry function run this first.
|  Once state function has run clear the event list.
|
|  PARAMETERS:  'sm' - the state machine
|
|  RETURNS:  FALSE if:
|              - entry function or state function returns FALSE
|              - state function returns illegal value for new state
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 NanoSM_Execute( S_NanoSM RAM_IS *sm ) 
{
   U8    newState;
   BIT   run;

   if(sm->state > sm->cfg->numStates) {                              // illegal state value?
      return FALSE;                                                  // quit rightaway   
      }
   else {                                                            // else state is valid
      run = FALSE;                                                   // unless TRUE below

      if( BCLR(sm->flags, NanoSM_Flags_Halt) ) {                     // not halted?
      
         if( sm->events ||                                           // run if got event
             BSET(sm->cfg->flags, NanoSM_CfgFlags_AutoRun) )         // or if set to always run
             { run = TRUE;  }                                        
         }

      if( run ) {
         if(sm->cfg->stateFuncs[0]) {                                // has an entry function?
            newState = (*sm->cfg->stateFuncs[0])();                  // then run entry function; pass it event list

            if( !newState ) {                                        // returned fail?
               SETB(sm->flags, NanoSM_Flags_Halt | NanoSM_Flags_Error); // so halt state machine
               return FALSE;                                         // then quit
               }
            else if( newState <= sm->cfg->numStates ) {              // else returned a legal state?
               sm->state = newState;                                 // then update current state
               }
            else if( newState == NanoSM_State_Unchanged ) {
               }
            else {                                                   // else returned > numStates (NanoSM_State_Unchanged=0xFE)
               }                                                     // so continue but in current state
            }                                
         
         // --- done with entry function; run state function
         
         if( sm->state > sm->cfg->numStates ) {      // bad return?
            }
         newState = (*sm->cfg->stateFuncs[sm->state])();             // run current state; pass it the event list

         if( newState == 0 || newState > sm->cfg->numStates ) {      // bad return?
            SETB(sm->flags, NanoSM_Flags_Halt | NanoSM_Flags_Error); // so halt state machine
            return FALSE;                                            // and quit now
            }
         else {                                                      // else have legal next state
            sm->state = newState;
            if( BCLR(sm->cfg->flags, NanoSM_CfgFlags_KeepEvents) )   // assume state func handled all events?
               { nanoSM_ClearAllEvents(sm); }                        // then clear event list
            return TRUE;
            }
         }
      }
   return FALSE;     // didn't run
}




/* ----------------------------------- eof ----------------------------------------- */
