/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins - Nano version, uses no RAM
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "nano_inpin.h"

/*--------------------------------------------------------------------------------
|
|  InPin_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT InPin_Read(S_InPin CONST *p)
{
   return p->rdPin();
}

PUBLIC S16 InPin_ReadInt(S_InPin CONST *p) { return (S16)InPin_Read(p); }

/*--------------------------------------------------------------------------------
|
|  InPin_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 InPin_Init(S_InPin RAM_IS *p, S_InPin CONST *cfg)
{
   return 1;
}


// -------------------------- eof -----------------------------------------

 
