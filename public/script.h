/*---------------------------------------------------------------------------
|
|                       Executes Text Scripts
|
|--------------------------------------------------------------------------*/

#ifndef SCRIPT_H
#define SCRIPT_H

#include "common.h"
#include "tiny_statem.h"

#define _Script_Event_Run     TinySM_1stUserEvent
#define _Script_Event_Delay   TinySM_1stUserEvent + 1
#define _Script_Event_Stop    TinySM_1stUserEvent + 2
#define _Script_Event_Pause   TinySM_1stUserEvent + 3
#define _Script_Event_Resume  TinySM_1stUserEvent + 4
#define _Script_Event_Wait    TinySM_1stUserEvent + 5

extern S_TinySM Script_StateM;
extern BIT Script_Running;

PUBLIC void Script_MarkCmdFailed(void);
PUBLIC BIT Script_HandleCtrlCh( U8 ch );
PUBLIC S16 Script_ReadElapsed(void);
PUBLIC S16 Script_ElapsedMinutes(void);
PUBLIC BIT Script_Run(U8 scriptNum);
PUBLIC BIT Script_Paused(void);
PUBLIC S16 Script_CurrentLine(void);

#define _IU_Sec(t)  ((S16)(10 * (t)))

#endif
