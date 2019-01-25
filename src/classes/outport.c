/*---------------------------------------------------------------------------
|
|                    Tiny Lib - 16-wide output port
|
|  Public:
|     OutPort_Update()
|     OutPort_Init()
|     OutPort_Reset()
|     OutPort_Write()
|     OutPort_Read()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "outport.h"
#include "hostcomms.h"
#include "wordlist.h"
#include "tty_ui.h"     // UI_PrintScalar()


/*--------------------------------------------------------------------------------
|
|  OutPort_Update()
|
|  Set the current port state to 'n' and write to the port.
|
-----------------------------------------------------------------------------------*/

PUBLIC void OutPort_Update(S_OutPort *p, U16 n)
{
   p->state = n;
   
   if( p->cfg->wrPin )                          // there's a write function? (there should be!!)
      { (*p->cfg->wrPin)(p->state); }           // then run it.
}



/*--------------------------------------------------------------------------------
|
|  OutPort_Init()
|
|  Make S_OutPort 'obj' from 'init' and reset it. Always returns 1 (success)
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 OutPort_Init( T_ObjAddr obj, T_CodeAddr init )
{
   ((S_OutPort*)obj)->cfg = (S_OutPortCfg CONST*)init;
   ((S_OutPort*)obj)->locked = FALSE;
   OutPort_Update((S_OutPort*)obj, ((S_OutPort*)obj)->cfg->resetState);
   return 1;
}


/*--------------------------------------------------------------------------------
|
|  OutPort_Reset()
|
|  If 'p' is not locked then reset it. Returns 0 if 'p' could not be reset.
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPort_Reset( S_OutPort *p )
{
   if( p->locked )
   {
      return 0;
   }
   else
   {
      OutPort_Update(p, p->cfg->resetState);
      return 1;
   }
}


/*--------------------------------------------------------------------------------
|
|  OutPort_Write()
|
|  If 'p' is not locked then set its state from 'n'.
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPort_Write( S_OutPort *p, U16 n )
{
   if( p->locked )
   {
      return 0;
   }
   else
   {
      OutPort_Update(p, n);
      return 1;
   }
}

/*--------------------------------------------------------------------------------
|
|  OutPort_Read()
|
-----------------------------------------------------------------------------------*/

PUBLIC U16 OutPort_Read( S_OutPort *p )
{
   return p->state;
}

// --------------------------------- eof -------------------------------------------






 
