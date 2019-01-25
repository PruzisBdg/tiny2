 /*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|  1.1   2/15/05  See tiny_statem.c
|   
|
|
|
|
|
|   
|      Rev 1.1   Aug 20 2009 12:01:52   spruzina
|   Add NanoSM_ZeroTimers().
|   
|      Rev 1.0   May 19 2009 12:56:28   spruzina
|   Initial revision.
|  
|--------------------------------------------------------------------------*/

#ifndef NANO_STATEM_H
#define NANO_STATEM_H

#include "common.h"
#include "systime.h"

typedef U8 T_NanoEvents;
typedef U8 T_NanoSM_Timer;

// Defines state machine
typedef struct 
{
   U8             numStates,              // excluding the entry function
                  flags;                  // see 'Flags' below
   T_NanoSM_Timer freeRunIntvl;           // run interval in the absence of events;
                                          // 0 => run from events only

   U8(* CONST_FP * stateFuncs)(void);     // to the list of state functions
   U8 ( *initFunc )(void);                // init function (optional)
} S_NanoSM_Cfg;

/* ------ Flags

   AutoRun     - State machine is always run when called; used in datapaths.
                 Default is to run when timer expires.

   KeepEvents  - Don't clear the event list after a state function has run. State
                 functions are responsible for ensuring the event list is cleared.
                 The default is to clear the list.
*/
#define NanoSM_CfgFlags_AutoRun     _BitM(0)
#define NanoSM_CfgFlags_KeepEvents  _BitM(1)



// the state machine itself
typedef struct 
{
   S_NanoSM_Cfg CONST *cfg;      // configuration data
   U8                 state;     // the current state
   T_NanoEvents       events;    // holds events pending, if any
   U8                 flags;
} S_NanoSM;


/* ------ Flags

   Halt     - state machine is halted; won't run even when scheduled. Set by Stop(),
              cleared by Run().

   Error    - set if there was an error running a state machine.
   
   Trig1    - If set, will trigger the state machine in the same way as an event. The 
              regular event queue NanoSM_SendEvent() is not reeentrant so 'Trig1' is used 
              to trigger the SM from interrupt. When the SM is set to run the trigger
              is stuffed onto the event list as 'NanoSM_Event_Trig1'
*/
#define NanoSM_Flags_Halt  _BitM(0)
#define NanoSM_Flags_Error _BitM(1)


// Master modes, Stop, run, error
#define NanoSM_Mode_Stop 0
#define NanoSM_Mode_Run  1


PUBLIC U8   NanoSM_Init     ( S_NanoSM RAM_IS *sm, S_NanoSM_Cfg CONST *cfg ); 
PUBLIC void NanoSM_Restart  ( S_NanoSM RAM_IS *sm );
PUBLIC U8   NanoSM_Execute  ( S_NanoSM RAM_IS *sm ); 
PUBLIC void NanoSM_Run      ( S_NanoSM RAM_IS *sm ); 
PUBLIC void NanoSM_Stop     ( S_NanoSM RAM_IS *sm ); 
PUBLIC U8   NanoSM_NumEvents( S_NanoSM RAM_IS *sm );
PUBLIC void NanoSM_SetState ( S_NanoSM RAM_IS *sm, U8 state );
PUBLIC U8   NanoSM_GetState ( S_NanoSM RAM_IS *sm );
PUBLIC U8   NanoSM_GetMode  ( S_NanoSM RAM_IS *sm );

PUBLIC void  NanoSM_SendEvent( S_NanoSM RAM_IS *sm, U8 event );
PUBLIC BIT NanoSM_GetEvent ( U8 reqEvent );

#define _NanoSM_SetTrig1(sm)        SETB((sm)->events, NanoSM_Event_Trig1)
#define _NanoSM_SendEvent(sm, ev)   SETB((sm)->events, (ev))

// Reserved states
#define NanoSM_State_Unchanged 0xFE    // return this from entry function if no change in state

// Reserved Events
#define NanoSM_Event_Tick    _BitM(0)
#define NanoSM_Event_Trig1   _BitM(1)

// Start your events here
#define NanoSM_1stUserEventBit  2

// To initialise and run a list of state machines
typedef struct
{
   U8                     numStateMs;
   S_NanoSM     RAM_IS  * CONST *stateMs;    // To array of state machine addresses
   S_NanoSM_Cfg CONST   * CONST *cfgs;       // To array of state machine cfg addresses
   U8                     loopHWM;           // below this, loop to thread top if any SM ran
} S_NanoSM_List;

PUBLIC BIT NanoSM_InitList( void );
PUBLIC BIT NanoSM_RunList( void );
PUBLIC void NanoSM_UpdateTimers(void );
PUBLIC void NanoSM_ZeroTimers(void);

extern S_NanoSM RAM_IS *NanoSM_CurrentSM;

// The application must have these
extern S_NanoSM_List CONST NanoSM_List;
extern T_NanoSM_Timer RAM_IS NanoSM_Timers[];


/* Inline version of NanoSM_UpdateTimers(), for use in interrupts. This has no
   function calls, and so doesn't prompt the interrupt to stack the entire
   context.
   
   'Nano_Ctr' must be defined, as U8.
*/
#define _NanoSM_UpdateTimers                                                     \
   for( Nano_Ctr = 0; Nano_Ctr < NanoSM_List.numStateMs; Nano_Ctr++ )            \
   {                                                                             \
      if(NanoSM_List.stateMs[Nano_Ctr]->cfg->freeRunIntvl)                       \
      {                                                                          \
         if(NanoSM_Timers[Nano_Ctr])                                             \
         {                                                                       \
            NanoSM_Timers[Nano_Ctr]--;                                           \
         }                                                                       \
         else                                                                    \
         {                                                                       \
            NanoSM_Timers[Nano_Ctr] = NanoSM_List.stateMs[Nano_Ctr]->cfg->freeRunIntvl - 1;  \
            SETB( NanoSM_List.stateMs[Nano_Ctr]->events, NanoSM_Event_Tick);     \
         }                                                                       \
      }                                                                          \
   }



#endif // NANO_STATEM_H

/* --------------------------------------- eof -------------------------------------------- */
