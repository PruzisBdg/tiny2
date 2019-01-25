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
#include "errcode.h"

/* The Tiny_StateM module calls state machines via function pointers. 8051 compilers typically
   overlay automatic variables. To do this they must determine the function call tree for the
   entire application. The Raisonance compiler misses some functions and so overlays when
   it shouldn't. Avoid this by disabling overlays for this module.
*/
#if _TOOL_IS == TOOL_RIDE_8051
#pragma NOOVERLAY
#endif

/* =============================== PRIVATE =================================== */


/* ------------------------------ tinySM_ClearAllEvents ------------------------------ */

PUBLIC void tinySM_ClearAllEvents(S_TinySM RAM_IS *sm)
{
   U8 rIDATA c;

   for(c = 0; c < TinySM_MaxEvents; c++) 
   {
      sm->events.buf[c] = 0;
   }
   sm->events.cnt = 0;
   sm->events.ticks = 0;
}


/* =========================== end: PRIVATE =================================== */


/*--------------------------------------------------------------------------------
|
|  TinySM_Init()
|
|  Init state machine 'sm'. If 'sm' has an init function run this.
|
|  PARAMETERS:  'sm'    - the new state machine
|               'cfg'   - SM setup
|
|  RETURNS:  FALSE if an init function failed
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 TinySM_Init( S_TinySM RAM_IS *sm, S_TinySM_Cfg CONST *cfg ) 
{
   sm->cfg = cfg;                         // link to state function list etc.
   TinySM_Restart(sm);                    // Set to initial state

   if( cfg->initFunc )                    // if there is an init function?
      { return (*cfg->initFunc)(); }      // then run it.
   else
      { return TRUE; }
}



/*--------------------------------------------------------------------------------
|
|  TinySM_Execute()
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
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
-----------------------------------------------------------------------------------*/

// To trap bad state functions for the debugger.
   #ifdef STATEM_LOG_BAD_STATE
PUBLIC U16 StateM_BadFunc;
PUBLIC U8  StateM_BadVar;
   #endif

PUBLIC U8 TinySM_Execute( S_TinySM RAM_IS *sm ) 
{
   U8 rIDATA newState;
   BIT      run;

   if(sm->state > sm->cfg->numStates) {                              // illegal state value?
      TrapSoftError(_TinyErrCode_BadStateVar);
      return FALSE;                                                  // quit rightaway   
      }
   else {                                                            // else state is valid
      run = FALSE;                                                   // unless TRUE below

      if( BCLR(sm->flags, TinySM_Flags_Halt) ) {                     // not halted?
      
         if( BSET( sm->flags, TinySM_Flags_Trig1 ) ) {               // trigger? (usually from interrupt)
            TinySM_SendEvent(sm, TinySM_Event_Trig1);                // then stuff onto event list (as an event)
            CLRB( sm->flags, TinySM_Flags_Trig1 );                   // and clear flag
            run = TRUE;                   
            }
         if( sm->events.cnt ||                                       // run if got event
             BSET(sm->cfg->flags, TinySM_CfgFlags_AutoRun) )         // or if set to always run
             { run = TRUE; }                                        

         if( sm->cfg->freeRunIntvl &&                                // freeRunIntvl == 0  =>  run on events only
             (ElapsedS(sm->lastRun) >= sm->cfg->freeRunIntvl))       // otherwise if elapsed?
            {
            run = TRUE;                                              // then run state machine
            TinySM_SendEvent(sm, TinySM_Event_Tick);                 // Let it know it was triggered by timer.
            
            /* Threads are often held off by others. To exactly time the next run, don't use the 
               conventional '_ResetTimer()'. Instead set 'lastRun' to when the thread SHOULD have 
               run, not to now, when it actually did run.
            */ 
            sm->lastRun += sm->cfg->freeRunIntvl;
            }
         }

      if( run ) {
         if(sm->cfg->stateFuncs[0]) {                                // has an entry function?
         
               #ifdef DBG_TINY1_RUNTIME_LOADING
            _Tiny1_MarkEnterThread;       // About to do compute work
               #endif
               
            newState = (*sm->cfg->stateFuncs[0])(&sm->events);       // then run entry function; pass it event list

               #ifdef DBG_TINY1_RUNTIME_LOADING
            _Tiny1_MarkLeaveThread;       // Compute work done
               #endif
               
            if( !newState ) {                                        // returned fail?
               SETB(sm->flags, TinySM_Flags_Halt | TinySM_Flags_Error); // so halt state machine
               return FALSE;                                         // then quit
               }
            else if( newState <= sm->cfg->numStates ) {              // else returned a legal state?
               if(newState != sm->state) {                           // State changed
                  _ResetTimer(sm->sinceNewState);                    // then mark that entered new state
                  sm->state = newState;                              // and update current state
                  }
               }
            else if( newState == TinySM_State_Unchanged ) {
               }
            else {                                                   // else returned an illegal state. Trap it.
                  #ifdef STATEM_LOG_BAD_STATE
               StateM_BadVar = sm->state;
               StateM_BadFunc = (U16)sm->cfg->stateFuncs[0];
                  #endif            
               TrapSoftError(_TinyErrCode_BadStateVar);
               return FALSE;
               }                                                     // so continue but in current state
            }                                
         // --- done with entry function; run state function
         
            #ifdef DBG_TINY1_RUNTIME_LOADING
         _Tiny1_MarkEnterThread;       // About to do compute work
            #endif
            
         newState = (*sm->cfg->stateFuncs[sm->state])(&sm->events);  // run current state; pass it the event list
         
            #ifdef DBG_TINY1_RUNTIME_LOADING
         _Tiny1_MarkLeaveThread;       // Compute work done
            #endif

         if( newState == 0 ||                                        // Returned bad/illegal state?
            (newState > sm->cfg->numStates && newState != TinySM_State_Unchanged) ) {
            SETB(sm->flags, TinySM_Flags_Halt | TinySM_Flags_Error); // so halt state machine
               #ifdef STATEM_LOG_BAD_STATE
            StateM_BadVar = sm->state;
            StateM_BadFunc = (U16)sm->cfg->stateFuncs[0];
               #endif            
            TrapSoftError(_TinyErrCode_BadStateRtn);                 // which should never happen, so trap it.
            return FALSE;                                            // and quit now
            }
         else {                                                      // else have legal next state
            if(newState != sm->state &&
               newState != TinySM_State_Unchanged) {                 // Is actually a new state?
               _ResetTimer(sm->sinceNewState);                       // then mark that we changed
               sm->state = newState;                                 // and apply it.
               }
            if( BCLR(sm->cfg->flags, TinySM_CfgFlags_KeepEvents) )   // assume state func handled all events?
               { tinySM_ClearAllEvents(sm); }                        // then clear event list
            return TRUE;                                             // and we ran successfully
            }
         }
      }
   return FALSE;     // didn't run
}




/* ----------------------------------- eof ----------------------------------------- */
