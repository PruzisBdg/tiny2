/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|  Public:
|     Actuator_WrAlways()
|     Actuator_Init()
|     Actuator_Write()
|     Actuator_Read()
|     Actuator_Reset()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "actuator.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()

#include "arith.h"


/*--------------------------------------------------------------------------------
|
|  Actuator_WrAlways()
|
|  Write Actuator, bypassing the lock. 
|
-----------------------------------------------------------------------------------*/

PUBLIC void Actuator_WrAlways(S_Actuator RAM_IS *a, S16 n)
{
   a->value = ClipS16(n, a->cfg->min, a->cfg->max);    // Clip new value to current limits

   if( a->cfg->write )                    // there's a write function? (there should be!!)
      { (*a->cfg->write)(a->value); }     // then run it.
}



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


/*--------------------------------------------------------------------------------
|
|  Actuator_Write()
|
|  Set actuator to 'n', provided it's not locked.
|
|  Return 0 if locked; else 1;
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT Actuator_Write(S_Actuator RAM_IS *a, S16 n)
{
   if( a->locked )
   {
      return 0;
   }
   else
   {
      Actuator_WrAlways(a, n);
      return 1;
   }
}


/*--------------------------------------------------------------------------------
|
|  Actuator_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC S16 Actuator_Read(S_Actuator RAM_IS *a)
{
   return a->value;
}



/*--------------------------------------------------------------------------------
|
|  Actuator_Reset()
|
|  Reset actuator to default, provided it's not locked.
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Actuator_Reset(S_Actuator RAM_IS *a)
{
   if( a->locked )
   {
      return 0;
   }
   else
   {
      Actuator_WrAlways(a, a->cfg->dflt);
      return 1;
   }
}



// ----------------------------- eof ----------------------------------------


 
