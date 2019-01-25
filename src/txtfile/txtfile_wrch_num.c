/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|  This utility reads text from the Host interface and stores it in MCU flash
|  as strings. Each text 'file' resides in one or more MCU flash sectors.
|  A flash sector holds a single text 'file'. When text is written to a
|  'file' the existing data is erased and repaced by the new text.
|
|  This utility is used for holding text scripts and and text-based configuration 
|  data. It is complementary to the FlashBlock utlity (flashblk.c) which stores
|  many small binary items in single flash sectors and is typically used for holding
|  calibration numbers.
|
|--------------------------------------------------------------------------*/

//#include "libs_support.h"
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
#include "util.h"
#include "arith.h"


// =================  Configurations, defined externally =====================

/*-----------------------------------------------------------------------------------------
|
|  file_WrS16AtIdx()
|
|  Write 'n' to the current text store at 'idx', where 'idx' accesses an array of
|  S16
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_WrS16AtIdx(S16 idx, S16 n)
{
   // Index to chars is 2 * index to integers.
   file_WrChAtIdx(2*idx,     HIGH_BYTE(n)); 
   file_WrChAtIdx((2*idx)+1, LOW_BYTE(n));
}

/*-----------------------------------------------------------------------------------------
|
|  file_WrS16_BumpPtr()
|
|  Write 'n' to the current text store at TxtF.cnt. Increment TctF.cnt
|
------------------------------------------------------------------------------------------*/

PUBLIC void file_WrS16_BumpPtr(S16 n)
{
   file_WrCh_BumpPtr(HIGH_BYTE(n));
   file_WrCh_BumpPtr(LOW_BYTE(n));
}

/*-----------------------------------------------------------------------------------------
|
|  wrS16ToStore()
|
|  Write 'n' to the current text store.
|
|  Returns 1 if success; 0 if no room left
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT wrS16ToStore(S16 n)
{
   if( overflowedStore )                           // No more room?
   {
      return 0;
   }
   else                                            // else more room maybe - check
   {
      if( TxtF.cnt >= File_BankSize() - 2 )        // Check for room. No! store has just 1 or 2 bytes left
      {
         Comms_WrStrConst((U8 CONST *)"Too many numbers for store\r\n");
         overflowedStore = 1;
         return 0;
      }
      else                                         // else room for another number
      {                                            
         file_WrS16_BumpPtr(n);                    // so write number, msb 1st (for 8051)
         return 1;                                 // Success!
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  finishUpIntegerStore()
|
------------------------------------------------------------------------------------------*/

PRIVATE void finishUpIntegerStore(void) 
{
   S16 intCnt;
   
   /* 'TxtF.cnt' is the total number of bytes written to the store. Each integer is
      2 bytes. The 1st integer is the number-store tag; the 2nd is the number-count.   
   */
   intCnt = MaxS16( (TxtF.cnt/2) - 2, 0);

   // Insert the number of integers in the 2nd integer slot.
   file_WrS16AtIdx(1, intCnt);               
   
   // and say how many we wrote.
   sprintf(PrintBuf.buf, "\r\nNumbers written = %d\r\n", intCnt);
   PrintBuffer();   
}



/*-----------------------------------------------------------------------------------------
|
|  File_WrCh_Num()
|
|  Write a character from the Host port to the current flash bank / file. Esc terminates
|  the writes and returns control to the command line interpreter (see hostcomms.c).
|
|  If the flash bank is full there is an error message and subsequent incoming char(s) 
|  are dumped.
|
------------------------------------------------------------------------------------------*/

PUBLIC void File_WrCh_Num(U8 ch)
{
   S16 n;

   file_setFlashBank();                                  // Select the 32KB bank for Flash writes

   if(ch == 27)                                          // Esc?
   {
      if( storeBinaryInts )                              // Was reading integers to store as binary?
      {                                                  // then finish up. First flush the last number...                                         
         if( ReadDirtyASCIIInt_ByCh(ch, &n) )            // ... if there is one?
         { 
            wrS16ToStore(n);                             // and write it to the store.
         }                                     
         finishUpIntegerStore();                         // Insert integer count in 2nd slot            
      }
      else
      {
         file_wrLineToStore();                           // write the line so far to the store
         file_wrCharToStore('\0');                       // Terminate text with '\0' so it will read as string
   
         Comms_WrStrConst( _RomStr("\r\n\r\nEcho to check:\r\n") );  // echo revised flash contents to check
         File_List();                                    // Echo revised flash contents to check
   
         terminalMode = 0;                               // cancel line editor, if it was enabled
      }
      inFileMode = 0;                                    // and we're back to command line mode
      LineEdit_ClearCurrentBuffer();                     // and clear buffer for new edit
   }
   else                                                  // else not Esc, so not done
   {
      ResetTimer(&file_WhenGotLastChar);                 // Reset timer (whether it's used or no)
      
      if( storeBinaryInts )                              // Reading integers to store as binary?
      {
         if( ReadDirtyASCIIInt_ByCh(ch, &n) )            // Got another ASCII integer
         {
            wrS16ToStore(n);                             // then write it to the store as binary
            
            if( (TxtF.cnt % 20) == 0)                    // Every 20 chars = every 10 integers
            {                                            // print the number of integers written so far
               sprintf(PrintBuf.buf, "\x1B[2K\r> writing: %d", TxtF.cnt/2);
               PrintBuffer();
            }   
         }
      }
      else
      {
         if( File_WritingScript )                        // Wrting a script file?
            { ch = File_StripScript(ch); }               // then strip comments and whitespace.
         
         if( ch )                                        // Write this char?
         {            
            if( !terminalMode )                          // 'stream' mode?
            {
               file_wrCharToStore(ch);                   // then put char straght into the store (flash)
            }
            else                                         // else 'terminal' mode - use the line editor
            {
               if( LineEdit_AddCh(ch) )                  // process char - got a complete line?
               {
                  LineEdit_AcceptLine();                 // store in multi-line buffer
                  file_wrLineToStore();                  // write the complete line to flash, adding a CR.
                  Comms_WrStrConst(_RomStr("\r\n+ "));   // drop to edit next line on terminal
               }
            }
         }
      }
   }
   File_ResetBankSw();                                   // Restore original C8051 PSBANK settings
}

// -------------------------------------- eof -------------------------------------------- 
