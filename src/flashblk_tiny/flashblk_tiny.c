/*---------------------------------------------------------------------------
|
|                 A small Flash store for multiple items, 
|
|  Items in the store are accessed by handle. They may be read and written 
|  individually.
|
|  The underlying store is a block of Flash up to 256 bytes. Items are packed
|  in the block in order of increasing handle. Items may be of different sizes.
|
|  Because of the packed structure, items may be appended to the stored structure
|  provided their handles are larger than those of the existing contents.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblock.h"
#include "flashblk.h"
#include "arith.h"

extern S_FlashBlk_Item CONST FlashBlock_Items[];    // List of contents, in order of increasing handle
extern U8 FlashBlock_NumItems(void);               // Size of said list

#define _HdlNotFound_0xFF 0xFF


/*-----------------------------------------------------------------------------------
|
|  getItem()
|
--------------------------------------------------------------------------------------*/

PRIVATE U8 getItemIdx(T_FlashBlk_Handle hdl)
{
   U8 c;
   
   for( c = 0; c < FlashBlock_NumItems(); c++ )       // For each item listed in FlashBlock_Items
   {
      if( FlashBlock_Items[c].hdl == hdl )            // Handle matches requested?
         { return c; }                                // then return offset to this item in the block
   }
   return _HdlNotFound_0xFF;                          // there was no match to 'hdl'
}


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Read()
|
|  Read up to 'maxRead' bytes to 'out'
|
|  PARAMETERS:    'out'       - location 
|                 'hdl'       - the handle of the data to be read
|                 'maxRead'   - number of bytes to read
|
|  RETURNS:       number of bytes read.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_Read( U8 RAM_IS *out, T_FlashBlk_Handle hdl, U8 maxRead )
{
   U8 idx;

   if( (idx = getItemIdx(hdl)) == _HdlNotFound_0xFF )
      { return 0; }
   else
   {
      return FlashBlock_Read(out, FlashBlock_Items[idx].ofs, MinU8(maxRead, FlashBlock_Items[idx].size));
   }
}



/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Write()
|
|  Write data to the store, labelled with 'hdl'. If an item 'hdl' exists in the store
|  then and the data to be written is different from whats in the store then overwrite 
|  with the new data.
|
|  PARAMETERS:    'src'    - address of data to be written
|                 'cnt'    - number of bytes from 'src' to be written
|                 'hdl'    - the handle a the data to be written
|
|  RETURNS:       TRUE if the data was successfully written.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_Write( U8 RAM_IS *src, T_FlashBlk_Handle hdl, U8 cnt )
{
   U8 idx;

   if( (idx = getItemIdx(hdl)) == _HdlNotFound_0xFF )
      { return 0; }
   else
   {
      return FlashBlock_Write(src, FlashBlock_Items[idx].ofs, MinU8(cnt, FlashBlock_Items[idx].size));
   }
}

// ----------------------------- eof ---------------------------------------
 
