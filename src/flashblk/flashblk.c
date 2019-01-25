/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|  This module stores multiple variables of different sizes in Flash on the Silabs
|  C8051xx series of MCUs. Each variable is accessed via a unique handle. Variables
|  may be independently added, modified and erased from the store. The store holds
|  a few hundred bytes, enough for calibrations for a typical instrument.
|
|  Bytes in Flash may be programmed individually but Flash is erased by the block.
|  This store uses 2 512 byte blocks. At any time one block is the active block.
|  Adding or modifying variables uses up the flash in this current block. Once all
|  flash is used up the data in the this block is 'compacted' into the other one.
|  The lastest versions of each variable are copied across, leaving older versions
|  or 'deleted' variables behind. The new block is marked as the current one for
|  writes and reads while original block is erased. 
|
|  This 2 block method avoids a large RAM buffer which would be needed to cache
|  data while a single full flash block was being cleared and compacted.
|
|  Each flash block is divided into fixed-size slots. Each slot holds a (byte)
|  handle and several bytes data. Each item in the store occupies as many slots
|  as needed to hold its bytes. Each slot is labelled with the handle of the
|  item. To delete an item from the store its slot handles are written to zero.
|  To modify an item, its existing value is erased and the revised value is
|  written to a 'clean' area in the current flash block.
|
|     
|                              Store
|                                |
|                        ----------------------
|                       |                      |
|                    block A (512 bytes)   block B (512 bytes)
|                       |
|                       |
|                ------------------------
|                |                      |
|             'current' tag          slot 0 
|                                    slot 1
|                                       .
|                                       .
|                                    slot n
|                                       |
|                             ----------------------
|                             |                    |
|                       handle (byte)          data[0..n]
|
|
|  This is a basic data store. Data is not checksummed or labelled with size. The calling 
|  process either knows the size of the data or embeds its with whats written to the store.
|  Same with checksum.
|  
|
|  Public functions:
|
|     FlashBlk_Init()
|     FlashBlk_BytesFree()
|     FlashBlk_Read()
|     FlashBlk_Write()
|     FlashBlk_Delete()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flash_basic.h"
#include "flashblk.h"
#include "wordlist.h"

#include "hostcomms.h"
#include "svc.h"

//#define DBG_FLASHBLK

/* ---------------------------- Store Specs ---------------------------------

   The store uses 2 x 512 byte blocks. Each slot is a byte handle plus 2 bytes 
   data. This is the best slot size for storing individual integer calibrations. 
   With this formatting the store will hold about 330 bytes or 160 calibration
   numbers.
*/

// Each page consists of a (1 byte) tag and a data area. The tag is at the start
// of the page; the data area is the remainder of the page.
typedef U8 T_BankTag;

// Given the start of a data bank within a page, returns the address of the tag
// for that data bank.
#define _TagAddrFromBank(bankAddr)  ((bankAddr)- sizeof(T_BankTag))

// The rest of the page is for data storage area (the 'bank')
#define _BankBytes       (FlashBlk_PageSize() - sizeof(T_BankTag))

#define _SlotDataBytes  2                                               // Each slot holds an integer
#define _SlotBytes      (_SlotDataBytes + sizeof(T_FlashBlk_Handle))    // A slot has data bytes plus handle
#define _ErasedHandle   0xFF
#define _BankCapacity   (_BankBytes / _SlotBytes)                       // Number of data items the bank will hold

// ------------------------- end: store specs -------------------------------


// ============================== PRIVATE ==========================================

/*-----------------------------------------------------------------------------------
|
|  bankA/B()
|
|  The return the start of the data area within the page.
|
--------------------------------------------------------------------------------------*/

PRIVATE T_FlashAddr bankA(void) {return FlashBlk_PageA() + sizeof(T_BankTag);}
PRIVATE T_FlashAddr bankB(void) {return FlashBlk_PageB() + sizeof(T_BankTag);}


/*-----------------------------------------------------------------------------------
|
|  tagFromPage
|
|  Given the start address of a flash page, return the tag for the data store within
|  that page.
|
--------------------------------------------------------------------------------------*/

PRIVATE T_BankTag tagFromPage( T_FlashAddr pageAddr ) 
{ 
	return FlashBlk_GetCh(pageAddr);		// Rem: must use FlashBlk_GetCh() to read from correct Flash bank.
}


/*-----------------------------------------------------------------------------------
|
|  legalHandle()
|
|  Return 1 if 'hdl' is a legal store handle, else 0.
|
--------------------------------------------------------------------------------------*/
#if 0
PRIVATE U8 legalHandle( T_FlashBlk_Handle hdl )
{
   return (hdl > 0x00) && (hdl < 0xFF);
}
#endif

/*-----------------------------------------------------------------------------------
|
|  slotHasData_flashBlk()
|
|  Returns 1 if slot whose handle is address 's' has legal data, else zero
|
--------------------------------------------------------------------------------------*/

PUBLIC BIT slotHasData_flashBlk( T_FlashAddr s )
{
   // If neither untouched or marked deleted then must have data.
   return (FlashBlk_GetHdl(s) != 0x00) && (FlashBlk_GetHdl(s) != 0xFF);
}
#define slotHasData slotHasData_flashBlk


/*-----------------------------------------------------------------------------------
|
|  currentBank_flashBlk()
|
|  Returns start address of the currently active bank, 0 if the Flash store has not 
|  been initialised.
|
--------------------------------------------------------------------------------------*/

PUBLIC T_FlashAddr currentBank_flashBlk( void )
{
   if( tagFromPage(FlashBlk_PageA()) == 0 )           // used tag of store A == zero?
      { return bankA(); }                             // then store A is current
   else if( tagFromPage(FlashBlk_PageB()) == 0 )      // else store B tag == 0?
      { return bankB(); }                             // so store B is current
   else                                               // else there is no current store
      { return 0; }
}
#define currentBank currentBank_flashBlk



/*-----------------------------------------------------------------------------------
|
|  bankEnd_flashBlk()
|
|  Returns the last address of the current bank
|
--------------------------------------------------------------------------------------*/

PUBLIC T_FlashAddr bankEnd_flashBlk( void )
{
   return currentBank() + _BankBytes - 1;
}

#define bankEnd bankEnd_flashBlk 



/*-----------------------------------------------------------------------------------
|
|  compactToOtherBank()
|
|  Transfer all the data from the current flash bank to the other bank, compacting it
|  on the way. The destination bank is marked as the current bank. The source bank is
|  then erased.
|
--------------------------------------------------------------------------------------*/

PRIVATE void compactToOtherBank( void )
{
   T_FlashAddr src, dest,    // will copy from 'src' to 'dest'
               s, d;
   U8 c;

   if( currentBank() == bankA() )
      { src = bankA(); dest = bankB(); }
   else
      { src = bankB(); dest = bankA(); }

   s = src; d = dest;

   while( s < (src + _BankBytes - 1) )          // for each slot in the current bank
   {
      if( slotHasData(s) )                      // if slot contains data
      {
         for( c = _SlotBytes; c; c-- )          // then copy the slot contents to destination bank
         {
            Flash_WrByte( d++, FlashBlk_GetCh(s++) );
         }
      }
      else                                      // else slot has no data
      {
         s += _SlotBytes;                       // so go to next slot.
      }
   }
   Flash_WrByte( _TagAddrFromBank(dest), 0 );   // Mark the destination bank as the current
   Flash_Erase(src,1);                          // and erase the source bank
}



/*-----------------------------------------------------------------------------------
|
|  firstEmptySlot()
|
|  Return the first empty slot in the current flash bank, 0 if the bank is full.
|
--------------------------------------------------------------------------------------*/

PRIVATE T_FlashAddr firstEmptySlot(void)
{
   T_FlashAddr p;

   // The label of first unused slot will still be 0xFF (erased state). 

   for( p = currentBank(); FlashBlk_GetHdl(p) != _ErasedHandle; p += _SlotBytes )     
   {
      if( p >= bankEnd() )
         { return 0; }
   }
   return p;
}


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_getVarAddr()
|
|  Return address in the store of the start of variable 'hdl'. If 'hdl' does not
|  exist returns 0.
|
--------------------------------------------------------------------------------------*/

PUBLIC T_FlashAddr getVarAddr_flashBlk( T_FlashBlk_Handle hdl )
{
   T_FlashAddr p;
   
   for( p = currentBank(); FlashBlk_GetHdl(p) != hdl; p += _SlotBytes )     
   {
      if( p >= bankEnd() ) 
         { return 0; }
   }
   return p;
}


// ================================ end: Private =======================================


PUBLIC U16 FlashBlk_BankCapacity(void)
{
   return _BankCapacity;
}


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Clear()
|
|  Initialise the Flash store. Erase both blocks, then mark one block as the current one.
|
|  If 'force' == 1 then initialise the bank regardless.
|
--------------------------------------------------------------------------------------*/

PUBLIC void FlashBlk_Clear( void )
{
   Flash_SetBank(FlashBlk_GetCodeBank());
   Flash_Erase(bankA(), 1);                      // then wipe both banks
   Flash_Erase(bankB(), 1);
   Flash_WrByte(_TagAddrFromBank(bankA()), 0);   // and make the 1st current
}




/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Init()
|
|  Initialise a Flash store if necessary. If neither block in the store has beem marked 
|  as initialised, then erase both blocks, then mark one block as the current one.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_Init( T_ObjAddr obj, T_CodeAddr init )
{
   if( !currentBank() )          // no valid bank?
   {
      FlashBlk_Clear();          // then initialise store from scratch
   }
   return 1;
}



/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Read(), FlashBlk_ReadOfs()
|
|  Read up to 'maxRead' bytes to 'out'
|
|  PARAMETERS:    'out'       - location 
|                 'hdl'       - the handle of the data to be written
|                 'maxRead'   - number of bytes to read
|                 'ofs'       - start position for read
|
|  RETURNS:       number of bytes read.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_Read( U8 *out, T_FlashBlk_Handle hdl, U8 maxRead )
{
   return FlashBlk_ReadOfs( out, hdl, maxRead, 0 );
}

PUBLIC U8 FlashBlk_ReadOfs( U8 *out, T_FlashBlk_Handle hdl, U8 maxRead, U8 ofs )
{
   T_FlashAddr p;
   U8 c, readIdx, bytesRead;

   /* Search forward through the current bank for the first slot which matches
      'hdl'.
   */
   for( p = currentBank();; p += _SlotBytes )      // check each slot
   {
      if( p == 0 || p >= bankEnd() )               // Store not initialised OR end of bank and no match?
      { 
	      	#ifdef DBG_FLASHBLK
	      sprintf(PrintBuf.buf, "Flashblk: read - no bank / no match p = %x\r\n",  (U16)p );
	      PrintBuffer();
      		#endif
      	return 0;                                  // so no bytes copied
      }
      else if( FlashBlk_GetHdl(p) == hdl )         // else match for 'hdl'
      { 
	      	#ifdef DBG_FLASHBLK
   	 	WrStrLiteral("Flashblk: read - matched hdl\r\n");
      		#endif
      	break;                                   // so break to read out the data
     	}
   }

   /* Read up to 'maxRead' bytes into out. Previous search leaves 'p' at the handle
      field of the matching slot.
   */
   for( bytesRead = 0, readIdx = 0;; )
   {
      p++;                                         // advance from slot 'hdl' field to start of data

      for( c = _SlotDataBytes; c; c-- )            // for each data byte in this slot
      {
         if( readIdx >= ofs )                      // past the offset, if non-zero?
         { 
            *out++ = FlashBlk_GetCh(p);            // then copy data byte to 'out'

            if( ++bytesRead >= maxRead )           // enough bytes read?
               { break; }                          // then done.
         }
         readIdx++; p++;                           // advance read ptr and index
      }
      if( p >= bankEnd() )                         // at end of data bank?
         { break; }                                // so done

      else if( FlashBlk_GetHdl(p) != hdl )         // next slot not a match to 'hdl'?
      {
         break;
      }
   }
   return bytesRead;                               // return bytes read to 'out'
}


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Set/ClrAppend()
|
|  Sets / clears append mode for flash writes. In append mode calls to FlashBlk_Write()
|  append to any existing data with the same handle. For the data to be contiguous in
|  the store the preexisiting data must have been the most recent write to the store.
|  'Append' is used to write data directly from the host port to flash without an 
|  intervening buffer. The writing process must set append mode and clear it afterwards.
|
|  RETURNS:
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

PRIVATE BIT append = 0;

PUBLIC void FlashBlk_SetAppend(void) { append = 1; }
PUBLIC void FlashBlk_ClrAppend(void) { append = 0; }



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
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_Write( U8 *src, T_FlashBlk_Handle hdl, U8 cnt )
{
   T_FlashAddr p;
   U8          c;

   Flash_SetBank(FlashBlk_GetCodeBank());

   // Look for existing variable labeled 'hdl'. If its value matches 'src' then don't rewrite.

   if( FlashBlk_MatchVar(src, hdl, cnt) )
   { 
      	#ifdef DBG_FLASHBLK
    	WrStrLiteral("Flashblk: write - value already in store\r\n");
      	#endif
   	return 1; 
  	}

   // Delete existing value, if it exists.

   FlashBlk_Delete( hdl );

   /* Search forward until the 1st empty slot. If there is not enough room in current bank 
      to add data; either switch banks and compact data to make room or, if that won't succeed, 
      then quit.
   */
   p = firstEmptySlot();

   if( p == 0 ||                                            // current bank is full? OR 
       (cnt > ((bankEnd() - p) * _SlotDataBytes / _SlotBytes)) )  // not enough room to append to current bank?
   {
      if( cnt > FlashBlk_BytesFree() )                      // not enough even if bank were compacted?
      {
      		#ifdef DBG_FLASHBLK
      	WrStrLiteral("Flashblk: no bytes free\r\n");
      		#endif
         return 0;                                          // then can't write - quit
      }
      else                                                  // else compacting will free up space
      {
      		#ifdef DBG_FLASHBLK
      	WrStrLiteral("Flashblk: compact to other bank\r\n");
      		#endif
         compactToOtherBank();                              // so do it
      }
   }

   /* Write new data from 'src' to flash, using as many blocks as necessary. Each block is labelled 
      with 'hdl'. Because we already checked that the data will fit in the block there's no need
      to check while writing.
   */
   for( p = firstEmptySlot();; )
   {
      if( !cnt )                                            // all data written?
      { 
      		#ifdef DBG_FLASHBLK
      	WrStrLiteral("Flashblk: no (more) bytes to write\r\n");
      		#endif
      	return 1;                                       	// so return success!
     	}
      else                                                  // else more to write
      {
         if( !Flash_WrByte(p++, hdl) )                      // so first write the handle
         { 
	      		#ifdef DBG_FLASHBLK
	      	WrStrLiteral("Flashblk: handle write failed\r\n");
	      		#endif
         	return 0; 
         }
         else
         {
	      		#ifdef DBG_FLASHBLK
	      	WrStrLiteral("Flashblk: wrote Hdl\r\n");
	      		#endif
         }
         
      }

      for( c = _SlotDataBytes; c; c-- )                     // then fill the slot with data.. for each byte in the slot
      {
         if( !cnt-- )                                       // no more data to write (in this slot)?
         {
	      		#ifdef DBG_FLASHBLK
	      	WrStrLiteral("Flashblk: all data written\r\n");
	      		#endif
            return 1;                                       // then return success!
         }
         else                                               // else more data to write (in this slot)
         {
            if( !Flash_WrByte(p++, *src++) )                // so write byte and advance ptrs
            { 
	      			#ifdef DBG_FLASHBLK
	      		WrStrLiteral("Flashblk: byte write failed\r\n");
	      			#endif
            	return 0;                                 // fail if write didn't verify
           	}
           	else
           	{
	      			#ifdef DBG_FLASHBLK
	      		WrStrLiteral("Flashblk: wrote byte\r\n");
	      			#endif
           	}
         }
      }
   }
   return 0;             // Shouldn't ever reach here... but just in case
}


// --------------------------------------- eof ------------------------------------------
