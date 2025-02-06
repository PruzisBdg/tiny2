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


// =================  Configurations, defined externally =====================

// The following PUBLIC are for 'txtfile_ui.c' only and so are not in the a global
// header file, just extern'ed in txtfile_ui.c

PUBLIC S_TxtFileBank CONST * file_getFlashBanks(void);
PUBLIC U8 File_GetNumBanks(void);
PUBLIC C8 CONST * File_GetBankNames(void);
PUBLIC U16 File_BankSize(void);
PUBLIC T_FlashAddr File_BankStart(void);

/*-----------------------------------------------------------------------------------------
|
|  File_WrCh()
|
|  Write a character from the Host port to the current flash bank / file. Esc terminates
|  the writes and returns control to the command line interpreter (see hostcomms.c).
|
|  If the flash bank is full there is an error message and subsequent incoming char(s) 
|  are dumped.
|
------------------------------------------------------------------------------------------*/

PUBLIC void File_WrCh(U8 ch)
{
   file_setFlashBank();                                  // Select the 32KB bank for Flash writes

   if(ch == 27)                                          // Esc?, means done
   {
      file_wrLineToStore();                              // write the line so far to the store
      file_wrCharToStore('\0');                          // Terminate text with '\0' so it will read as string

      Comms_WrStrConst( _RomStr("\r\n\r\nEcho to check:\r\n") );  
      File_List();                                       // Echo revised flash contents to check

      inFileMode = 0;                                    // and we're back to command line mode
      terminalMode = 0;                                  // cancel line editor, if it was enabled
      File_WritingScript = 0;
   }
   else                                                  // else this char will be added to file
   {
      ResetTimer(&file_WhenGotLastChar);                 // Reset timer (whether it's used or no)
      
      if( File_WritingScript )                           // Wrting a script file?
         { ch = File_StripScript(ch); }                  // then strip comments and whitespace.
         
      if( ch )                                           // Write this char?
      {         
         if( !terminalMode )                             // 'stream' mode?
         {
            file_wrCharToStore(ch);                      // then put char straght into the store (flash)
         }
         else                                            // else 'terminal' mode - use the line editor
         {
            if( LineEdit_AddCh(ch) )                     // process char - got a complete line?
            {
               LineEdit_AcceptLine();                    // store in multi-line buffer
               file_wrLineToStore();                     // write the complete line to flash, adding a CR.
               Comms_WrStrConst(_RomStr("\r\n+ "));      // drop to edit next line on terminal
            }
         }
      }
   }
   File_ResetBankSw();                                   // Restore original MCU code-bank settings
}


// -------------------------------------- eof --------------------------------------------
