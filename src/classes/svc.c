/*---------------------------------------------------------------------------
|
|            Tiny1 Support - User and Service Mode
|
|  Public:
|     BIT ServiceMode
|     Svc_Init()
|     Svc_Init_AlwaysInSvc()
|     InServiceMode()
|     InServiceMode_MsgIfNot()
|
|--------------------------------------------------------------------------*/

#include "svc.h"
#include "hostcomms.h"     // Comms_WrStr()
#include "common.h"
         // sscanf()
#include "wordlist.h"
#include <string.h>        // strcat()
#include "flashblk.h"
#include "std_nvaddr.h"    // _nvaddr_ServicePassword
#include "romstring.h"

PUBLIC BIT ServiceMode;     

PUBLIC C8 CONST DefaultPassword[] = "default";
#define _MaxPasswordChars 8
#define _MaxPasswordStr (_MaxPasswordChars + 1)

PUBLIC U8 CurrentPassword[_MaxPasswordStr];


/*-----------------------------------------------------------------------------------------
|
|  Svc_Init()
|
|
--------------------------------------------------------------------------------------------*/

PUBLIC void Svc_Init( void )
{
   ServiceMode = 1;     // start in service mode

   if( !FlashBlk_Read( CurrentPassword, _nvaddr_ServicePassword, _MaxPasswordStr) )
   {
      strcpy((C8*)CurrentPassword, DefaultPassword);
   }
}

/*-----------------------------------------------------------------------------------------
|
|  Svc_Init_AlwaysInSvc()
|
|
--------------------------------------------------------------------------------------------*/

PUBLIC void Svc_Init_AlwaysInSvc( void )
{
   ServiceMode = 1;     // start in service mode
}


/*-----------------------------------------------------------------------------------------
|
|  InServiceMode()
|
|  Return 1 if in service mode
|
--------------------------------------------------------------------------------------------*/

PUBLIC U8 InServiceMode(void) { return ServiceMode == 1; }



/*-----------------------------------------------------------------------------------------
|
|  InServiceMode_MsgIfNot()
|
|  Return 1 if in service mode. Send a message if not.
|
--------------------------------------------------------------------------------------------*/

PUBLIC BIT InServiceMode_MsgIfNot(void)
{
   if(ServiceMode == 1)
      { return 1; }
   else
   {
      Comms_WrStrConst("Must be in service mode\r\n");
      return 0;
   }
}



// --------------------------- eof ------------------------------------------
