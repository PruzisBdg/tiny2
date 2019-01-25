/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (OutPins)
|
|  Public:
|     OutPin_UpdatePin()
|     OutPin_Init()
|     OutPin_Reset()
|     OutPin_Write()
|     OutPin_WriteInt()
|     OutPin_Read()
|     OutPin_ReadInt()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "outpin.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()


/*--------------------------------------------------------------------------------
|
|  OutPin_UpdatePin()
|
|  Set 'p' from 'n'. 'p' is on (set) if n <> 0, else it is off (clear)
|
-----------------------------------------------------------------------------------*/

PUBLIC void OutPin_UpdatePin(S_OutPin RAM_IS *p, U8 n)
{
   if( n ) p->state = 1; else p->state = 0;
   
   if( p->cfg->wrPin )                          // there's a write function? (there should be!!)
      { (*p->cfg->wrPin)(p->state); }           // then run it.
}



/*--------------------------------------------------------------------------------
|
|  OutPin_Init()
|
|  Make S_OutPin 'obj' from 'init' and reset it. Always returns 1 (success)
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 OutPin_Init( T_ObjAddr obj, T_CodeAddr init )
{
   ((S_OutPin*)obj)->cfg = (S_OutPinCfg CONST*)init;
   ((S_OutPin*)obj)->locked = FALSE;
   OutPin_UpdatePin((S_OutPin RAM_IS *)obj, ((S_OutPin*)obj)->cfg->resetState);
   return 1;
}


/*--------------------------------------------------------------------------------
|
|  OutPin_Reset()
|
|  If 'p' is not locked then reset it. Returns 0 if 'p' could not be reset.
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPin_Reset( S_OutPin RAM_IS *p )
{
   if( p->locked )
   {
      return 0;
   }
   else
   {
      OutPin_UpdatePin(p, 0);
      return 1;
   }
}


/*--------------------------------------------------------------------------------
|
|  OutPin_Write()
|
|  If 'p' is not locked then set its state from 'n'. If 'n' == 0 then the pin is off
|  (clear) else its is on (set)
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPin_Write( S_OutPin RAM_IS *p, U8 n )
{
   if( p->locked )
   {
      return 0;
   }
   else
   {
      OutPin_UpdatePin(p, n);
      return 1;
   }
}

/*--------------------------------------------------------------------------------
|
|  OutPin_WriteInt()
|
|  Same as OutPin_Write() but follows the call model for Class write functions
|
-----------------------------------------------------------------------------------*/

PUBLIC void OutPin_WriteInt( S_OutPin RAM_IS *p, S16 n ) { OutPin_Write(p, (U8)n); }


/*--------------------------------------------------------------------------------
|
|  OutPin_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPin_Read( S_OutPin RAM_IS *p )
{
   return p->state;
}

/*--------------------------------------------------------------------------------
|
|  OutPin_ReadInt()
|
|  Same as OutPin_Read() but follows the call model for Class read functions
|
-----------------------------------------------------------------------------------*/

PUBLIC S16 OutPin_ReadInt( S_OutPin RAM_IS *p ) { return (S16)OutPin_Read(p); }


// --------------------------------- eof -------------------------------------------






 
