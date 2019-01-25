/* -------------------------------------------------------------------------
|
|                 A Single RAM-shaowed Flash Block
----------------------------------------------------------------------------- */

#ifndef FLASHBLOCK_H
#define FLASHBLOCK_H

#include "flash_basic.h"

typedef struct
{
   T_FlashAddr addr;       // Where the block is
   U8          size;       // How many bytes
   U8 RAM_IS *shadow;    // Where the RAM shadow is
} S_FlashBlock;

PUBLIC U8 FlashBlock_Read(U8 RAM_IS *out, U8 ofs, U8 cnt);
PUBLIC BIT FlashBlock_Write(U8 RAM_IS *src, U8 ofs, U8 cnt);
PUBLIC BIT FlashBlock_WriteAll(void);

#endif

 
