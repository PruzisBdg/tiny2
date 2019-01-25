/*---------------------------------------------------------------------------
|
|                  Tiny Lib - Sensors
|
|  This supports Sensors, one of the Tiny1 classes.
|
|--------------------------------------------------------------------------*/

#ifndef SENSOR_H
#define SENSOR_H

#include "common.h"
#include "sysobj.h" 

#if USE_NANO_CAL == 1
   #include "nano_cal.h"
#else   
   #include "cal.h"
#endif

typedef union
{
   #if USE_NANO_CAL == 1
   S_NanoCal CONST *asCal;
   #else
   S_Cal RAM_IS *asCal;
   #endif
   
   S16           asNum;
} U_CalOrConst;

typedef struct
{
   U_CalOrConst  val;
   U8            isCal;
} S_CalOrConst;

#define _CalOrConst_IsAConst 0
#define _CalOrConst_IsACal   1

typedef struct
{
   S_CalOrConst   ofs,     // offset
                  num,     // numerator
                  den;     // denominator
} S_SensorScale;

// Short config saves space
typedef struct
{
   S16      (*read)(void);       // signal source function
   U8       flags;               // must be 0 to indicate a short cfg        
} S_SensorCfgShort; 

// Longer config specifies gain and offest
typedef struct
{
   S16      (*read)(void);       // signal source function
   U8       flags;               // non-zero indicates a 'full' cfg
   S_SensorScale scale;
} S_SensorCfgFull;

#define _SensorCfg_IsFullCfg           _BitM(0)    // Set at least this to use the full config
#define _SensorCfg_SubtractOfsPostGain _BitM(1)
#define _SensorCfg_RawIsUnsigned       _BitM(2)

typedef union
{
   S_SensorCfgShort shrt;
   S_SensorCfgFull full;
} U_SensorCfg;

typedef struct
{
   U_SensorCfg CONST *cfg;
   U8 flags;
} S_Sensor;

PUBLIC U8 Sensor_Init(T_ObjAddr obj, T_CodeAddr init);
PUBLIC S16 Sensor_Read(S_Sensor RAM_IS *s);


#endif // SENSOR_H
