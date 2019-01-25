/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins
|
|  Public:
|     InPin_Read()
|     InPin_ReadInt()
|     InPin_Init()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "inpin.h"
#include "sysobj.h"
#include "class.h"
#include "wordlist.h"
#include "tty_ui.h"


/*--------------------------------------------------------------------------------
|
|  InPin_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT InPin_Read(S_InPin RAM_IS *p)
{
   return p->cfg->rdPin();
}

/*--------------------------------------------------------------------------------
|
|  InPin_ReadInt()
|
|  Same as InPin_Read() but follows the call model for Class read functions
|
-----------------------------------------------------------------------------------*/

PUBLIC S16 InPin_ReadInt(S_InPin RAM_IS *p) { return (S16)InPin_Read(p); }

/*--------------------------------------------------------------------------------
|
|  InPin_Init()
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 InPin_Init(S_InPin RAM_IS *p, S_InPinCfg CONST *cfg)
{
   p->cfg = cfg;
   return 1;
}


// -------------------------- eof -----------------------------------------

 
