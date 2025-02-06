/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "common.h"
#include "hostcomms.h"     // PrintBuffer()
#include "wordlist.h"      // Str_GetNthWord()
         // sprintf()
#include "txtfile.h"       // File_LegalBank_MsgIfNot()
#include "romstring.h"     // _StrConst()
#include "txtfile_hide.h"  // File_GetBankNames()


/*-----------------------------------------------------------------------------------------
|
|  File_LegalBank_MsgIfNot()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT File_LegalBank_MsgIfNot(C8 *args, U8 *bank)
{
   if( (*bank = Str_FindWord(_StrConst(File_GetBankNames()), Str_GetNthWord(args,0))) == _Str_NoMatch)    // file number wasn't legal
   {
      sprintf(PrintBuf.buf, "Store does not exist.  Specify one of %s\r\n", File_GetBankNames() );   // then show user legal choices
      PrintBuffer();
      return 0;
   }
   else
   {
      return 1;
   }
}

// ------------------------------------ eof -------------------------------------------- 
