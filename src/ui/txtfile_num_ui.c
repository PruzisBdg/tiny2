/*---------------------------------------------------------------------------
|
|             Mini Text Files in Flash - With Binary Integer Extension
|
|  This module does the same as txtfile_ui.c but also will also read text integers
|  store them as a binary array. This for waveform playback. See txtfile_ui.c
|  for main notes.
|
|  How integers are stored.
|  
|  A 'txtfile' store which holds integers prefaced by a integer tag which identifies 
|  it as a number store. This tag is followed by an (integer) count of the number of
|  integers in the store.
|
|     byte index                      Contents
|     -------------------------------------------------------------
|     [0,1]             Tag   =  '_File_NumericStoreTag'  =   0x5A7B (see txtfile_hide.h)
|     [2,3]             n     =  integer_count
|     [4..4+(2*n)]      integers 
|
|  The integers are stored in the endian of the target system.     
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
#include "util.h"          // ReadDirtyASCIIInt_ByCh_Init()
#include "arith.h"         // MinS16()

extern BIT inFileMode;        // Tells UI that we are working with a file
extern BIT overflowedStore;
extern BIT terminalMode;      // Data is entered, line-by-line, from a terminal

PUBLIC BIT storeBinaryInts;   // Incoming data is ASCII integers, to be stored as binary

extern S_TxtFile TxtF;


/*-----------------------------------------------------------------------------------------
|
|  eraseAndSetupWrite()
|
|  Common initialisation for writing text or integers to a file.
|
------------------------------------------------------------------------------------------*/

PRIVATE void eraseAndSetupWrite(void)
{
   inFileMode = 1;                     // So incoming chars are redirected to Flash
   overflowedStore = 0;                // Unless an overflow happens
   file_WhenGotLastChar = _Never;      // Until 1st char received.
   File_EraseBank();                   // Erase current contents.. now ready to accept new text
}


/*-----------------------------------------------------------------------------------------
|
|  readS16AtIdx()
|
|  Read the 'idx'th binary integer from the current text bank.
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 readS16AtIdx(S16 idx)
{
   S16 n;

   File_EnterTextBank();
   n = ((S16 CONST*)File_BankStart())[idx];
   File_LeaveTextBank();
   
   return n;
}

/*-----------------------------------------------------------------------------------------
|
|  reportOnBank()
|
|  Report the size and contents of each text bank. If the bank holds text then say
|  how many chars there are and quote the 1st line. If it holds integers then say
|  how many integers there are.
|
------------------------------------------------------------------------------------------*/

PRIVATE void reportOnBank(U8 bank)
{
   TxtF.currentBank = bank;
   
   sprintf(PrintBuf.buf, "Bank %d start %x size %u contains %u %s\r\n",
      bank,
      File_BankStart(),
      File_BankSize(),
      readS16AtIdx(0) == _File_NumericStoreTag ? readS16AtIdx(1) : File_BytesInBank(),
      readS16AtIdx(0) == _File_NumericStoreTag ? "integers" : "chars"
      );
   PrintBuffer();
   
   
   if( readS16AtIdx(0) != _File_NumericStoreTag )
   {
      if( File_BytesInBank() > 0 )                      // bank not empty?
      {
         WrStrLiteral("   ---- ");
         File_EnterTextBank();
         Comms_WrLine((U8 CONST *)Str_GetNthWord(_StrConst((C8 CONST *)File_BankStart()), 0));  // then quote the 1st line with any text.
         File_LeaveTextBank();
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  UI_File_Num(), UI_File_Num_Help[]
|
|  These implement the 'txtfile' interface with the integer-store extension. See UI_File()
|  in txtfile_ui.c for the text-only version of this UI.
|
|  Syntax:
|
|     txtfile 'report' | <fileNum <<'write' ['term']>|'clear'|'list'>>
|
|        fileNum     - is a legal file number, currently 0..2
|        'report'    - shows details on all files
|        'write'     - starts a write of user text to file 'fileNum'
|        'num'       - reads integers, stored as binary
|        'clear'     - erases 'fileNum'
|        'list'      - lists contents of 'fileNum'
|        'term'      - when specified with 'write', echoes incoming chars back to user
|
|  Examples:
|
|     "txtfile report"     - report on all files
|     "txtfile 1 list"     - show contents of file 1
|
|  This function and the help string are derived from UI_File() and UI_File_Help[] 
|  respectively. If the applicaiton uses an integer store then these replace their
|  ancestors in the Host's UI table.
|
------------------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST UI_File_Num_Help[] =
"\
  Syntax:\r\n\
\r\n\
     txtfile 'report' | <fileNum <<'write' ['term']>|'num'|'clear'|'list'>>\r\n\
\r\n\
        fileNum     - is a legal file number, currently 0..2\r\n\
        'report'    - shows details on all files\r\n\
        'write'     - starts a write of user text to file 'fileNum'\r\n\
        'wrNum'     - read integers, store as binary\r\n\
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

typedef enum { action_Write, action_WrNum, action_List, action_Clear } E_Actions;
PRIVATE C8 CONST actionList[] = "write wrNum list clear";

PUBLIC U8 UI_File_Num(C8 *args)
{
   U8 c, bank, action;
   S16 idx, n;

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
                  if( InServiceMode_MsgIfNot() )               // in service mode? then can write
                  {                                            // if 'term' specified, then set echo mode   
                     if( Str_WordInStr(args, "term")) { terminalMode = 1; } else {terminalMode = 0;}

                     storeBinaryInts = 0;                      // Tell File_WrCh() Flash literally what it gets
                     eraseAndSetupWrite();                     // Setup flags for write and erase current contents
                     
                     TxtF.cnt = 0;
                     
                     if( terminalMode )
                        { WrStrLiteral("Enter text by line. <Enter> commits current line to flash. Esc to quit\r\n+ "); }
                     else
                        { WrStrLiteral("Send ASCII now, > 2msec inter-char interval; Esc to quit\r\n"); }
                        
                     LineEdit_ClearCurrentBuffer();
                  }
                  break;
                  
               case action_WrNum:
                  if( InServiceMode_MsgIfNot() )               // in service mode? then can write
                  {
                     storeBinaryInts = 1;                      // Tell File_WrCh() to parse integers                     
                     eraseAndSetupWrite();                     // Setup flags for write and erase current contents
                     
                     file_setFlashBank();                      // Select the 32KB bank for Flash writes
                     file_WrS16AtIdx(0, _File_NumericStoreTag);// Write the numeric store tag, (msb 1st)
                     File_ResetBankSw();                       // Restore original MCU code-bank settings
                     
                     // The numeric store tag is followed by an count of the number of integers
                     // in the store. The data goes beyond that, 4 bytes in.
                     TxtF.cnt = 4;
                     WrStrLiteral("Send ASCII integers now, >2msec inter-char interval; Esc to quit\r\n");
                     
                     ReadDirtyASCIIInt_ByCh_Init();            // Init integer-parsing state engine.
                  }
                  break;                  

               case action_List:
               
                  if( readS16AtIdx(0) == 0xFFFF )                          // Store is empty?
                  {
                      WrStrLiteral("is empty\r\n");                        // then say so
                  }

                  /* If this store holds numeric data then the 1st 2 bytes (1st integer) will 
                     contain a tag. The 2nd integer is a count of the numebr of integers 
                     following
                  */                  
                  else if( readS16AtIdx(0) == _File_NumericStoreTag )      // else store contains numeric data?
                  {
                     // Say how many numbers               
                     sprintf(PrintBuf.buf, "Store holds %d numbers...\r\n   ", readS16AtIdx(1) );
                     PrintBuffer();
                     
                     // Get index to last integer; not beyond end of store.
                     n = MinS16( File_BankSize()/2, readS16AtIdx(1)+2 );
                           
                     // Print the numbers, 10 to a line
                     for(idx = 2; idx < n; idx++)
                     {
                        if( (idx+2) % 10 == 0 )                            // 10 numbers printed
                           { WrStrLiteral("\r\n   "); }                    // then descend one line
                           
                        sprintf(PrintBuf.buf, " %d", readS16AtIdx(idx) );
                        PrintBuffer();
                        
                     }
                     WrStrLiteral("\r\n");                  
                  }                  
                  else                                                     // else this store has text
                  {               
                     File_EnterTextBank();
                     WrStrConst_Flag((U8 CONST*)File_BankStart(), _WrStr_AddLF); // so print it
                     File_LeaveTextBank();
                  }
                  break;
                  
               case action_Clear:
                  if( InServiceMode_MsgIfNot() )               // in service mode?
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
