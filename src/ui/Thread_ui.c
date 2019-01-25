/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Thread control
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"

#include "sysobj.h"           // S_Obj
#include "class.h"
#include "wordlist.h"      // Str_FindWord()
#include "svc.h"
#include "romstring.h"



/*-----------------------------------------------------------------------------------------
|
|  UI_Thread()
|
|  User interface to thread objects.
|
|     thread <thread_name> <'run'|'stop'|'restart'|'report'>
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 CONST UI_Thread_Help[] = "thread <thread_name> ['run' | 'stop' | 'restart' | 'report' ]";

typedef enum { threadAction_Run, threadAction_Stop, threadAction_Restart, threadAction_Report } E_threadActions;
PRIVATE U8 CONST threadActions[] = "run stop restart report";

PRIVATE CONST U8 runStr[] = "run";
PRIVATE CONST U8 stopStr[] = "stop";

PUBLIC U8 UI_Thread( U8 * args )
{
   U8          action;
   U8 CONST    *p;
   S_TinySM    *sm;
   S_Obj CONST * obj;

   if( !(obj = UI_GetObj_MsgIfFail(args, _Class_Thread)) )
   {
      return 0;
   }
   else
   {
      if( (action = UI_GetAction_MsgIfFail(args, threadActions)) == _Str_NoMatch )
      {
         return 0;
      }
      else
      {
         sm = (S_TinySM*)obj->addr;
            
         switch(action)
         {
            case threadAction_Run:
               TinySM_Run(sm);
               break;

            case threadAction_Stop:
               TinySM_Stop(sm);
               break;

            case threadAction_Restart:
               TinySM_Restart(sm);
               break;

            case threadAction_Report:

               if( TinySM_GetMode(sm) == TinySM_Mode_Stop )
                  { p = stopStr;  }
               else
                  { p = runStr;  }

               sprintf(PrintBuf.buf, _StrLit("mode = %s state = %d\r\n"), _StrConst(p), TinySM_GetState(sm) );
               PrintBuffer();
               break;
         }
      }
   }
   return 1;
}

// -------------------------------------- eof ------------------------------------------------ 
