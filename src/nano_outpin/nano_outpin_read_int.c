/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
|
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_outpin.h"
#include "nano_outpin_priv.h"

/*--------------------------------------------------------------------------------
|
|  OutPin_ReadInt()
|
|  Same as OutPin_Read() but follows the call model for Class read functions
|
-----------------------------------------------------------------------------------*/

PUBLIC S16 OutPin_ReadInt( S_OutPin CONST *p ) { return (S16)OutPin_Read(p); }


// --------------------------------- eof -------------------------------------------






 
