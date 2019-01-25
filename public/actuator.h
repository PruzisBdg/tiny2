/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|--------------------------------------------------------------------------*/

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "common.h"
#include "sysobj.h" 

typedef struct
{
   void (*write)(S16 n);
   S16   dflt,
         min, max;
} S_ActuatorCfg; 

typedef struct
{
   S_ActuatorCfg CONST *cfg;
   U8    locked;
   S16   value;
} S_Actuator;

PUBLIC U8 Actuator_Init(S_Actuator RAM_IS *a, S_ActuatorCfg CONST *cfg);
PUBLIC BIT Actuator_Write(S_Actuator RAM_IS *s, S16 n);
PUBLIC S16 Actuator_Read(S_Actuator RAM_IS *a);
PUBLIC U8 Actuator_Reset(S_Actuator RAM_IS *a);
PUBLIC void Actuator_WrAlways(S_Actuator RAM_IS *a, S16 n);

#endif // ACTUATOR_H


 
