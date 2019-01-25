/*---------------------------------------------------------------------------
|
|                 Dynamically-allocated Timers
|
|  Up to (10) concurrent timers. Each may exectute a callback() function at
|  a fixed delay. The callback() can be repeated up to 255 times.
|
|  Public:
|     Timer_Run()       - Excecute a callback after a specified delay
|     Timer_RunShort()  - Same as timer_Run() but no parm for callback function
|                         and delay time is T_STime (short time) only
|     Timer_RunRepeat() - Execute callback() multiple times, separated by a repeat interval.
|     Timer_RunRepeat_NoParm() 
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "systime.h"
#include "tiny_statem.h"
#include "timers.h"
#include "arith.h"

typedef struct
{
   T_Timer  t1;                        // when to timeout; _Never means the timer is free
   void(*callback)(U16 parm, U8 cnt);  // call this when times out
   U16      parm;                      // parm to callback
   T_STime  delay;                     // delay / repeat interval
   U8       repeatCnt;                 // number of times to repeat 'callback()'
} S_Timer;


/* This dummy variable prevents 'timers[]' from being linked into address 0 if this
   file (timers.obj) is the first on the link list. This is essential because, if
   timers[] is linked to address 0, then getFreeTimer() will return 0 = 'fail' 
   as the address of the 1st timer.
*/   
PRIVATE U8 DummyVariableTakesAddrZero;

PRIVATE S_Timer timers[10];            // Block of timers, dynamically allocated

/*--------------------------------------------------------------------------------
|
|  getTimer()
|
|  Get timer index (c) from the list
|
-----------------------------------------------------------------------------------*/

PRIVATE S_Timer * getTimer(U8 c) 
{
   return &timers[c];
}

/*--------------------------------------------------------------------------------
|
|  getFreeTimer()
|
|  Get first free timer from the pool. Returns zero if there were no free timers.
|
-----------------------------------------------------------------------------------*/

PRIVATE S_Timer * getFreeTimer(void)
{
   U8 c;

   for( c = 0; c < RECORDS_IN(timers); c++ ) // For each timer in the list
   {
      if( getTimer(c)->t1 == _Never )        // This timer is available?
      {
         return getTimer(c);                 // then return this timer.
      }
   }
   return 0;                                 // no free timers found  
}


/*--------------------------------------------------------------------------------
|
|  Timer_RunRepeat()
|
|  Call 'callback('parm')' after 'delay'. Repeat 'repeatCnt' times. ;parm' and 
|  the repeat count will be passed to the callback function each time it is run. 
|
|  Returns 0 if no timer was free, else 1
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Timer_RunRepeat( T_LTime delay, void(*callback)(U16, U8), U16 parm, U8 repeatCnt)
{
   S_Timer *t;

   if( !(t = getFreeTimer()) )
   {
      return 0;
   }
   else
   {
      _SetDelay(t->t1, delay);      // Set initial delay
      t->delay = delay;             // Must remember the delay
      t->callback = callback;       // Will callback this function
      t->parm = parm;               // Unused; but set to safe value
      t->repeatCnt = repeatCnt;     // This many repeats
      return 1;
   }
}


/*--------------------------------------------------------------------------------
|
|  initTimers()
|
|  Free all timers for use.
|
-----------------------------------------------------------------------------------*/

PRIVATE U8 initTimers(void)
{
   U8 c;

   for( c = 0; c < RECORDS_IN(timers); c++ ) // for each timer in the list
   {
      getTimer(c)->t1 = _Never;                 // Zero the callback, indicating the timer is unused.
   }
   return 1;
}


/*--------------------------------------------------------------------------------
|
|  runTimers()
|
|  Is triggered every (0.1sec) by a tick. Runs callback and frees any timers which 
|  have timed out.
|
-----------------------------------------------------------------------------------*/

PRIVATE U8 runTimers(S_TinySM_EventList *events)
{
   U8 c;
   S_Timer *t;
   
   for( c = 0; c < RECORDS_IN(timers); c++ )          // for each timer in the list
   {
      t = getTimer(c);                                // get the timer
      
      if( Timeout(t->t1) )                            // done?
      {
         if(t->repeatCnt != _Timer_NoRepeat)          // Do use repeats?
         { 
            if(DecrU8_NowZero(&t->repeatCnt))         // Decrement. Last/only repeat?
               { t->t1 = _Never; }                    // then mark timer as free
            else                                      // else more repeats to do
               { _SetDelay(t->t1, t->delay); }        // so will repeat after 'delay'                      
         }
         else                                         // else no repeats
         {
            t->t1 = _Never;                           // so this will be the last time
         }
         if(t->callback)                              // There's a callback()? (there should be!)
            { (*t->callback)(t->parm, t->repeatCnt); }// then run it, passing 'parm' and repeat count         
      }
   }
   return 1;
}


/* ----------------- Timers State Machine --------------------------------

   Must run in a thread list for timers to be active.
*/

#if _TOOL_IS == TOOL_Z8_ENCORE
PRIVATE U8(CONST_FP * stateFuncs[])(S_TinySM_EventList*) = 
{
   0,             // no entry function
   runTimers      // and just one state
};
#elif _TOOL_IS == TOOL_CC430 || _TOOL_IS == TOOL_GCC_ARM
PRIVATE U8(* const stateFuncs[])(S_TinySM_EventList*) = 
{
   0,             // no entry function
   runTimers      // and just one state
};
#elif _TOOL_IS == TOOL_RIDE_8051
PRIVATE U8(CONST_FP * CODE stateFuncs[])(S_TinySM_EventList*) = 
{
   0,             // no entry function
   runTimers      // and just one state
};
#endif

PUBLIC S_TinySM Timers_StateM;


PUBLIC S_TinySM_Cfg CONST Timers_StateM_Cfg = 
{
   1,                   // 1 state (excluding entry function)
   0,                   // no flags - default behaviour
   _TicksSec_S(0.1),    // send tick every 0.1sec, gives 0.1 secs resolution
   stateFuncs,          // list of state functions (just 1 for timers)
   initTimers
};

// ----------------------------- eof ---------------------------------------




 
