/*---------------------------------------------------------------------------
|
|        Tiny1 Test Fixture Support - Standard User Interface Command Handlers
|
|  Each test fixture supports at least these.
|
|--------------------------------------------------------------------------*/

#ifndef TTY_UI_H
#define TTY_UI_H

#include "common.h"
#include "hostcomms.h"
#include "sysobj.h"

// Specification for a command

typedef struct
{
   C8   CONST  *cmd;                      // command name
   U8          (*handler)(C8 *parms);     // handler
   C8 CONST    *helpStr;                  // help string (optional)
   U8          minArgs;                   // minimum number of arguments
} S_UICmdSpec;

#define _UI_MaxCmdChars 10          // Command name can be no longer than this.

//#define _CmdSpec_Flags_ReturnsData  // command returns value(s) or status e.g 'read', rather than just being executed e.g 'delay'

// List of commands

typedef struct
{
   S_UICmdSpec CONST *cmdSpecs;
   U8              numCmds;   
} S_UICmdList;


PUBLIC U8   UI_DoHelp( C8 * args );
PUBLIC U8   UI_EchoHdlr( C8 *args );
PUBLIC U8   UI_ReadObj( C8 *args );
PUBLIC U8   UI_WriteObj( C8 *args );
PUBLIC U8   UI_ListObjs( C8 *args );
PUBLIC U8   UI_Thread( C8 * args );

PUBLIC BIT UI_BeQuiet(void);

PUBLIC void UI_PrintScalar( S16 n, S_ObjIO CONST * io, U8 appendUnits);
PUBLIC void UI_SPrintScalar( C8 *out, S16 n, S_ObjIO CONST * io, U8 appendUnits );
#define _UI_PrintScalar_NoUnits     0
#define _UI_PrintScalar_AppendUnits 1

PUBLIC void UI_PrintVector( S_Vec *vec, S_ObjIO CONST * io, BIT appendUnits);
#define _UI_PrintVector_NoUnits     0
#define _UI_PrintVector_AppendUnits 1

PUBLIC void UI_PrintObject( S_Obj CONST *obj, U8 flags );
PUBLIC void UI_SPrintScalarObject( C8 *out, S_Obj CONST *obj, U8 flags );

#define _UI_PrintObject_AppendUnits _BitM(0)
#define _UI_PrintObject_PrependName _BitM(1)
#define _UI_PrintObject_Raw         _BitM(2)

PUBLIC S16 UI_GetScalarArg(C8 *args, U8 idx, float scale );
PUBLIC S16 UI_GetIntFromObject(S_Obj CONST *obj, C8 *p);

PUBLIC void UI_PrintVec( S_Vec *v, C8 CONST *format, float scale );

extern S_UICmdList CONST UICmdList;    // User application must make this command list

PUBLIC BIT UI_RawInArgList(C8 * args);
PUBLIC S16 UI_IntFromArgs( C8 *args, S_ObjIO CONST *io, U8 idx);

PUBLIC S_Obj CONST * UI_GetObj_MsgIfFail(C8 *args, U8 classID);
PUBLIC S_Obj CONST * UI_GetAnyObj_MsgIfFail(C8 *args);

PUBLIC U8 UI_GetAction_MsgIfFail(C8 *args, C8 CONST * actionList);
#define _UI_NoActionMatched 0xFF

PUBLIC U8 Scanf_NoArgs(U8 scanfRtn );



/* ----- UI Help Strings switch

   In some targets help text is omitted to save code space. tty_ui.lib has no
   help strings. tty_ui_whelp.lib has help strings.
*/
   #ifdef LINK_TTY_UI_HELP_TEXT

extern C8 CONST UI_Echo_Help[];
extern C8 CONST UI_Help_Help[];
extern C8 CONST UI_ReadObj_Help[];
extern C8 CONST UI_ListObjs_Help[];
extern C8 CONST UI_Cal_Help[];
extern C8 CONST FlashBlk_Help[];
extern C8 CONST UI_CalV_Help[];
extern C8 CONST UI_Thread_Help[];
extern C8 CONST UI_Script_Help[];
extern C8 CONST UI_File_Help[];
extern C8 CONST UI_File_Num_Help[];
extern C8 CONST UI_Write_Help[];
extern C8 CONST UI_OutPin_Help[];
extern C8 CONST UI_Sensor_Help[];
extern C8 CONST UI_Actuator_Help[];
extern C8 CONST UI_Quiet_Help[];
extern C8 CONST Eval_Help[];
extern C8 CONST UI_InPin_Help[];
extern C8 CONST UI_OutPort_Help[];

   #endif // LINK_TTY_UI_HELP_TEXT

   /* If not using string help, then #def this in your applications 'hostcmdtbl.c' and
      provide str_NoHelp[].
   */
   #ifdef LINK_TTY_UI_HELP_TEXT     // First, make sure both #defs don't exist at the 
      #undef NO_TTY_UI_HELP         // same time.
   #endif
   
   #ifdef NO_TTY_UI_HELP   

#define UI_Echo_Help       str_NoHelp
#define UI_Help_Help       str_NoHelp
#define UI_ReadObj_Help    str_NoHelp
#define UI_ListObjs_Help   str_NoHelp
#define UI_Cal_Help        str_NoHelp
#define FlashBlk_Help      str_NoHelp
#define UI_CalV_Help       str_NoHelp
#define UI_Thread_Help     str_NoHelp
#define UI_Script_Help     str_NoHelp
#define UI_File_Help       str_NoHelp
#define UI_File_Num_Help   str_NoHelp
#define UI_Write_Help      str_NoHelp
#define UI_OutPin_Help     str_NoHelp
#define UI_Sensor_Help     str_NoHelp
#define UI_Actuator_Help   str_NoHelp
#define UI_Quiet_Help      str_NoHelp
#define Eval_Help          str_NoHelp
#define UI_InPin_Help      str_NoHelp
#define UI_OutPort_Help    str_NoHelp
#define UI_Servo_Help      str_NoHelp

   #endif // NO_TTY_UI_HELP
   
#endif // TTY_UI_H   

// ------------------------------- eof ----------------------------------------
