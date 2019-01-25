/*---------------------------------------------------------------------------
|
|                Tiny Lib - Analog outputs (Actuators)
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "actuator.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()
#include <stdio.h>
#include "arith.h"


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

// ----------------------------- eof ----------------------------------------


 
