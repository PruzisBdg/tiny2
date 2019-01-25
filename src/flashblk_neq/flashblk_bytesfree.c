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
|  FlashBlk_BytesFree()
|
|  Return the number of free bytes in the store.
|
--------------------------------------------------------------------------------------*/

PUBLIC U16 FlashBlk_BytesFree( void )
{
   T_FlashAddr p;
   U16         freeCnt;

   for( p = currentBank(), freeCnt = 0; p < bankEnd(); p += _SlotSize )
   {
      if( !slotHasData(p) )
      {
         freeCnt += _SlotDataBytes;
      }
   }
   return freeCnt;
}

// -------------------------------- eof ----------------------------------------
