/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "actuator.h"


/*--------------------------------------------------------------------------------
|
|  Actuator_Init()
|
|  Make S_Actuator 'obj' from S_ActuatorCfg 'init'. Return 1 if init succeeded
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Actuator_Init(S_Actuator RAM_IS *a, S_ActuatorCfg CONST *cfg)
{
   a->cfg = cfg;                    // link to cfg data
   a->locked = 0;                   // start unlocked
   Actuator_WrAlways( a, a->cfg->dflt);  // output to default
   return 1;
}

// ----------------------------- eof ----------------------------------------


 
