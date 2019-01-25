/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_outpin.h"
#include "nano_outpin_priv.h"

/*--------------------------------------------------------------------------------
|
|  OutPin_Write()
|
|  If 'p' is not locked then set its state from 'n'. If 'n' == 0 then the pin is off
|  (clear) else its is on (set)
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPin_Write( S_OutPin CONST *p, U8 n )
{
   if( locked(p) )
   {
      return 0;
   }
   else
   {
      OutPin_UpdatePin(p, n);
      return 1;
   }
}

// --------------------------------- eof -------------------------------------------






 
