/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Flash Storage - text user Interface
|
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flash_basic.h"
#include "flashblk.h"
#include "wordlist.h"

#include "hostcomms.h"
#include "svc.h"
#include "romstring.h"

/*-----------------------------------------------------------------------------------
|
|  FlashBlk_UI()
|
|  Handler for the user interface 'nvstore' command. Format is:
|
|     Cal <action = 'clear' | 'report'>
|
|     'clear'  erases the store
|     'report' returns a printout of all details of the cal
|
--------------------------------------------------------------------------------------*/

PUBLIC C8 CONST FlashBlk_Help[] = "nvstore <action = 'report' | 'clear'>";

typedef enum { action_Clear, action_Report } E_Actions;
PRIVATE C8 CONST actionList[] = "clear report";

PUBLIC U8 FlashBlk_UI( C8 *args )
{
   U8 action,
      argCnt;
   C8 actionStr[10];

   argCnt = sscanf( (C8*)args, "%s", actionStr );

   if( argCnt != 1 )
   {
   }
   else
   {
      if( (action = Str_FindWord(_StrConst(actionList), actionStr)) == _Str_NoMatch )
      {
         WrStrLiteral("nvstore option must be 'clear' | 'report'\r\n"); 
      }
      else
      {
         switch(action)
         {
            case action_Clear:
               if( InServiceMode_MsgIfNot() )
               {
                  FlashBlk_Clear();
                  WrStrLiteral( "NV Store cleared...  " );
               }

               // fall through to 'report'

            case action_Report:
               sprintf(PrintBuf.buf, "capacity = %u numVars = %u free = %u\r\n", 
                  (U16)FlashBlk_BankCapacity(), 
                  (U16)FlashBlk_VarCnt(), 
                  FlashBlk_BytesFree() 
                  );
               PrintBuffer();
               break;
         }
      }
   }
   return 1;   
}


// -------------------------------------- eof ------------------------------------------ 
