/*---------------------------------------------------------------------------
|
|         Tiny Lib  TTY Interface for Proportional-Integral Servos
|
|  A servo links a Sensor and an Actuator. This servo has input and output
|  setpoints and separate proportional and integral gains.
|
|  Public:
|     Servo_UIInit()
|     UI_Servo()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "servo.h"
#include "tiny_statem.h"
#include "sensor.h"
#include "actuator.h"
#include "util.h"
#include "arith.h"
#include "class.h"         // _Class_Sensor, _Class_Actuator
#include "tty_ui.h"        // UI_GetObj_MsgIfFail
#include "sysobj.h"
#include "wordlist.h"
#include "romstring.h"


// ============================== Private ================================

// Make 'ui' public to all these functions. Saves code not passing it as a parm.
PRIVATE S_ServoUI *ui;

/*--------------------------------------------------------------------------------
|
|  servoMade()
|
|  Return 1 if the servo has both s Sensor and an Actuator.
|
-----------------------------------------------------------------------------------*/

PRIVATE BIT servoMade(void)
{
   if( ui->sensorObj && ui->actuatorObj )
      { return 1; }
   else
   {
      WrStrLiteral("Servo not made\r\n");
      return 0;
   }
}

// Utility functions - shrink the code
PRIVATE C8 CONST * sensorName(void) { return ui->sensorObj->name; }
PRIVATE C8 CONST * actuatorName(void) { return ui->actuatorObj->name; }

/* Get sensor and actuator output scales; used to scale the gains and input
   scales.
*/
PRIVATE float getOutScale(C8 CONST * name) { return GetObjIO(_StrConst(name))->outScale; }
PRIVATE float getSensorScale(void) {return getOutScale(sensorName()); }
PRIVATE float getActuatorScale(void) {return getOutScale(actuatorName()); }

/* Derive the output scale for proportional and integral gains

                                 sensor_outScale
      gain_scale =  -------------------------------------------
                    error_acc_internal_gain * actuator_outScale
*/
PRIVATE float getGainOutScale(void)  { return  getSensorScale() / (_Servo_PGainScale * getActuatorScale()); }


/* Read input and output setpoints from the element at 'pos' in the space-delimited
   argument list 'args'. The input set point is scaled by the IO scale of the sensor
   which has been associated with the servo; the output set point is scaled from the
   actuator io scale.
*/
PRIVATE void getInPoint( C8 *args, U8 pos)
   { ui->theServo.cfg.setPoint = UI_GetScalarArg(args, pos, getSensorScale() ); }

PRIVATE void getOutPoint( C8 *args, U8 pos)
   { ui->theServo.cfg.actuatorOfs = UI_GetScalarArg(args, pos, getActuatorScale() ); }


// Apply servo gain scale factor to 'gain'
PRIVATE float scaleGainOut(S16 gain) { return gain * getGainOutScale(); }

// ============================== end: Private ================================

PUBLIC BIT Servo_ShowRun;

/*--------------------------------------------------------------------------------
|
|  Servo_UIInit()
|
|  Make S_Servo 'obj' from S_Servo 'init'. Return 1 (success)
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Servo_UIInit(S_ServoUI *obj, S_ServoCfg CONST *init)
{
   obj->sensorObj = 0;
   obj->actuatorObj = 0;
   Servo_Init(&obj->theServo, init);
   Servo_ShowRun = 0;
   return 1;
}


/*--------------------------------------------------------------------------------
|
|  UI_Servo()
|
|  Run the servo UI.
|
-----------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST UI_Servo_Help[]  =
"Usage:\r\n\
    <servo name> <action =\r\n\
    (\r\n\
    'make' <sensor name> <actuator name> [input set point] [output set point] [proportional gain] [integral gain] | []runIntvl\r\n\
    'run' | 'stop' |\r\n\
    'inPoint'  <input set point> |\r\n\
    'outPoint' <output set point> |\r\n\
    'pgain'    <proportional gain> |\r\n\
    'igain'    <integral gain> |\r\n\
    'report'\r\n\
    'showRun'\r\n\
    )\r\n";
   #endif // INCLUDE_HELP_TEXT

PRIVATE C8 CONST actionList[] = "make run stop inPoint outPoint pgain igain runIntvl report showRun";
typedef enum { action_Make, action_Run, action_Stop,
   action_InPoint, action_OutPoint, action_PGain, action_IGain, action_RunIntvl, action_Report, action_ShowRun } E_Actions;

PUBLIC U8 UI_Servo(C8 *args)
{
   S_Obj CONST *obj;
   S_Servo     *sv;
   U8          action;
   U8          n;

   if( NULL != (obj = UI_GetObj_MsgIfFail(args, _Class_Servo)) )
   {
      if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
         { return 0; }
      else
      {
         ui = (S_ServoUI*)obj->addr;
         sv = &ui->theServo;

         switch(action)
         {
            /* 'Make' a servo. At least the sensor and actuator must be specified. Some or all of the
               other 4 parameters may be supplied; if a parm is missing the value for that parm is set
               zero.

               Syntax is:  <required> [optional]
                  'servo' <servo_name> 'make'
                      <sensor_name> <actuator_name>
                          [input setpoint] [output setpoint] [proportional gain] [integral gain]
            */
            case action_Make:
               if(
                   ( (ui->sensorObj   = GetObjByClass( Str_GetNthWord(args, 2), _Class_Sensor )) == 0) ||
                   ( (ui->actuatorObj = GetObjByClass( Str_GetNthWord(args, 3), _Class_Actuator )) == 0)
                 )
               {
                  WrStrLiteral("Make failed\r\n");
               }
               else
               {
                  sv->cfg.sensor = (S_Sensor*)ui->sensorObj->addr;
                  sv->cfg.actuator = (S_Actuator*)ui->actuatorObj->addr;
                  getInPoint(args, 4);
                  getOutPoint(args, 5);
                  sv->cfg.pGain = UI_GetScalarArg(args, 6, getGainOutScale() );
                  sv->cfg.iGain = UI_GetScalarArg(args, 7, getGainOutScale() );

                  n = UI_GetScalarArg(args, 8, 0);
                  sv->cfg.runIntvl = n == 0 ? 1 : n;
               }
               Servo_Reset(sv);     // reset the error accumulator. (Re)generate the backlash lmits from the new settings
               break;

            case action_Run:
               if(servoMade())
                  { Servo_Run(); }
               break;

            case action_Stop:
               if(servoMade())
                  { Servo_Stop(); Servo_ShowRun = 0; }
               break;

            case action_InPoint:
               if(servoMade())
                  { getInPoint(args, 2); }
               break;

            case action_OutPoint:
               if(servoMade())
                  { getOutPoint(args, 2); }
               break;

            // Set proportional gain
            //    'servo' <servo name> 'pgain' <gain to set>
            case action_PGain:
               sv->cfg.pGain = UI_GetScalarArg(args, 2, getGainOutScale() );
               break;

            // Set integral gain
            //    'servo' <servo name> 'igain' <gain to set>
            case action_IGain:
               sv->cfg.iGain = UI_GetScalarArg(args, 2, getGainOutScale() );
               break;

            // Set run Interval in units of 100msec
            //    'servo' <servo name> 'runIntvl' <run interval to set>
            case action_RunIntvl:
               sv->cfg.runIntvl = UI_GetScalarArg(args, 2, 0 );
               break;

            /* Print out servo status. Lists:
                  - the sensor and actuator
                  - gains and setpoints
                  - current sensor and actuator readings
                  - whether the servo is stopped or running
            */
            case action_Report:
               if( servoMade() )
               {
                  sprintf(PrintBuf.buf, "sensor = %s actuator = %s\r\n   inSetPt = ",
                     sensorName(),
                     actuatorName()
                     );
                  PrintBuffer();

                  UI_PrintScalar(sv->cfg.setPoint, GetObjIO(_StrConst(sensorName())), _UI_PrintScalar_AppendUnits);
                  WrStrLiteral(" outSetPt = " );
                  UI_PrintScalar(sv->cfg.actuatorOfs, GetObjIO(_StrConst(actuatorName())), _UI_PrintScalar_AppendUnits);

                  sprintf(PrintBuf.buf, " pGain = %3.1f iGain = %3.1f %s/%s\r\n   running = %d ",
                     scaleGainOut(sv->cfg.pGain),
                     scaleGainOut(sv->cfg.iGain),
                     GetObjIO(_StrConst( sensorName()))->units,
                     GetObjIO(_StrConst( actuatorName()))->units,
                     (S16)Servo_IsRunning()
                     );
                  PrintBuffer();
                  UI_PrintObject(ui->sensorObj, _UI_PrintObject_PrependName | _UI_PrintObject_AppendUnits );
                  WrStrLiteral("  ");
                  UI_PrintObject(ui->actuatorObj, _UI_PrintObject_PrependName | _UI_PrintObject_AppendUnits );
                  WrStrLiteral("\r\n");
               }
               break;

            case action_ShowRun:
               Servo_ShowRun = 1;
               break;

            default:
               break;
         }
      }
      return 1;
   }
   return 1;
}

// ---------------------------------------- eof -----------------------------------------
