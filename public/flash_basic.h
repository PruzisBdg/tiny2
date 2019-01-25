/*---------------------------------------------------------------------------
|
|               Basic Flash support
|
|  Currently there's support for Silabs C8051xx and Zilog Z8
|
|--------------------------------------------------------------------------*/

#ifndef FLASH_BASIC_H
#define FLASH_BASIC_H

#include "libs_support.h"
#include "common.h"

#ifndef CODE_SPACE_IS
   #error "CODE_SPACE_IS must be defined"
#else
    #if CODE_SPACE_IS == CODE_SPACE_20BIT
        typedef U32 T_FlashAddr;
    #else
        #error "CODE_SPACE_IS must be defined to determine T_FlashAddr"
    #endif
#endif

PUBLIC U8 Flash_WrByte( T_FlashAddr addr, U8 theByte );
PUBLIC U8 Flash_WrBlock( U8 RAM_IS *buf, T_FlashAddr flashAddr, U8 cnt );
PUBLIC U8 Flash_Erase( T_FlashAddr addr, U8 sectors );
PUBLIC void Flash_SetBank(U8 bank);       // for C8051F12xx multi-bank devices

// For the scratchpad Flash
PUBLIC U8  Scratchpad_RdByte( T_FlashAddr addr);
PUBLIC BIT Scratchpad_WrByte( T_FlashAddr addr, U8 theByte );
PUBLIC BIT Scratchpad_WrBlock( U8 const *buf, T_FlashAddr flashAddr, U8 cnt );
PUBLIC BIT Scratchpad_Erase( void );


/* Optional interlock to prevent another application from illegally writing Flash.

   To use this lock, link the 'lock' version of the flash_basic library then declare
   
      'PUBLIC U8 IDATA Flash_Lock;'
      
   in the Host application. Set to 'Flash_LockValid' to allow writes. Clear it to
   zero when done.
*/
#define Flash_LockValid 0x38



#endif // FLASH_BASIC_H


