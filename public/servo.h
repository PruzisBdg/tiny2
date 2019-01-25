/*---------------------------------------------------------------------------
|
|                  Tiny Lib  Proportional-Integral Servos
|
|  A servo links a Sensor and an Actuator. This servo has input and output
|  setpoints and separate proportional and integral gains.
|
|--------------------------------------------------------------------------*/

#ifndef SERVO_H
#define SERVO_H

#include "common.h"
#include "sensor.h"
#include "actuator.h"
#include "sysobj.h"

// Defines a servo
typedef struct
{
   S_Sensor   *sensor;        // reads from one sensor
   S_Actuator *actuator;      // output to one actuator
   S16        setPoint,       // input setpoint
              actuatorOfs,    // output operating point. With zero error the servo centres here
              pGain,          // proportional gain
              iGain;          // integral gain
   U8         runIntvl;       // Run interval in multiples of 100msec
} S_ServoCfg;

// A servo
typedef struct
{
   S_ServoCfg  cfg;           // contains the configuration above
   S16         errAcc,        // integral error accumulator
               accMax;        // backlash limits (are symmetrical about zero)
} S_Servo;

// Servo text user interface
typedef struct
{
   S_Servo        theServo;      // contains a servo
   S_Obj CONST    *sensorObj;    // and can access the sensor and actuator OBJECTS...
   S_Obj CONST    *actuatorObj;  // ... this for names and numeric scaling for the tty interface
} S_ServoUI;

PUBLIC void Servo_Stop(void);
PUBLIC void Servo_Run(void);
PUBLIC BIT Servo_IsRunning(void);
PUBLIC void Servo_Reset(S_Servo *sv);
PUBLIC void Servo_Init(S_Servo *sv, S_ServoCfg CONST *init);

// Enable printouts showing the servo running.
extern BIT Servo_ShowRun;

// Servo runs 10/sec
#define _ServoRunIntvl_sec 0.1

/* Internal scale for proportional and integral gains

   Typical proporional gain settings might be 0.5 - 2.0. Choose _PGainScale for a 
   resasonable resolution of typical proportional gain settings. If the scale is
   too large then it will crimp the range of the integral gain term.
   
   The intergral gain scale set so that:
            integral_gain_units = propotional_gain_units / 1sec

   The integral error accumulator is S16 (+/-32767). With the constants below, and with 
   an input error of 1000 counts and a pGain setting of 1.0, the error accumulator will 
   saturate in:
          32767 / (10 * 1.0 * 20.0 * 1000) = 1.5 secs.       
          
   Make sure that the error accumulator can null the worst case proportional error. With
   a typical integral gain of 0.5, the error accumulator can null:
          32767 * 20.0/ (20.0/0.1) = 32767 * 20 / 2000  = 3276.                       
*/
#define _Servo_PGainScale  20.0
#define _Servo_IGainScale  (_Servo_PGainScale/_ServoRunIntvl_sec)

#endif 

// ----------------------------------- eof ----------------------------------------
