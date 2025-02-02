/*---------------------------------------------------------------------------
|
|                  Timers based of a Master System Tick.
|
|   Systime provides basic, guarded timing functions based off the system tick.
|   There's a master time updated in interrupt. Individual Systime(r)s are relative
|   to that master time.
|
|   The master time is generally the time since the application was booted.
|
|   Exports:
|       GetSysTime()    - Return the current time.
|       Now()           - Same as GetSysTime
|       MarkTime(t)     - Set 't' to now
|       Elapsed(t)      - Time elapsed since 't' was 'marked'
|       Elapsed_S       - 16 bit, econo version, for comparisions against a short, fixed limit.
|
|--------------------------------------------------------------------------*/


#ifndef SYSTIME_H
#define SYSTIME_H

#include "libs_support.h"
#include "spj_stdint.h"

/* For an embedded target _CPU_IS and _COMPILER_IS should be defined (in
   'projname_defines.h'. However if they are not, e.g in a test stand, then
   supply some values here.
*/
#ifndef _CPU_IS
    #define _CPU_ANY    0           // Will match any undefined.
    #define _CPU_NONE   1
    #define _CPU_PIC24  2
    #define _CPU_IS     _CPU_NONE
#endif

#ifndef _COMPILER_IS
    #define _COMPILER_GENERIC   0
    #define _COMPILER_XC16      1
    #define _COMPILER_IS _COMPILER_GENERIC
#endif

typedef U32     T_Time;         // Up to 10,000hrs at 10msecs.
typedef U16     T_ShortTime;    // Up to 10 minutes @  10msec
typedef T_Time  T_Timer;        // Each timer is just a time-stamp (for now)

#define MAX_SHORT_TIME MAX_U16

#define _Never     MAX_U32
#define _EndOfDays (_Never-1)

PUBLIC void        ZeroSysTime(void);
PUBLIC T_Time      GetSysTime(void);
PUBLIC void 	   SetSysTime(T_Time t);
#define _Now()     GetSysTime()
PUBLIC void        MarkNow(T_Timer *t);
PUBLIC void        MarkNever(T_Timer *t);
PUBLIC T_Time      Elapsed(T_Timer const *t);
PUBLIC T_ShortTime Elapsed_S(T_Timer const *t);
PUBLIC BOOL        ResetIfElapsed(T_Timer *t, T_Time intvl);
PUBLIC BOOL        ResetIfElapsed_S(T_Timer *t, T_ShortTime intvl);

PUBLIC U32         T_TimeToSecs(T_Time t);

// These are here for Told Tiny1 stuff. Don't use going forward.
#define _SetDelay(timer, delay)  {(timer) = _Now() + (delay);}
#define Timeout(t)   (_Now() > (t))


    #ifdef __SYSTEM_IS_TDD_HARNESS__
PUBLIC void AddToSysTime(T_Time t);
PUBLIC void SetSysTime(T_Time t);
    #endif

/* ---- Master Timer

   .The master timer is incremented in the system time interrupt and so inline it..
*/
extern T_Time SysTime;     // Up to 10,000hrs at 10msecs.

    #if _CPU_IS == _CPU_PIC24

/* 'T_Time' is 32bit; the PIC24 platform is 16bit.. 'SysTime' could get diced if the master
    timer interrupt is preempted by another interrupt which uses Systime(). This isn't likely,
    Systime gets read at thread level and this read is guarded against a dice so ne need to
    protect it here.

    Time stops at end-of-days. No rollover; otherwise all the bodies will start walking.
*/
#define _IncrementMasterTime() \
    { if(SysTime < _EndOfDays) { SysTime++; } }

        #if _COMPILER_IS == _COMPILER_XC16
/* !!!! XC16 Compiler ????. The XC16 compiler manual states that __builtin_disable_enable/interrupts()
   take the interrupt number as  parm. But XC16 1.21 expect these function to be f(void). So, watch out,
   this function either blocks everything (dangerous) or blocks noth (v. bad).

   Check with emulator whenever.
*/
#include <timer.h>
#define _BlockSystemTimerInterrupt()    { DisableIntT1; }
#define _AllowSystemTimerInterrupt()    { EnableIntT1; }
        #endif

    #elif _CPU_IS == _CPU_LPC824 || _CPU_IS == _CPU_CORTEXM0LPUS
// Time stops at end-of-days. No rollover; otherwise all the bodies will start walking.
#define _IncrementMasterTime() \
    { if(SysTime < _EndOfDays) { SysTime++; } }

#define _BlockSystemTimerInterrupt()    { }
#define _AllowSystemTimerInterrupt()    { }

    #else
#define _IncrementMasterTime() \
    { if(SysTime < _EndOfDays) { SysTime++; } }

#define _BlockSystemTimerInterrupt()    { }
#define _AllowSystemTimerInterrupt()    { }
    #endif

// (Long) constant times
#define _TicksMSec(t) (T_Time)((t)/_SystemTick_msec)
#define _TicksSec(t) (T_Time)( 1000.0 * (t) / _SystemTick_msec)
#define _TicksMin(t) ( 60 * _TicksSec((t)) )

// Short constant times
#define _TicksMSec_S(t) (T_ShortTime)((t)/_SystemTick_msec)
#define _TicksSec_S(t) (T_ShortTime)( 1000.0 * (t) / _SystemTick_msec)

#endif // SYSTIME_H

// -------------------------------------- eof ---------------------------------------------

