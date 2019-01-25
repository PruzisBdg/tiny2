/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblk.h"

extern T_FlashAddr currentBank_flashBlk( void );
#define currentBank currentBank_flashBlk

extern T_FlashAddr bankEnd_flashBlk( void );
#define bankEnd bankEnd_flashBlk

#define _SlotDataBytes  2                                               // block holds an integer
#define _SlotSize       (_SlotDataBytes + sizeof(T_FlashBlk_Handle))    // data bytes plus handle

extern BIT slotHasData_flashBlk( T_FlashAddr s );
#define slotHasData slotHasData_flashBlk


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_VarCnt()
|
|  Return the number of variables in the store. This is done by counting the number
|  of different handles. The count assumes that where variables use multiple slots,
|  that these slots are not interleaved.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_VarCnt( void )
{
   T_FlashAddr p;

   U8 varCnt,
      currHdl;

   for( p = currentBank(), varCnt = 0, currHdl = 0; p < bankEnd(); p += _SlotSize )
   {
      if( slotHasData(p) )                      // data in this slot?
      {
         if( FlashBlk_GetHdl(p) != currHdl )         // a new handle too?
         {
            varCnt++;                           // then its a new variable - count it.
            currHdl = *(U8 CODE *)p;            // and note the latest handle
         }
      }
   }
   return varCnt;
}

// ----------------------------------- eof ------------------------------------------- 
