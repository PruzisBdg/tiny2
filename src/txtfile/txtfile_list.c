/*-----------------------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|  File_List()
|
|  List the contents of the current text file, specified by the global
|  'TxtF.currentBank'. If the file is empty then say so.
|
------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"           // WrStrLiteral()
#include "txtfile.h"
#include "txtfile_hide.h"     // File_BankStart()
#include "hostcomms.h"


PUBLIC void File_List(void)
{
   S16 c;

   File_EnterTextBank();                           // Enter text bank to read num chars
   c = File_BytesInBank();                         // How many chars before 0xFF?
   File_LeaveTextBank();                           // then leave the bank

   sprintf(PrintBuf.buf, "Store %d has %d bytes....\r\n", TxtF.currentBank, c );
   PrintBuffer();
   
   if( c > 0 )                                     // At least one char in store?
   { 
      File_EnterTextBank();                        // then reenter text bank to list contents
      
      /* Print out, 100 bytes at a time, so out buffer not overloaded. 
      
         ***** Also, evaluate File_BytesInBank() each time in the loop, instead of
         using a variable e.g 'cnt'. This adds a delay which the printout needs to 
         work. With  a 'cnt' the printout trips up.
      */
      c = 0;
      while( c < (S16)File_BytesInBank()-100 )     // Do by 100's, until fewer than 100 left         
      {
         Comms_WrBuf( (U8 CONST*)File_BankStart()+c, 100 );    // Print 100 chars
          c += 100;                                // Move ahead
      }
      WrStrConst((U8 CONST*)File_BankStart()+c);   // Print remaining text up to the '\0'
      File_LeaveTextBank();                        // and leave the bank once it's printed.
      WrStrLiteral("\r\n");                        // Drop to next line after printout.
   }
}

// --------------------------- eof -----------------------------------------
