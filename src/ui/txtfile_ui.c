/*---------------------------------------------------------------------------
|
|                       Mini Text Files in Flash
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

extern BIT inFileMode;
extern BIT overflowedStore;
extern BIT terminalMode;

extern S_TxtFile TxtF;


/*-----------------------------------------------------------------------------------------
|
|  reportOnBank()
|
------------------------------------------------------------------------------------------*/

PRIVATE void reportOnBank(U8 bank)
{
   TxtF.currentBank = bank;

   sprintf(PrintBuf.buf,
         sizeof(T_TxtFileAddr) == 2                              // Flash space is 16bit?
            ? "Bank %d start 0x%X size %u contains %u\r\n"     // then print addresses as 2-digit hex
            : "Bank %d start 0x%lX size %u contains %u\r\n",   // else print as 4-digits.
      bank,
      File_BankStart(),
      File_BankSize(),
      File_BytesInBank()
      );
   PrintBuffer();

   if( File_BytesInBank() > 0 )                      // bank not empty?
   {
      WrStrLiteral("   ---- ");
      File_EnterTextBank();
      Comms_WrLine((U8 CONST*)Str_GetNthWord(_StrConst((C8 CONST *)File_BankStart()), 0));  // then quote the 1st line with any text.
      File_LeaveTextBank();
   }
}

/*-----------------------------------------------------------------------------------------
|
|  UI_File()
|
|  Syntax:
|
|     txtfile 'report' | <fileNum <<'write' ['term']>|'clear'|'list'>>
|
|        fileNum     - is a legal file number, currently 0..2
|        'report'    - shows details on all files
|        'write'     - starts a write of user text to file 'fileNum'
|        'clear'     - erases 'fileNum'
|        'list'      - lists contents of 'fileNum'
|        'term'      - when specified with 'write', echoes incoming chars back to user
|
|  Examples:
|
|     "txtfile report"     - report on all files
|     "txtfile 1 list"     - show contents of file 1
|
------------------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST UI_File_Help[] =
"\
  Syntax:\r\n\
\r\n\
     txtfile 'report' | <fileNum <<'write' ['term']>|'clear'|'list'>>\r\n\
\r\n\
        fileNum     - is a legal file number, currently 0..2\r\n\
        'report'    - shows details on all files\r\n\
        'write'     - starts a write of user text to file 'fileNum'\r\n\
        'clear'     - erases 'fileNum'\r\n\
        'list'      - lists contents of 'fileNum'\r\n\
        'term'      - when specified with 'write', uses the line editor, else chars flashed as they are typed\r\n\
\r\n\
  Examples:\r\n\
\r\n\
     txtfile report          - report on all files\r\n\
     txtfile 1 list          - show contents of file 1\r\n\
     txtfile 2 write term    - write to file 2, using line editor\r\n";
   #endif // INCLUDE_HELP_TEXT

typedef enum { action_Write, action_List, action_Clear } E_Actions;
PRIVATE C8 CONST actionList[] = "write list clear";

PUBLIC U8 UI_File(C8 *args)
{
   U8 c, bank, action;

   if( Str_WordInStr(args, "report") )
   {
      for( c = 0; c < File_GetNumBanks(); c++ )
         { reportOnBank(c); }
   }
   else
   {
      if( !File_LegalBank_MsgIfNot(args, &bank) )
      {
         return 0;
      }
      else
      {
         TxtF.currentBank = bank;                              // Set current bank to that specified in command line

         if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )
            { return 0; }
         else
         {
            switch(action)
            {
               case action_Write:
                  if( InServiceMode_MsgIfNot() )               // In service mode? then can write
                  {                                            // If 'term' specified, then will echo incoming chars                                                                  
                     terminalMode = Str_WordInStr(args, "term");
                     File_SetupWrite();
                  }
                  break;

               case action_List:                               // Print the file contents to the terminal
                  File_List();
                  break;

               case action_Clear:
                  if( InServiceMode_MsgIfNot() )               // In service mode?
                  { 
                     File_EraseBank();                         // then erase current bank and send message
                     sprintf(PrintBuf.buf, "File %d erased\r\n", TxtF.currentBank );
                     PrintBuffer();
                  }
                  break;
            }
         }
      }
   }
   return 1;
}


// -------------------------------------- eof -------------------------------------------- 
