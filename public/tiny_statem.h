 /*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|--------------------------------------------------------------------------*/

#ifndef TINY_STATEM_H
#define TINY_STATEM_H

#include "common.h"
#include "systime.h"

// Set the size of the event list for each state machine
#define TinySM_MaxEvents 5

typedef U8 TinySM_State;        // Up to 254 states
typedef U8 TinySM_StateCnt;
typedef U8 TinySM_Event;        // Up to 254 different events
typedef U8 TinySM_EventCnt;     // Up to 254 events in a queue
typedef U8 TinySM_Flags;

typedef struct 
{
   TinySM_EventCnt  cnt,                        // number of events in 'buf'
                    ticks;                      // number of tick events pending
   TinySM_Event     buf[TinySM_MaxEvents];      // holds events (except ticks)
} S_TinySM_EventList;
 

// Defines state machine
typedef struct 
{
   TinySM_StateCnt  numStates;                  // excluding the entry function
   TinySM_Flags     flags;                      // see 'Flags' below
   T_STime          freeRunIntvl;               // run interval in the absence of events;
                                                // 0 => run from events only

                                                // to the list of state functions
   TinySM_State(* CONST_FP * stateFuncs)(S_TinySM_EventList RAM_IS *events);
   TinySM_State ( *initFunc )(void);            // init function (optional)
} S_TinySM_Cfg;

/* ------ Flags

   AutoRun     - State machine is always run when called; used in datapaths.
                 Default is to run when timer expires.
                Otherwise run at state machine tick interval, if there is one.

   KeepEvents  - Don't clear the event list after a state function has run. State
                 functions are responsible for ensuring the event list is cleared.
                 The default is to clear the list.
                 Otherwise unhandled events are dumped after the statem machine has run.
*/
#define TinySM_CfgFlags_RunAtTick       _notB(0)
#define TinySM_CfgFlags_AutoRun         _BitM(0)
#define TinySM_CfgFlags_KeepEvents      _BitM(1)
#define TinySM_CfgFlags_DumpUnusedEvents _notB(1)



// the state machine itself
typedef struct 
{
   S_TinySM_Cfg CONST *cfg;            // configuration data
   TinySM_State       state;           // the current state
   S_TinySM_EventList events;          // holds events pending, if any
   T_Timer            lastRun,         // times free run intervals
                      sinceNewState;   // since last state change.
   TinySM_Flags       flags;
} S_TinySM;


/* ------ Flags

   Halt     - state machine is halted; won't run even when scheduled. Set by Stop(),
              cleared by Run().

   Error    - set if there was an error running a state machine.
   
   Trig1    - If set, will trigger the state machine in the same way as an event. The 
              regular event queue TinySM_SendEvent() is not reeentrant so 'Trig1' is used 
              to trigger the SM from interrupt. When the SM is set to run the trigger
              is stuffed onto the event list as 'TinySM_Event_Trig1'
*/
#define TinySM_Flags_Halt  _BitM(0)
#define TinySM_Flags_Error _BitM(1)
#define TinySM_Flags_Trig1 _BitM(2)


// Master modes, Stop, run, error
#define TinySM_Mode_Stop (0)
#define TinySM_Mode_Run  (1)


PUBLIC U8           TinySM_Init     ( S_TinySM RAM_IS *sm, S_TinySM_Cfg CONST *cfg );
PUBLIC void         TinySM_Restart  ( S_TinySM RAM_IS *sm );
PUBLIC TinySM_State TinySM_Execute  ( S_TinySM RAM_IS *sm );
PUBLIC void         TinySM_Run      ( S_TinySM RAM_IS *sm );
PUBLIC void         TinySM_Stop     ( S_TinySM RAM_IS *sm );
PUBLIC U8           TinySM_SendEvent( S_TinySM RAM_IS *sm, TinySM_Event event );
PUBLIC U8           TinySM_NumEvents( S_TinySM RAM_IS *sm );
PUBLIC void         TinySM_SetState ( S_TinySM RAM_IS *sm, TinySM_State state );
PUBLIC TinySM_State TinySM_GetState ( S_TinySM RAM_IS *sm );
PUBLIC U8           TinySM_GetMode  ( S_TinySM RAM_IS *sm );
PUBLIC T_Time       TinySM_SinceNewState( S_TinySM RAM_IS *sm );

PUBLIC U8 TinySM_GetEvent ( S_TinySM_EventList RAM_IS *ev, TinySM_Event reqEvent );
#define _TinySM_GetEvent_Wildcard  (0)		// matches any event, so gets the 1st.

#define _TinySM_SetTrig1(sm)  SETB((sm)->flags, TinySM_Flags_Trig1)

// Reserved states
#define TinySM_State_Unchanged 0xFE    // return this from entry function if no change in state

// Reserved Events
#define TinySM_Event_Null    (0)
#define TinySM_Event_Any     (1)
#define TinySM_Event_Tick    (2)
#define TinySM_Event_Trig1   (3)

// Start your events here
#define TinySM_1stUserEvent  (5)

// To initialise and run a list of state machines
typedef struct
{
   TinySM_StateCnt        numStateMs;
   S_TinySM     RAM_IS  * CONST *stateMs;    // To array of state machine addresses
   S_TinySM_Cfg CONST   * CONST *cfgs;       // To array of state machine cfg addresses
   TinySM_StateCnt        loopHWM;           // below this, loop to thread top if any SM ran
} S_TinySM_List;

PUBLIC U8 TinySM_InitList( S_TinySM_List CONST *list );
PUBLIC BIT TinySM_RunList( S_TinySM_List CONST *list );

#endif // TINY_STATEM_H

/* --------------------------------------- eof -------------------------------------------- */
