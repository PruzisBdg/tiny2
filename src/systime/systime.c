/*---------------------------------------------------------------------------
|
|                  Timers based of a Master System Tick.
|
|   Systime provides basic, guarded timing functions based off the system tick.
|   There's a master time updated in interrupt. Individual Systime(r)s are relative
|   to that master time.
|
|   Exports:
|       ZeroSysTime()   - Zero the master timer.
|       GetSysTime()    - Return the current time.
|       Now()           - Same as GetSysTime
|       MarkNow(t)      - Set 't' to now
|       MarkNever(t)    - Set 't' to the end of time
|       Elapsed(t)      - Time elapsed since 't' was 'marked'
|       Elapsed_S(t)    - 16 bit, econo version, for comparisions against a short, fixed limit.
|       ResetIfElapsed_S(t, intvl)
|                       - Return TRUE and reset 't' if >intvl has elapsed.
|
|   $Revision$
|
|   $Id$
|
|--------------------------------------------------------------------------*/


#include "libs_support.h"  // '_SystemTick_msec'. Include before 'systime.h' so it set _COMPILER_IS & _CPU_IS
#include "systime.h"

PUBLIC T_Time SysTime;      // The master time

/* --------------------------- ZeroSysTime -------------------------------

    Zero the system time.
*/

PUBLIC void ZeroSysTime(void)
{
    SysTime = 0;    // Khymer Rouge are here. It's now Year 0,
}

/* --------------------------- GetSysTime -------------------------------

    Return Now, the system time.

    We must block the interrupt which increments the master time; otherwise 'T_Time'
    will be diced if e.g a 16bit MCU increments a 32bit timer.
*/
PUBLIC T_Time GetSysTime(void)
{
    T_Time t;

    _BlockSystemTimerInterrupt();
    t = SysTime;
    _AllowSystemTimerInterrupt();

    return t;
}


/* --------------------------- SetSysTime -------------------------------

    Set the system timer, usually from an interrupt.
*/
PUBLIC void SetSysTime(T_Time t)
{
    SysTime = t;
}


/* --------------------------- MarkNow -------------------------------

    Set 't' to now, the current system time.
*/
PUBLIC void MarkNow(T_Timer *t)
{
    *t = GetSysTime();
}

/* --------------------------- MarkNever -------------------------------

    Set 't' to _Never, at the end of time
*/
PUBLIC void MarkNever(T_Timer *t)
{
    *t = _Never;
}


/* --------------------------- Elapsed -------------------------------

    Return the time elapsed since 't'. If t is ahead of now then return 0.
*/
PUBLIC T_Time Elapsed(T_Timer const *t)
{
    T_Time now;

    now = GetSysTime();
    return (*t > now) ? 0 : now - *t;
}

/* --------------------------- Elapsed_S -------------------------------

   Return the short time elapsed since 't'. If t is ahead of now then return 0.
   If the time elapsed exceeds max T_ShortTime then return MAX_SHORT_TIME.
*/
PUBLIC T_ShortTime  Elapsed_S(T_Timer const *t)
{
    T_Time elapsed;

    elapsed = Elapsed(t);
    return elapsed >= MAX_SHORT_TIME ? MAX_SHORT_TIME : elapsed;
}


/* --------------------------- ResetIfElapsed -------------------------------

   If more than 'intvl' has elapsed since 't' was marked then mark 't' and return
   TRUE. Else continue 't' and return FALSE.
*/
PUBLIC BOOL  ResetIfElapsed(T_Timer *t, T_Time intvl)
{
    if(Elapsed(t) >= intvl)
    {
        MarkNow(t);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/* --------------------------- ResetIfElapsed_S -------------------------------

   If more than 'intvl' has elapsed since 't' was marked then mark 't' and return
   TRUE. Else continue 't' and return FALSE.
*/
PUBLIC BOOL  ResetIfElapsed_S(T_Timer *t, T_ShortTime intvl)
{
    if(Elapsed_S(t) >= intvl)
    {
        MarkNow(t);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/* ----------------------------- T_TimeToSecs ---------------------------------------

   Assumes a whole number of ticks per sec.
*/

PUBLIC U32 T_TimeToSecs(T_Time t)
{
    return t / (U32)(1000 / _SystemTick_msec);
}


#ifdef __SYSTEM_IS_TDD_HARNESS__

/* --------------------------------- AddToSysTime ------------------------------------

   Add 't' to SysTime, but not beyond end-of-days. This is for the test harness time
   Simulator.
*/

PUBLIC void AddToSysTime(T_Time t)
{
    SysTime =
        (SysTime + t < SysTime)
            ? _EndOfDays
            : SysTime + t;
}

/* --------------------------------- SetSysTime ------------------------------------ */

PUBLIC void SetSysTime(T_Time t) { SysTime = t; }

#endif



// ------------------------------- eof ----------------------------------------
