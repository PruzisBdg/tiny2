/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "txtfile.h"          // struct TxtF, File_EraseBank()
#include "tty_ui.h"           // WrStrLiteral()
#include "txtfile_hide.h"     // 'inFileMode', 'overflowedStore'
#include "lineedit.h"         // LineEdit_ClearCurrentBuffer()
#include "systime.h"

/* When writing text to a file, this timer logs the last time a char was received
   from the terminal. It is used to auto-terminate when a file is being streamed 
   from a terminal to a flash store.
*/   
PUBLIC T_Timer file_WhenGotLastChar;

/*-----------------------------------------------------------------------------------------
|
|  File_SetupWrite()
|
|  Setup a file for writing; also used by Script_UI.
|
------------------------------------------------------------------------------------------*/

extern void File_EraseBank(void);

PUBLIC void File_SetupWrite(void)
{
   inFileMode = 1;                           // So incoming chars are redirected to Flash
   overflowedStore = 0;                      // False until store overflows
   TxtF.cnt = 0;                             // Zero the file byte-count
   File_EraseBank();                         // Erase current contents.. now ready to accept new text
   
   file_WhenGotLastChar = _Never;            // Until 1st char received.
                     
   WrStrLiteral(
      terminalMode                           // Hand-entered text / or streamed from file? Send message
         ? "Enter text by line. <Enter> commits current line to flash. Esc to quit\r\n+ "
         : "Send ASCII now, > 1msec inter-char interval; Esc to quit\r\n" );
                              
   LineEdit_ClearCurrentBuffer();            // Flush this command from the lne buffer
}


// --------------------------- eof ------------------------------------------------ 
