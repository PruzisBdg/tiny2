/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#ifndef FLASHBLK_H
#define FLASHBLK_H

#include "common.h"
#include "flash_basic.h"
#include "sysobj.h"

typedef U8  T_FlashBlk_Handle;
#define _FlashBlk_MaxHdl 100


PUBLIC U8   FlashBlk_Init( T_ObjAddr obj, T_CodeAddr init );
PUBLIC void FlashBlk_Clear( void );
PUBLIC U8   FlashBlk_Write( U8 RAM_IS *src, T_FlashBlk_Handle hdl, U8 cnt );
PUBLIC U8   FlashBlk_MatchVar( U8 *src, T_FlashBlk_Handle hdl, U8 cnt );
PUBLIC void FlashBlk_Delete( T_FlashBlk_Handle hdl );
PUBLIC U8   FlashBlk_Read( U8 RAM_IS *out, T_FlashBlk_Handle hdl, U8 cnt );
PUBLIC U8   FlashBlk_ReadOfs( U8 RAM_IS *out, T_FlashBlk_Handle hdl, U8 maxRead, U8 ofs );
PUBLIC U16  FlashBlk_BytesFree( void );
PUBLIC U8   FlashBlk_UI( C8 *args );
PUBLIC U8   FlashBlk_VarExists( T_FlashBlk_Handle hdl );
PUBLIC U8   FlashBlk_VarCnt( void );
PUBLIC U16  FlashBlk_BankCapacity(void);

PUBLIC T_FlashBlk_Handle FlashBlk_GetFreeHdl(T_FlashBlk_Handle min, T_FlashBlk_Handle max );

/* ------------------------ FlashBlk_ Specification -------------------------------

   Start address of each flash page and page size. Also the code bank containng the 
   Flash block. These must be defined externally
*/
extern T_FlashAddr FlashBlk_PageA(void);
extern T_FlashAddr FlashBlk_PageB(void);
extern U16         FlashBlk_PageSize(void);
extern U8          FlashBlk_GetCodeBank(void);
extern U8          FlashBlk_GetCh(T_FlashAddr p);

#define FlashBlk_GetHdl FlashBlk_GetCh


/* ----------------------- For the tiny block store -------------------------------

   When there are only a few items to store.
*/

typedef struct
{
   T_FlashBlk_Handle hdl;        // it's handle     
   U8                size,       // it's size
                     ofs;        // offset from the start of the block
} S_FlashBlk_Item;




#endif // FLASHBLK_H
