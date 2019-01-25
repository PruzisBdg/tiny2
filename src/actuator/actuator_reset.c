/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|  Pruzina 11/02/05
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "actuator.h"


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


 
