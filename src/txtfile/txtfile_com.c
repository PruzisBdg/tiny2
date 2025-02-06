/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|            Routines common to version with and without integer stores
|
|  This utility reads text from the Host interface and stores it in MCU flash
|  as strings. Each text 'file' resides in one or more MCU  flash sectors.
|  A flash sector holds a single text 'file'. When text is written to a
|  'file' the existing data is erased and repaced by the new text.
|
|  This utility is used for holding text scripts and and text-based configuration 
|  data. It is complementary to the FlashBlock utlity (flashblk.c) which stores
|  many small binary items in single flash sectors and is typically used for holding
|  calibration numbers.
|
|  Public:
|     File_Read()
|     File_IsOpen()
|     File_NotEmpty()
|     File_BytesInBank()
|     File_BankSize()
|     File_EraseBank()
|     File_GetNumBanks()
|     File_GetBankNames()
|
|  Public - but only to be shared among TxtFile lib:
|     file_getSectorBytes()
|     file_setFlashBank()
|     file_WrChAtIdx()
|     file_WrChAtIdx()
|     file_WrCh_BumpPtr()
|     file_wrCharToStore()
|     file_wrLineToStore()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "flash_basic.h"
#include "hostcomms.h"
#include "wordlist.h"

#include "svc.h"
#include "lineedit.h"
#include "txtfile.h"
#include "tty_ui.h"
#include "romstring.h"
#include "txtfile_hide.h"


// ===============  PRIVATE ===================================

/*-----------------------------------------------------------------------------------------
|
|  getFlashBanks()
|
|  To read an applications flash bank structure.
|
------------------------------------------------------------------------------------------*/

PRIVATE S_TxtFileBank CONST * getFlashBanks(void) 
   { return File_GetStructure()->flashBanks; }


// ==================== Shared just with txtfile module ============================

// The following PUBLIC are for 'txtfile_ui.c' only and so are not in the a global
// header file, just extern'ed in txtfile_ui.c

PUBLIC S_TxtFile TxtF;

// If == 1 then input from the Host is redirected to (this) file system; else
// processed by command line interpreter.
PUBLIC BIT inFileMode = 0;

// Is set when the current store has been filled by incoming chars
PUBLIC BIT overflowedStore;

// If == 1 then then text entry is via the line editor. An (edited) line is
// written to flash when <Enter> is pressed.
PUBLIC BIT terminalMode = 0;

/*-----------------------------------------------------------------------------------------
|
|  File_GetNumBanks()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 File_GetNumBanks(void) { return File_GetStructure()->numBanks; }

/*-----------------------------------------------------------------------------------------
|
|  File_GetBankNames()
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 CONST * File_GetBankNames(void) { return File_GetStructure()->bankNames; }

/*-----------------------------------------------------------------------------------------
|
|  file_getSectorBytes()
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 file_getSectorBytes(void) { return File_GetStructure()->sectorBytes; }

/*-----------------------------------------------------------------------------------------
|
|  file_setFlashBank()
|
|  Set the bank for writes to Flash to that specified in File_GetFlashBank()
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_setFlashBank(void)
{
   Flash_SetBank(File_GetFlashBank());
}


/*-----------------------------------------------------------------------------------------
|
|  file_WrChAtIdx()
|
|  Write 'ch' to the current text bank at 'idx', where 'idx' accesses and array of
|  bytes.
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_WrChAtIdx(S16 idx, U8 ch)
{
   Flash_WrByte(File_BankStart() + idx, ch); 
}



/*-----------------------------------------------------------------------------------------
|
|  file_WrCh_BumpPtr()
|
|  Write 'ch' to the current text bank at TxtF.cnt. Bump 'cnt'.
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_WrCh_BumpPtr(U8 ch)
{
   file_WrChAtIdx(TxtF.cnt, ch);
   TxtF.cnt++;                      // and bump file ptr
}

/*-----------------------------------------------------------------------------------------
|
|  file_wrCharToStore()
|
|  Write 'ch' to the current text store.
|
|  Returns 1 if success; 0 if no room left
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT file_wrCharToStore(U8 ch)
{
   if( overflowedStore )                           // no more room?
   {
      return 0;
   }
   else                                            // else more room maybe - check
   {
      if( TxtF.cnt >= File_BankSize() - 1 )        // Check for room. No! store has just one byte left (to write terminating '\0')?
      {
         Comms_WrStrConst((C8 CONST *)"Too many chars\r\n");
         overflowedStore = 1;
         return 0;
      }
      else                                         // else room for more
      {
         file_WrCh_BumpPtr(ch);
         return 1;
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  file_wrLineToStore()
|
|  Write the current line editor line to the store, prefacing it with '\r'. If the line
|  is empty then nothing is written.
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_wrLineToStore(void)
{
   U8 c, ch;
            
   for( c = 0; c < LineEdit_CharsInCurrentBuf(); c++ )   // for each char in the new line
   {
      ch = LineEdit_CurrentBuf()[c];
      
      if( c == 0 )                        // line is not empty?
      { 
         if( !file_wrCharToStore('\r') )  // then insert CRLF first
            {break; }                     // no room - then we're done
         if( !file_wrCharToStore('\n') )  // LF?
            {break; }                     // no room - then we're done
      }
      
      if( !file_wrCharToStore(ch) )       // insert current char?
         { break; }                       // no room - then we're done
   }
   LineEdit_ClearCurrentBuffer();         // Success or fail- we're done with this line. Clear for next one
}


// =========================== end: private ===================================



/*-----------------------------------------------------------------------------------------
|
|  File_EraseBank()
|
------------------------------------------------------------------------------------------*/

PUBLIC void File_EraseBank(void)
{
   file_setFlashBank();
   Flash_Erase(getFlashBanks()[TxtF.currentBank].start, getFlashBanks()[TxtF.currentBank].sectors);
   File_ResetBankSw();
}


/*-----------------------------------------------------------------------------------------
|
|  File_BankStart()
|
------------------------------------------------------------------------------------------*/

PUBLIC T_TxtFileAddr File_BankStart(void)
{
   return getFlashBanks()[TxtF.currentBank].start;
}

/*-----------------------------------------------------------------------------------------
|
|  File_BankSize()
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 File_BankSize(void)
{
   return file_getSectorBytes() * (U16)getFlashBanks()[TxtF.currentBank].sectors;
}

/*-----------------------------------------------------------------------------------------
|
|  File_BytesInBank()
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 File_BytesInBank(void)
{
   U16 cnt;

   for( cnt = 0; cnt < File_BankSize(); cnt++ )
   {
      if( File_GetCh((U8 CONST *)(File_BankStart() + cnt)) == 0xFF )
         break;
   }
   return cnt;
}


/*-----------------------------------------------------------------------------------------
|
|  File_NotEmpty()
|
|  Return '1' if file 'fileNum' is not empty AND 'fileNum' is a legal file.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT File_NotEmpty(U8 fileNum)
{
   return File_Read(fileNum) != 0;    
}



/*-----------------------------------------------------------------------------------------
|
|  File_IsOpen()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT File_IsOpen(void)
{
   return inFileMode == 1;
}


/*-----------------------------------------------------------------------------------------
|
|  File_Read()
|
|  Return pointer to start of 'fileNum'. If the file is empty then returns 0
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 CONST * File_Read( U8 fileNum )
{
    T_TxtFileAddr start;

   if(fileNum > File_GetNumBanks() - 1)            // Not a legal bank number?
   {
      return 0;
   }
   else                                            // else a legal text bank
   {
      start = getFlashBanks()[fileNum].start;      // Get the start address

      if( File_GetCh((U8 CONST *)start) == 0xFF )  // OxFF? -> then is erased / empty
         { return 0; }                             // so return 0
      else
         { return (C8 CONST *)start; }             // else return start of (presumed) string
   }
}


// -------------------------------------- eof -------------------------------------------- 
