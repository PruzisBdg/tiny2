/*---------------------------------------------------------------------------
|
|                  Tiny Lib  Proportional-Integral Servos
|
|  A servo links a Sensor and an Actuator. This servo has input and output
|  setpoints and separate proportional and integral gains.
|
|     out_drive = 
|        out_set_point - (integral_gain * error_acc) - (proportional_gain * input_error)
|
|  where:
|     input_error = sensor_reading - input_set_point.
|  and:
|     error_accumulator = SUM(input_error(s), every 100msec)
|
|  The servo limits the total in the error accumulator to span 1/2 the output
|  range of the Actuator. This to minimise backlash. So for best performance
|  the Actuator set limits should be no wider than necessary.
|
|  Public:
|     Servo_Reset()
|     Servo_Init()
|     Servo_Stop()
|     Servo_Run()
|     Servo_IsRunning()
|     S_TinySM            ServoPI_StateM
|     ServoPI_StateM_Cfg  ServoPI_StateM_Cfg
|
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "servo.h"
#include "tiny_statem.h"
#include "sensor.h"        // Sensor_Read()
#include "actuator.h"      // Actuator_Write()
#include "arith.h"         // ClipLong()
#include "util.h"          // CopyBytesU8()

//#include "app_firm.h"

#include "hostcomms.h"

#define DBG_SERVO

   #ifdef DBG_SERVO
#include "systime.h"

PRIVATE T_Timer dbgTimer;
   #endif

// Events
#define _Servo_Event_Stop      TinySM_1stUserEvent
#define _Servo_Event_Run       (TinySM_1stUserEvent + 1)

// Just one servo for now. Later there may be a list of these
extern S_ServoUI servo1;
#define sv1 (servo1.theServo)    // shorthand

PRIVATE BIT errAccClipped;

/*--------------------------------------------------------------------------------
|
|  showInternalNums()
|
-----------------------------------------------------------------------------------*/

PRIVATE void showInternalNums(S16 err)
{
   sprintf(PrintBuf.buf, "Servo: sens %d  act %d  set %d  pGain %d iGain %d\r\n"
                         "   ofs %d  err %d  acc %d min/max %d\r\n ", 
      Sensor_Read(sv1.cfg.sensor),
      Actuator_Read(sv1.cfg.actuator),
      sv1.cfg.setPoint,
      sv1.cfg.pGain,
      sv1.cfg.iGain,
      sv1.cfg.actuatorOfs,
      err,
      sv1.errAcc,
      sv1.accMax );
   PrintBuffer();         
}

/*--------------------------------------------------------------------------------
|
|  Servo_Reset()
|
|  Reset the integral error accumulator. Regenerate the error accumulator limit 
|  from the output Actuator set-limits and the integral gain. The limits are chosen
|  to span half the input range of the Actuator. (i.e 1/4 each way of the center point.
|  It's assumed that the proportional gain and set points are chosen so the the 
|  necessary Actuator range can be covered.
|
-----------------------------------------------------------------------------------*/

PUBLIC void Servo_Reset(S_Servo *sv)
{
   sv->accMax = AmulBdivC_S16(
                  sv->cfg.actuator->cfg->max - sv->cfg.actuator->cfg->min, // Actuator input span
                  _Servo_IGainScale/4,                                     // x 1/4 each way of center
                  sv1.cfg.iGain) - 1;
                  
   /* Note: Subtract 1 from the derived limit. This is done for limits checking during
      servo run. This limits 'accMax' to 32766 and ensure thats a greater-than test for
      servo clipping can always return true.
   */
                  
   sv->errAcc = 0;
   errAccClipped = 0;
}

/*--------------------------------------------------------------------------------
|
|  Servo_Init()
|
|  Make S_Servo 'sv' from S_Servo 'init'.
|
-----------------------------------------------------------------------------------*/

PUBLIC void Servo_Init(S_Servo *sv, S_ServoCfg CONST *init)
{
   sv->cfg = *init;        // just copy the whole init to the runtime image
   Servo_Reset(sv);        // and reset the servo                     
}


/*-----------------------------------------------------------------------------------------
|
|  initServo()
|
|  Servo state machine init function. Note that because the servo may not be configured
|  yet we can't reset it here.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 initServo(void)
{
   return 1;
}


// Machine states
#define _sStop 1
#define _sRun  2

/*-----------------------------------------------------------------------------------------
|
|  state_Stop()
|
|  Servo is stopped. Run if instructed
|
--------------------------------------------------------------------------------------------*/

PRIVATE U8 state_Stop(S_TinySM_EventList *events)
{
   if( TinySM_GetEvent(events, _Servo_Event_Run) )
   {
      Servo_Reset(&sv1);
      
         #ifdef DBG_SERVO
      ResetTimer(&dbgTimer);
         #endif
         
      return _sRun;
   }
   else
   {
      return _sStop;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  state_Run()
|
|  Run the servo (every 100msec), but stop if instructed
|
|     actuator_drive = 
|        output_set_point - (integral_gain * error_accumulator) - (proportional_gain * input_error)
|
|  where:
|     input_error = sensor_reading - input_set_point.
|  and:
|     error_accumulator = SUM(input_error(s))
|
--------------------------------------------------------------------------------------------*/

#define InsideLimitsS16(n, l, u)  ((n) >= (l) && (n) <= (u))

PRIVATE U8 state_Run(S_TinySM_EventList *events)
{
   S16 err;                // input error
   static U8   runDiv;     // to run the servo an interval which is a multiple of 100msec.

   if( TinySM_GetEvent(events, _Servo_Event_Stop) )
   {
      return _sStop;
   }
   else                                         // else continue in 'run'
   {
      if( DecrU8_NowZero(&runDiv) )             // Decrement the run interval divisor. If rollover?
      {
         runDiv = sv1.cfg.runIntvl;             // the reload the div counter
         
         // Get input error
         err = Sensor_Read(sv1.cfg.sensor) - sv1.cfg.setPoint;       // Get input error
         
         // The 1st time the error accumulator clips print servo state.
         if( !InsideLimitsS16(sv1.errAcc, -sv1.accMax, sv1.accMax) ) // Error accumulator is clipped?
         {
            if( !errAccClipped )                                     // Accumulator clipping just happened?
            { 
               WrStrLiteral("Servo accumulator clipped\r\n");
               showInternalNums(err);                                // then print the state of the servo (just this once)
            }                            
               
            errAccClipped = 1;                                       // and note that acc clipped
         }
         else                                                        // else error accumulator not clipped
            { errAccClipped = 0; }                                   // so clear clip flag
         
         /* Update error accumulator. Limit sum to values which span 1/2 the Actuator range
            See Servo_Reset() above. This limits servo 'windup'.
         */
         sv1.errAcc = ClipS16(AplusBS16(sv1.errAcc, err), -sv1.accMax, sv1.accMax);
         
         // Derive and apply servo to actuator
         Actuator_Write(                                    // Write...                                       
            sv1.cfg.actuator,                               // this Actuator with..
            AplusBS16(                                      // the sum off..
               sv1.cfg.actuatorOfs,                         // the output set point, plus..  
               AplusBS16(                                   // the sum of the proportional and integral errors
                  AmulBdivC_S16(err, sv1.cfg.pGain, _Servo_PGainScale),
                  AmulBdivC_S16(sv1.errAcc, sv1.cfg.iGain, _Servo_IGainScale ))));
               
            #ifdef DBG_SERVO   
         /* If run-display flag is set the show the servo runnng every 3secs. The numbers
            displayed are the direct internal integers; the user will have to translate
            these to units depending on the sensor and Actuator.
         */           
         if( Servo_ShowRun && ResetIfElapsed_S(&dbgTimer, _TicksSec_S(3) ) )
            { showInternalNums(err); }
            #endif            
      }  // DecrU8_NowZero(&runDiv)
      return _sRun;
   }
}


// ----------------- Servo State Machine --------------------------------

#if _TOOL_IS == TOOL_RIDE_8051
PRIVATE U8(const * code stateFuncs[])(S_TinySM_EventList*) =
{
   0,             // no entry function
   state_Stop,
   state_Run
};
#else
PRIVATE U8(* const stateFuncs[])(S_TinySM_EventList*) =
{
   0,             // no entry function
   state_Stop,
   state_Run
};
#endif

PUBLIC S_TinySM ServoPI_StateM;     // Called 'ServoPI' so not to clash with 'Servo_StateM' in Wavesmart

PUBLIC S_TinySM_Cfg CONST ServoPI_StateM_Cfg = 
{
   RECORDS_IN(stateFuncs)-1,        // number of states
   0,                               // no flags - default behaviour
   _TicksSec_S(0.1),                // send tick every (100)msec
   stateFuncs,                      // list of state functions
   initServo
};

// ----------------------------- Interface -------------------------------------


/*--------------------------------------------------------------------------------
|
|  Servo_Stop()
|
|  Suspend the servo state machine
|
-----------------------------------------------------------------------------------*/

PUBLIC void Servo_Stop(void)
{
   TinySM_SendEvent(&ServoPI_StateM, _Servo_Event_Stop);
}

/*--------------------------------------------------------------------------------
|
|  Servo_Run()
|
|  Run the servo state machine
|
-----------------------------------------------------------------------------------*/

PUBLIC void Servo_Run(void)
{
   TinySM_SendEvent(&ServoPI_StateM, _Servo_Event_Run);
}


/*--------------------------------------------------------------------------------
|
|  Servo_IsRunning()
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Servo_IsRunning(void)
{
   return ServoPI_StateM.state == _sRun;
}




// ---------------------------------- eof ----------------------------------------- 
