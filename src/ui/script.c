/*---------------------------------------------------------------------------
|
|                       Executes Text Scripts
|
|  See 'C:\SPJ1\swr\tiny1\TOS1 Guide rev.nn.pdf'
|
|  Public:
|     Script_StateM
|     Script_StateM_Cfg
|     Script_Running
|     UI_Script_Help[]
|     UI_Script()
|     Script_Run()
|     Script_MarkCmdFailed()
|     Script_HandleCtrlCh()
|     Script_Paused()
|     Script_CurrentLine()
|     Script_ReadElapsed()
|     Script_ElapsedMinutes()
|
|  TBD:
|     Make getDoAtArg() robust.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tiny_statem.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tty_ui.h"        // UICmdList[]
#include "hostcomms.h"
#include "systime.h"
#include "wordlist.h"
#include "txtfile.h"
#include "script.h"
#include "tiny1util.h"
#include "timers.h"
#include "arith.h"
#include "outpin.h"
#include "romstring.h"
#include "links.h"
#include "class.h"
#include "util.h"

//#include "app_firm.h"

#define _ScriptEngineRev "4"


typedef S16 T_LineNum;

extern S16 EvalExpr(C8 *expr);

// ============================== PRIVATE ============================================

// Specifies a loop
typedef struct
{
   U8          cnt,        // number of loops so far
               limit;      // if a given number of loops specified, this is how many
   T_LineNum   start,      // line number of the 'repeat' statment
               end;        // line number of 'until' or 'endrepeat'
} S_LoopCtl;               // controls one loop

#define _Mode_Default 0    // for now just one mode

#define _MaxArgListChars 40   // Maximum size of arg list passed to a subroutine.

// The current context for the script
typedef struct
{
   T_LineNum lineNum;         // the current line number
   U8        flags;           // quit if fail etc
   S_LoopCtl loop;            // controls script loops
   C8        callArgs[_MaxArgListChars+1];   // the current arg list (for a subroutine)
} S_State;

#define _SubNameMaxChars 21
#define _SubMaxArgs 4

// Tags a subroutine
typedef struct
{
   C8          name[_SubNameMaxChars+1];
   T_LineNum   startLine;                 // The 1st line after the subroutine header
} S_Sub;

#define _MaxSubs 8

typedef struct
{
   C8 CONST    *script;                // the script text (in a Flash text store)
   U8          mode;                   // execute verbose etc.
   T_Timer     dlyTimer,               // delay timer
               started;                // when the script started, used for 'doat' and 'doafter'
   C8          *args;                  // arguments to the current script line
   C8 CONST    *linePtr;               // Start of current line in the ROM script text.
   S_State     curr, pushed, pushed2;  // Contexts, stacked up to two deep
   U8          paused,                 // Script execution paused.
               numSubs,                // the number of subroutines declared in the current script
               skipSub,                // When parsing. '1' = skip through subroutine lines without executing
               stackCnt;               // The stack depth for subroutine calls
   S_Sub       subs[_MaxSubs];         // Subroutine contexts
} S_ScriptEngine;

PRIVATE S_ScriptEngine s;

#define _MaxCmdLineChars 100
PRIVATE C8 lineBuf[_MaxCmdLineChars+4];      // holds the current script line

#define _Flags_LastUserCmdFailed _BitM(0)
#define _Flags_AnyUserCmdFailed  _BitM(1)
#define _Flags_Break             _BitM(2)
#define _Flags_Skip              _BitM(3)    // skip current statement block in an if-else
#define _Flags_DidAnIf           _BitM(4)    // executed at least one statement block in if-elseif-else
#define _Flags_QuitIfTimeout     _BitM(5)    // quit if a 'wait' timed out
#define _Flags_RunningQuit       _BitM(6)    // running some quit actions

/* Set after a '$$' so that the printout does not drop to the next line
   after e.g a 'read' command. E.g
      $$ ambient pressure is
      read Pres6
   will print
      'ambient pressure is Pres6 = 99 kPa'  - all on one line
*/
PRIVATE BIT printMoreOnCurrentLine;


/*-----------------------------------------------------------------------------------------
|
|  getSub1stLine
|
|  Search the subroutine list for a subroutine 'name'. if found, return the line number
|  of the 1st line of the subroutine; else return 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE T_LineNum getSub1stLine(C8 *name)
{
   U8 c;

   for( c = 0; c < s.numSubs; c++ )
   {
      if( Str_1stWordsMatch(s.subs[c].name, name) )
      {
         return s.subs[c].startLine;
      }
   }
   return 0;
}

/*-----------------------------------------------------------------------------------------
|
|  scriptErrorMsg()
|
|  Print "Script error, line nn <description_of_error> "the line text" "
|
------------------------------------------------------------------------------------------*/

PRIVATE void scriptErrorMsg( C8 CONST *msg )
{
   sprintf( PrintBuf.buf, "err line %d: %s \"%s\"\r\n", s.curr.lineNum+1, msg, lineBuf );     // in code lines are counted from 0 upwards
   PrintBuffer();
}

#define _scriptErrorMsg(str) scriptErrorMsg(_RomStr(str))


/*-----------------------------------------------------------------------------------------
|
|  popScriptContext()
|
|  When leaving a subroutine, pop the script context to the caller
|
------------------------------------------------------------------------------------------*/

PRIVATE void popScriptContext(void)
{
   if( !s.stackCnt )                                           // Nothing to pop (already at top)?
   {
      _scriptErrorMsg("stack underflow");                      // then error
   }
   else
   {
      CopyBytesU8((U8*)&s.pushed,  (U8*)&s.curr,   sizeof(S_State));    // pop 1st down to current
      CopyBytesU8((U8*)&s.pushed2, (U8*)&s.pushed, sizeof(S_State));    // and 2nd down to 1st
      s.stackCnt--;                                            // and one less on the stack
   }

}

/*-----------------------------------------------------------------------------------------
|
|  pushScriptContext()
|
|  When entering a subroutine, push the current context.
|
------------------------------------------------------------------------------------------*/

PRIVATE void pushScriptContext(void)
{
   if( s.stackCnt >= 2 )
   {
      _scriptErrorMsg("call depth already = 2; can't push");
   }
   else
   {
      CopyBytesU8((U8*)&s.pushed, (U8*)&s.pushed2, sizeof(S_State)); // Push 1st down to 2nd
      CopyBytesU8((U8*)&s.curr,   (U8*)&s.pushed,  sizeof(S_State)); // and current down to 1st
      s.stackCnt++;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  evalCondition()
|
|  Return 1 if the 'expr' is true
|
------------------------------------------------------------------------------------------*/

PRIVATE C8 CONST condFlags[] = "lastFailed anyFailed noneFailed true false";
typedef enum {expr_LastFailed, expr_AnyFailed, expr_NoneFailed, expr_True, expr_False} E_ExprArgs;

PRIVATE BIT evalCondition( C8 *expr )
{
   switch(Str_FindWord(_StrConst(condFlags), expr))                // Look for one of 'condFlags[]' in 'expr'
   {
      case expr_LastFailed:                                       // 'lastFailed'?
         return BSET( s.curr.flags, _Flags_LastUserCmdFailed);    // then return true if that flag set

      case expr_AnyFailed:                                        // 'anyFailed'?
         return BSET( s.curr.flags, _Flags_AnyUserCmdFailed);     // then return true if that flag set

      case expr_NoneFailed:                                       // 'noneFailed'?
         return BCLR( s.curr.flags, _Flags_AnyUserCmdFailed);     // then return true if AnyFailed is clear

      case expr_True:
         return 1;

      case expr_False:
         return 0;

      default:                                              // Not found.. Str_FindWord() returned _Str_NoMatch
         return EvalExpr(expr) != 0;                        // then pass the args to EvalExpr().
      }
}

// List of script commands

typedef enum
{
   cmd_Wait,
   cmd_PrintRtn,           // prints a string and newline
   cmd_PrintDRtn,          // '$' = shortform for 'print' = cmd_PrintRtn
   cmd_PrintD,             // '$$' = print strng, no newline
   cmd_Quit,               // unconditional
   cmd_Repeat,             // starts a counted or conditional loop
   cmd_EndRepeat,          // ends a counted loop
   cmd_Until,              // ends a loop conitionally
   cmd_While,              // conditionally entered loop
   cmd_Wend,               // ends 'while'
   cmd_Break,              // break out of a loop
   cmd_Continue,           // goto top of loop
   cmd_If,
   cmd_EndIf,
   cmd_Else,
   cmd_ElseIf,
   cmd_Eval,               // evaluate an expression without printing the result; used for assignments
   cmd_Pause,              // pause until Ctr-R (resume) is pressed.
   cmd_Beep,
   cmd_WaitFor,            // wait until and expression is true
   cmd_ClrFail,            // Clear the last fail flag
   cmd_DoAt,               // Do something at a specified time. Continue with script in meantime
   cmd_DoAfter,            // Do something after a delay. Continue with script in meantime
   cmd_Sub,                // starts a subroutine definition
   cmd_EndSub,             // end a subroutine definition
   cmd_Call,               // call a subroutine
   cmd_Return,             // return from inside a subroutine body
   cmd_DoRepeat,           // Starting now, repeat n-times. Continue with script in meantime
   cmd_DoOnQuit            // Actions on quit.
} E_Actions;

PRIVATE C8 CONST scriptCmds[] =
"wait print $ $$ quit repeat endrepeat until while wend break continue "
"if endif else elif < pause beep waitfor clrfail doat doafter sub endsub call return repeatIntvl doOnQuit";

/*-----------------------------------------------------------------------------------------
|
|  isConditionalCmd()
|
|  Return 1 if 'cmd' might be followed by a conditional expression
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT isConditionalCmd(U8 cmd)
{
   switch(cmd)
   {
      case cmd_PrintRtn:
      case cmd_PrintDRtn:
      case cmd_PrintD:
      case cmd_Quit:
      case cmd_While:
      case cmd_Until:
      case cmd_Break:
      case cmd_Continue:
      case cmd_If:
      case cmd_ElseIf:
      case cmd_Pause:
         return 1;

      default:
         return 0;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  readFloat()
|
|  Try parsing a float from 'str'. Return 1 if successful and the result in 'f'.
|  If failed then 'f' is undefined.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT readFloat(C8 *str, float *f)
{
   return !Scanf_NoArgs(sscanf( str, "%f", f ));
}

/*-----------------------------------------------------------------------------------------
|
|  readInt()
|
|  Try parsing an int from 'str'. Return 1 if successful and the result in 'n'.
|  If failed then 'n' is undefined.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT readInt(C8 *str, S16 *n)
{
   return !Scanf_NoArgs(sscanf( str, "%d", n ));
}

/*-----------------------------------------------------------------------------------------
|
|  getTimeFromStr()
|
|  Return the system ticks corresponding to 'timeStr'. If 'timeStr' has 'minutes' appended
|  then read as minutes, else read as seconds.
|
|  'timeStr' must start with a number, no leading whitespace  e.g '10' or '10 minutes'
|  If 'timeStr' did not start with a legal number, then return 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE C8 CONST minuteWords[] = "min mins minute minutes";
PRIVATE C8 CONST hourWords[]   = "hr hrs hour hours";

PRIVATE T_LTime getTimeFromStr( C8 *timeStr )
{
   float f;

   return                              // Read 'timeStr', return as system ticks
      !readFloat(timeStr, &f)                         // Couldn't parse a float number?
         ? 0                                          // then return zero
         : ClipFloatToLong(                           // else result is clipped from float of...
            TicksPerSec                               // TicksPerSec multiplied by
            * f                                       // the time number, multiplied by ...
            * (Str_WordsInStr(s.args, _StrConst(minuteWords)) != _Str_NoMatch    // If followed by 'minutes'?
               ? 60                                                              // then x60
               : (Str_WordsInStr(s.args, _StrConst(hourWords)) != _Str_NoMatch   // If followed by 'hours'?
                  ? 3600.0                                                       // then x3600
                  : 1.0                                                          // else neither 'minutes' nor 'hours' so x1
                 )
              )
           );
}


/*-----------------------------------------------------------------------------------------
|
|  getDoAtArg()
|
|  For 'theLine' containing a 'doat' and 'doafter', return a pointer the the 'action'. E.g
|
|     "doat 2 minutes : quit"
|
|  returns pointer to 'q' in 'quit'.
|
------------------------------------------------------------------------------------------*/

PRIVATE T_AnyAddr getDoAtArg( C8 CONST * theLine )
{
   U8 c;
   T_AnyAddr p;

   File_EnterTextBank();

   c = Str_FindWord(_StrConst(theLine), ":");                  // Get index to ':'
   if( c < _Str_NoMatch ) c += 1;                              // Bump index to next word
   p = (T_AnyAddr)Str_GetNthWord(_StrConst(theLine), c);       // Use U16 so works with CONST and non-const pointers

   File_LeaveTextBank();
   return p;
}


/*-----------------------------------------------------------------------------------------
|
|  Beeper control
|
|  Do one or more beeps, 1 sec apart. Each beep is 0.2sec.
|
|  Beeps are done using the Timers callback.
|
------------------------------------------------------------------------------------------*/

extern S_OutPin Beeper;
PRIVATE void beepOff(void) { OutPin_Write(&Beeper, 0); }

// Enable beeper, and schedule beepOff() 200msec later.
PRIVATE void oneBeep(void) { OutPin_Write(&Beeper, 1); Timer_RunShort(_TicksSec_S(0.2), beepOff); }

// Schedule oneBeep() to be done 'n' times.
PRIVATE void doBeeps(U8 n) { Timer_RunRepeat(_TicksSec_S(1.0), (void(*)(U16,U8))oneBeep, 0, n); }


PRIVATE void doScriptLine( C8 CONST *theLine );

/*-----------------------------------------------------------------------------------------
|
|  doScriptLineUntil()
|
|  Execute 'scriptLine'. If cnt == 0 then send a 'resume' to the script engine.
|
|  This function is sent as a callabck to 'Timers' to execute a given script line
|  multiple times.
|
------------------------------------------------------------------------------------------*/

PRIVATE void doScriptLineUntil( C8 CONST *scriptLine, U8 cnt )
{
   doScriptLine(scriptLine);

   if(cnt == 0 && s.paused)
      { TinySM_SendEvent(&Script_StateM, _Script_Event_Resume); }
}


/*-----------------------------------------------------------------------------------------
|
|  sendScriptLineToTimer()
|
|  Queue a timer to execute the portion of the current script line from a
|  colon (:) onwards.
|
|  This is used by 'doat' and 'doafter' commands which have the format:
|
|     doat 10 minutes : $ execute e.g this line
|
|  ***** Note: To avoid recursion feed the action to the timer even if it's its due now.
|  doScriptLine()  calls the command interpreter doScriptCmd() to process the remainder
|  of the same script line. But doScriptCmd() already called this function,
|  sendScriptLineToTimer()!!! Avoid this recursion by always using the timer which
|  runs in a seprate thread.
|
------------------------------------------------------------------------------------------*/

PRIVATE void sendScriptLineToTimer(T_LTime delay)
{
   Timer_Run(                       // Queue timer to run 'doScriptLine()'
      delay,                        //  after this delay
      (void(*)(U16))doScriptLine,   // 'Timers' will execute this function
      getDoAtArg(s.linePtr)) ;      // which parses this script line, retrieved from after the colon (:)
}

/*-----------------------------------------------------------------------------------------
|
|  doScriptCmd()
|
|  If 'theLine' is a script command or it's 1st arg is a writeable system scalar
|  then execute the line.
|
|  Returns:
|     '_cmd_OK'      if script command executed successfully
|     '_cmd_Error'   if it did not
|     '_cmd_Quit'    the script engine should quit
|     '_cmd_NoMatch' if 'theLine' wasn't a script command
|                       in which case you should pass it to the user command handler
|
------------------------------------------------------------------------------------------*/

#define InsideLimitsS16(n, l, u)  ((n) >= (l) && (n) <= (u))

PRIVATE BIT fetchScriptLine( C8 *out, C8 CONST *txt, T_LineNum lineNum );

#define _cmd_NoMatch 0
#define _cmd_OK      1
#define _cmd_Error   2
#define _cmd_Quit    3


PRIVATE U8 doScriptCmd( C8 *theLine, U8 autoAdvance )
{
   U8    cmdIdx,
         idx,
         argListLen;

   T_LineNum firstSubLine;

   T_LTime t;

   S16   n;
   BIT   conditionTrue;
   BIT   gotCondition;
   BIT   branched;

   if( (cmdIdx = Str_FindWord(_StrConst(scriptCmds), theLine)) == _Str_NoMatch &&
        !Obj_IsWritableScalar(GetObj(Str_GetNthWord(theLine, 0))) )
   {
      return _cmd_NoMatch;                               // so say failed.
   }
   else                                                  // else line starts with a script command or a writable scalar.
   {
                     // breaking out of repeat loop? AND not a loop control statement?
      if( (BSET(s.curr.flags, _Flags_Break) && cmdIdx != cmd_Until && cmdIdx != cmd_Wend && cmdIdx != cmd_EndRepeat) ||
                     //                            OR
                     // skipping if or else block AND not one of the if-block control statements?
          (BSET(s.curr.flags, _Flags_Skip) && cmdIdx != cmd_Else && cmdIdx != cmd_ElseIf && cmdIdx != cmd_EndIf ) ||
                     //                            OR
                     // reading past subroutine text without executing AND this command isn't 'return'
          (s.skipSub && (cmdIdx != cmd_EndSub)) )
      {
         s.curr.lineNum++;
         return _cmd_OK;                                 // then return OK without executing a command
      }
      else                                               // else don't skip... try to execute this command
      {
         branched = 0;                                   // Default is to advance line counter

         // If a conditional command, preevaluate the condition now - saves code

         s.args = Str_GetNthWord(theLine, 1);            // Get args, everything from 2nd word onward.

         gotCondition = 0; conditionTrue = 0;            // unless set below
         if( Str_WordCnt(s.args) > 0 && isConditionalCmd(cmdIdx))  // are there args AND comamnd is a conditional?
         {
            gotCondition = 1;                            // then assume args are a conditional expression
            conditionTrue = evalCondition(s.args);       // and evaluate the expression
         }

         switch( cmdIdx )
         {
            case cmd_Wait:
               if( (t = getTimeFromStr(s.args)) > 0 )
               {
                  _SetDelay(s.dlyTimer, t);
                  TinySM_SendEvent(&Script_StateM, _Script_Event_Delay);
               }
               else
               {
                  _scriptErrorMsg( "delay not specified" );
                  return _cmd_Error;
               }
               break;

            case cmd_PrintD:
            case cmd_PrintRtn:
            case cmd_PrintDRtn:
               if( Str_FindWord(_StrConst(condFlags), s.args) == _Str_NoMatch )     // 1st arg was NOT e.g 'anyFailed'?
                  {  Comms_WrStr(s.args); }                                         // then write the whole line
               else                                                                 // else 1st arg WAS e.g 'lastFailed'
               {
                  if( evalCondition(s.args) )                                       // Did this arg evaluate to true?
                     { Comms_WrStr(Str_GetNthWord(s.args, 1)); }                    // then write out the remainder of the line
                  else
                     { break; }                                                     // else no printout, quit here
               }

               if(cmdIdx == cmd_PrintD)
                  { printMoreOnCurrentLine = 1; }
               else
                  { printMoreOnCurrentLine = 0; }

               WrStrLiteral(cmdIdx == cmd_PrintD ? "  " : "\r\n");                  // wrote something; finish with spaces or CRLF.
               break;

            case cmd_Quit:
               if( conditionTrue || !gotCondition )
               {
                  sprintf(PrintBuf.buf, "Quit script at line %d \"%s\"\r\n", (S16)s.curr.lineNum, s.args);
                  PrintBuffer();

                  while(1)
                  {
                     if( !fetchScriptLine(lineBuf, s.script, ++s.curr.lineNum) )
                        { return _cmd_Quit; }

                     if( Str_FindWord(lineBuf, "doOnQuit") != _Str_NoMatch )
                        { break; }
                  }
                  break;
               }
               else
                  { break; }

            case cmd_DoOnQuit:                                 // Label 'doOnQuit'
               return _cmd_OK;                                 // Ignore the label, goto next line.

            case cmd_Repeat:
               if( !readInt( s.args, &n) )                     // Fetch the 1st arg; if it exists it should be a loop count
               {
                  s.curr.loop.limit = 0;                       // No loop count, default to 0, which means forever
               }
               else                                            // else 2nd arg was a number
               {
                  if( n > 100 )                                // too large? loop counter is a byte
                     { _scriptErrorMsg( "Loop count was limited to 100 max" );
                       s.curr.loop.limit = 100; }              // then limit to 100
                  else
                     { s.curr.loop.limit = n; }                // else use the number given
               }
               s.curr.loop.start = s.curr.lineNum+1;           // loop starts at 1st line past 'repeat' statement
               break;

            case cmd_EndRepeat:
               if( BSET(s.curr.flags, _Flags_Break) )          // broke inside the loop?
               {
                  CLRB(s.curr.flags, _Flags_Break);            // then clear flag and continue past the 'endrepeat'
               }
               else if( ++s.curr.loop.cnt < s.curr.loop.limit )// else not reached loop count?
               {
                  s.curr.lineNum = s.curr.loop.start;          // then loop back
                  branched = 1;                                // don't advance the line counter
               }
               break;

            case cmd_Until:
               if( BSET(s.curr.flags, _Flags_Break) )          // broke inside the loop?
               {
                  CLRB(s.curr.flags, _Flags_Break);            // then clear flag and continue past the 'until'
               }
               else                                            // else check 'until' condition
               {
                  if( !conditionTrue )                         // and its false?
                  {
                     s.curr.lineNum = s.curr.loop.start;       // then loop back
                     branched = 1;
                  }
               }
               break;

            case cmd_While:
               if( !conditionTrue )
               {
                  SETB( s.curr.flags, _Flags_Break );
               }
               else
               {
                  s.curr.loop.start = s.curr.lineNum;
               }
               break;

            case cmd_Wend:
               if( BSET(s.curr.flags, _Flags_Break) )          // broke inside the loop?
               {
                  CLRB(s.curr.flags, _Flags_Break);            // then clear flag and continue past the 'wend'
               }
               else
               {
                  s.curr.lineNum = s.curr.loop.start;          // then loop back to reevaluate the 'while'
                  branched = 1;
               }
               break;

            case cmd_Continue:
               if( conditionTrue || !gotCondition )
               {
                  s.curr.lineNum = s.curr.loop.start;
                  branched = 1;
               }
               break;

            case cmd_Break:
               if( conditionTrue || !gotCondition )
               {
                  SETB( s.curr.flags, _Flags_Break );
               }
               break;

            // If-elseif-else starts with _Flags_DidAnIf and _Flags_Skip clear (0)

            case cmd_If:
               if( conditionTrue )                             // condition true?
                  { SETB(s.curr.flags, _Flags_DidAnIf); }      // then mark that we executed this block
               else
                  { SETB(s.curr.flags, _Flags_Skip); }         // else skip this block - test / execute next one
               break;

            case cmd_ElseIf:
               if( BSET(s.curr.flags, _Flags_DidAnIf) ||       // already executed an if clause?
                   !conditionTrue )                            // or this condition false
                  { SETB( s.curr.flags, _Flags_Skip ); }       // then skip this block - do else
               else
               {
                  CLRB( s.curr.flags, _Flags_Skip );           // else clear so this block will be executued
                  SETB(s.curr.flags, _Flags_DidAnIf)           // and mark that we executed a block
               }
               break;

            case cmd_Else:
               if( BSET(s.curr.flags, _Flags_DidAnIf) )        // executed any if clause?
                  { SETB( s.curr.flags, _Flags_Skip ); }       // then must skip else
               else
                  { CLRB( s.curr.flags, _Flags_Skip ); }       // otherwise execute else clause
               break;

            case cmd_EndIf:
               CLRB( s.curr.flags, _Flags_Skip );              // reset these flags for the next conditional
               CLRB( s.curr.flags, _Flags_DidAnIf );
               break;

            case cmd_Eval:                                     // Evaluate without printing
               EvalExpr(s.args);
               break;

            case cmd_Pause:
               if( conditionTrue || !gotCondition )            // Unconditional pause? OR condtional and condition is true?
               {
                  s.paused = 1;                                // then set flags which tell engine we are paused.
                  sprintf(PrintBuf.buf, "paused script at line %d; Ctrl-R to resume\r\n", (S16)s.curr.lineNum);
                  PrintBuffer();

                  if( Str_WordInStr(s.args, "beep") )  // Beep too?
                     { doBeeps(3); }                           // then do 3 beeps
               }
               break;

            case cmd_Beep:
               doBeeps(UI_GetScalarArg(s.args, 0, 0.0));       // Do the number of beeps specified in the 1st arg
               break;

            case cmd_WaitFor:
               s.dlyTimer = _Never;

               if( (idx = Str_FindWord(s.args, "upto")) != _Str_NoMatch )   // Wait has a timeout?
               {
                  if( (t = getTimeFromStr( Str_GetNthWord(s.args, idx+1))) > 0 )    // Read how long to wait before timeout?
                  {
                     _SetDelay(s.dlyTimer, t);                                      // then set dealy to that time

                     if( Str_WordInStr(s.args, "quitIfTimeout") )                   // Quit if there's a timeout?
                     {
                        SETB(s.curr.flags, _Flags_QuitIfTimeout);                   // then set a flag for that
                     }
                  }
               }
               TinySM_SendEvent(&Script_StateM, _Script_Event_Wait);                // Send the script engine from 'run' to 'wait'
               break;

            case cmd_ClrFail:
               CLRB(s.curr.flags, _Flags_LastUserCmdFailed);
               break;

            case cmd_DoAt:
               t = getTimeFromStr(s.args) + s.started;         // calc time at which to excute

               sendScriptLineToTimer(
                  t < _Now()                           // ...It's already past time to do it?
                     ? 0                              // then do right now
                     : t - _Now());                    // else do after 't' ticks
               break;

            case cmd_DoAfter:
               sendScriptLineToTimer( getTimeFromStr(s.args));
               break;

            case cmd_Sub:
               if( s.numSubs >= _MaxSubs )                              // Subroutine list already full?
                  { _scriptErrorMsg("Too many subs - can't log reference"); }
               else                                                     // else room for another sub
               {
                  if( Str_WordCharCnt(s.args) >= _SubNameMaxChars )
                     { _scriptErrorMsg("Subroutine name too long - 20 chars max"); }
                  else
                  {
                     Str_CopyNthWord(s.args, s.subs[s.numSubs].name, 0, _SubNameMaxChars);   // note the subroutine name (the 1st arg) in subs[]
                     s.subs[s.numSubs].startLine = s.curr.lineNum+1;                         // and the line number
                     s.numSubs++;                                                            // bump sub count
                  }
               }
               s.skipSub = 1;              // Either way, set flag to read through sub body without executing
               break;

            case cmd_EndSub:
               if( s.skipSub)                               // Was just parsing past this subroutine, instead of executing it?
                  { s.skipSub = 0; }                        // Now have left subroutine body so reeenable command line execution
               else                                         // else subroutine was being executed
                  { popScriptContext(); }                   // so pop context back to caller
               break;

            case cmd_Return:
               if( !s.skipSub )                             // Wasn't parsing past this subroutine? ....
               {                                            // so must have been executing it
                  popScriptContext();                       // Therefore, pop context back to caller
               }
               break;

            case cmd_Call:
               if( !(firstSubLine = getSub1stLine(s.args)) )            // Subroutine not in the sub list?
                  { _scriptErrorMsg("Subroutine not found"); }
               else                                                     // else found subroutine in the sub list
               {
                  /* The argument list is the first 1 to (4) words following 'call'.
                     Str_GetEndWord(str, n) returns the end of the nth word in 'lst' or, if
                     there are fewer than 'n' words', the end of the last word.
                  */
                  argListLen = Str_GetEndWord(s.args, _SubMaxArgs) - Str_GetNthWord(s.args, 1) + 1;

                  if( argListLen > _MaxArgListChars )
                  {
                     _scriptErrorMsg("Argument list must be < 40 chars");
                  }
                  else
                  {
                     pushScriptContext();
                     strncpy((C8*)s.curr.callArgs, (C8*)Str_GetNthWord(s.args, 1), argListLen);  // Copy any args into parms buffer for the new context
                     s.curr.callArgs[argListLen] = '\0';
                     s.curr.lineNum = firstSubLine;                        // and jump to 1st line after the sub header
                     branched = 1;                                         // flag the branch so won't advance past 1st line
                  }
               }
               break;

            case cmd_DoRepeat:
               if( readInt( s.args, &n) )                                  // Fetch the 1st arg; if it exists it should be an integer?
               {
                  if( InsideLimitsS16(n, 1, 254) )                         // Integer is a loop count; must be 1-254
                  {
                     if( (t = getTimeFromStr(Str_GetNthWord(theLine, 2))) == 0 )  // Fetch next arg(s), which should be a time?
                     {
                        _scriptErrorMsg("2nd arg must be a time");         // Say we didn't get a legal time
                     }
                     else                                                  // else got a legal loop count and time
                     {                                                     // So setup timer to repeat the rest of the script line
                        Timer_RunRepeat(                                   // Run repeatedly...
                           t,                                              // after this delay and at this interval
                           (void(*)(U16,U8))doScriptLineUntil,             // Run this function...
                           getDoAtArg(s.linePtr),                          // which will process whatever is after the ':' colon
                           n);                                             // Repeat this many times.

                        if( Str_WordInStr(s.args, "wait") )                // Got 'wait'
                        {
                           s.paused = 1;                                   // then pasue script, will be restarted by timer when done
                        }
                     }
                     break;
                  }
               }
               _scriptErrorMsg("1st arg must be count 1-254");             // Didn't get a legal loop count
               break;

            default:
               /* If we got here it's because the 1st arg in the line was a writeable scalar
                  (see the top of doScriptCmd() ). Try to eval this; it should be an
                  assignment e.g 'Valve3 = 1'. If it isn't an assignment then it will
                  just have no effect.
               */
               EvalExpr(theLine);
               break;
         }                                                        // end: case
         if( !branched && autoAdvance)                         // except for loops and condtionals
            { s.curr.lineNum++; }                            // advance to the next line.
      }
      return _cmd_OK;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  doUserCmd()
|
|  PARAMETERS:    'cmdLine'   - command line to be processed
|
|  Returns:
|     'cmd_OK'       - the line was successfully executed
|     'cmd_Error'    - couldn't parse a word from the line (probably it was empty)
|     'cmd_NoMatch'  - the 1st word wasn't a recognised command
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 doUserCmd( C8 *cmdLine )
{
   U8 rIDATA c;
   S_UICmdSpec CONST * rIDATA cl;         // to scan legal command list for a match
   C8          cmd[_UI_MaxCmdChars];      // holds command pulled from text line

   if( sscanf( (C8*)cmdLine, "%s", cmd ) == 1 )             // Get 1st word - the command
   {
      for( c=0, cl = UICmdList.cmdSpecs; c < UICmdList.numCmds; c++, cl++ ) // Scan the command list
      {
         if( strcmp((C8 CONST*)_StrConst(cl->cmd), (C8*)cmd) == 0 )    // Found a matching command?
         {
            Comms_WroteAStr = 0;                            // Clear this; will be set if the command replies to terminal

            if( Str_WordCnt(cmdLine) < cl->minArgs+1 )      // too few args?
            {
               _scriptErrorMsg("too few args");             // then message
               return _cmd_Error;
            }
            else if( cl->handler )                          // else enough args AND command has a handler (it always should)?
            {
               if( (*cl->handler)(Str_GetNthWord(cmdLine,1)) == 0)      // handler returned fail
               {
                  _scriptErrorMsg("");                      // then message which line failed
                  return _cmd_Error;
               }
            }
            if(Comms_WroteAStr )                            // There was a printed reply from this command?
               { WrStrLiteral(
                  printMoreOnCurrentLine                    // May print more on this line?
                     ? "  "                                 // then insert some spaces
                     : "\r\n"); }                           // else drop to the next line
            return _cmd_OK;                                 // and we're done
         }
      }
      if( c >= UICmdList.numCmds )                          // command not found?
      {
         _scriptErrorMsg("bad cmd");
         return _cmd_NoMatch;                               // then tell the user
      }
   }
   else                                                     // else couldn't read 1st word
   {
      return _cmd_Error;
   }
   return _cmd_OK;                                          // code should never run to here but keeps compiler happy
}


/*-----------------------------------------------------------------------------------------
|
|  nextScriptLine()
|
|  Search 'txt' for the start of the next legal script line and return the start of the line
|  Lines in 'txt' may be separated by CR,LF or both. A line starts with an alphanumeric char
|  and continues until CR or LF. Leading non alphanumeric chars are ignored.
|
|  'nextScriptLine' skips lines which do not have 'legal' script content. Empty lines or lines
|  with comments only are not counted.
|
|  Returns the start of the next script line, else 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE CONST C8 * nextScriptLine( C8 CONST *txt )
{
   U8 state, ch;
   C8 CONST * lineStart;

   #define _start    0
   #define _line     1
   #define _comment  2
   #define _currLine 3

   #define _CommentCh ';'

   state = _currLine;                              // enter looking for start of a line

   while(1)
   {
      File_EnterTextBank();
      ch = *txt;
      File_LeaveTextBank();

      if( ch == '\0' )                             // end of source string?
      {
         if( state == _line )                      // parsing next script line now?
            { break; }                             // so break to return the line
         else                                      // else don't have next script line yet
            { return 0; }                          // so 0 => failed
      }
      if( state == _currLine )                     // parsing through current line (to get to the next one)
      {
         if( ch == '\r' || ch == '\n' )            // end of line?
         {
            state = _start;                        // terminates a comment
         }
      }
      else if( state == _start )                   // looking for start of next line
      {
         if( ch == _CommentCh )                    // start of comment?
         {
            state = _comment;
         }
         else if( !isspace(ch) )                   // not a space, tab or return?
         {
            state = _line;                         // then its start of a new line
            lineStart = txt;                       // mark the start position
         }
      }
      else if( state == _comment )                 // in the middle of a comment?
      {
         if( ch == '\r' || ch == '\n' )            // end of line?
         {
            state = _start;                        // terminates a comment
         }
      }
      else                                         // else state == _line, reading through a line
      {
         if( ch == _CommentCh || ch == '\r' || ch == '\n' )  // comment or CRLF?
         {
            break;                                 // break to output the line
         }
      }
      txt++;                                       // advance through source string
   }

   // Break.. so got a script line. Check length and return the start of the line

   if((U8)(txt - lineStart) > _MaxCmdLineChars)
   {
      _scriptErrorMsg("line > 100 chars");
      return 0;
   }
   else
   {
      return lineStart;
   }
   #undef _start
   #undef _line
   #undef _comment
}


/*-----------------------------------------------------------------------------------------
|
|  copyScriptLineToStr()
|
|  Copy the the current script line to a null-terminated string. Comments and trailing
|  whitespace are not copied.
|
|  If a script line exceeds '_MaxCmdLineChars' then it is truncated (but still null-terminated)
|
------------------------------------------------------------------------------------------*/

PRIVATE void copyScriptLineToStr( C8 *str, C8 CONST *txt )
{
   U8 ch, idx, firstSpace;
   BIT wasAlpha;

   for( idx = 0, wasAlpha = 0, firstSpace = 0;; idx++ )
   {
      File_EnterTextBank();
      ch = txt[idx];
      File_LeaveTextBank();

      if( idx > 100 )
      {
         str[0] = '\0';
         return;
      }

      if( idx >= _MaxCmdLineChars ||         // line too long?
          ch == _CommentCh ||                // start of (trailing) comment?
          ch == '\r' || ch == '\n' ||        // OR end of line?
          ch == '\0' )                       // OR end of entire script?
      {
         if( firstSpace )                    // there was a space after most recent (script) word?
            { str[firstSpace] = '\0'; }      // then string-terminate on that space
         else
            { str[idx] = '\0'; }             // else string-terminate at the current position
         return;
      }
      else                                   // else script portion of this line continues
      {
         if( ch != ' ' )                     // inside a script word?
         {
            firstSpace = 0;                  // clear this
            wasAlpha = 1;                    // and mark we're in a word
         }
         else                                // else in whitspace
         {
            if( wasAlpha )                   // last char wasn't whitespace
            {
               firstSpace = idx;             // so this char is the 1st trailing space
            }
            wasAlpha = 0;                    // clear flag
         }
         str[idx] = ch;                      // copy this char to string
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  fetchScriptLine()
|
|  Copy line 'lineNum' from multiple-line string 'txt' into null terminated string in 'out'.
|  Lines in 'txt' may be separated by CR,LF or both. A line starts with an alphanumeric char
|  and continues until CR or LF. Leading non alphanumeric chars are ignored.
|
|  Comments start with ';' and continue until end-of-line. They are not copied to 'out'.
|
|  'lineNum' counts only lines with 'legal' content which to be copied to 'out'. Empty lines
|  or lines with comments only are not counted.
|
|  Return 1 if copied a line to out, else 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT fetchScriptLine( C8 *out, C8 CONST *txt, T_LineNum lineNum )
{
   // For efficiency cache the number and addr of the last-fetched line. Otherwise the script
   // engine must parse from the start of the script for each new line. Very slow if the
   // script is long.

   static T_LineNum lineCnt = 0;

   if(lineNum == 0 || lineNum < lineCnt)           // either just started or looped back?
   {
      lineCnt = 0;                                 // then must (re)parse from start of script
      s.linePtr = txt;
   }                                               // otherwise continue forward form the current position

   for( ;; lineCnt++ )                             // for each script line, starting at 'txt'
   {
      if(lineCnt == lineNum)                       // is the line we want?
      {
         copyScriptLineToStr(out, s.linePtr);    // then copy it out, minus comments
         return 1;                                 // and say success
      }
      else if( !(s.linePtr = nextScriptLine(s.linePtr) ) )  // else try to get next script line (skips comments and empty lines)
      {                                            // no more lines; so..
         out = '\0';                               // didn't get requested line => return empty string
         return 0;                                 // and say failed!
      }
   }
   return 0;
}


/*-----------------------------------------------------------------------------------------
|
|  doScriptLine()
|
------------------------------------------------------------------------------------------*/

PRIVATE void doScriptLine( C8 CONST *theLine )
{
   copyScriptLineToStr(lineBuf, theLine);

   if( doScriptCmd(lineBuf, 0) == _cmd_NoMatch )         // line was not a script command?
   {
      if( !s.skipSub )
         { doUserCmd(lineBuf); }                         // then try to run it as a user command
   }
}







/*-----------------------------------------------------------------------------------------
|
|  markCommandFailed()
|
------------------------------------------------------------------------------------------*/

PRIVATE void markCommandFailed(void)
{
   s.curr.flags |= _Flags_LastUserCmdFailed | _Flags_AnyUserCmdFailed;
}




/*-----------------------------------------------------------------------------------------
|
|  resetScriptEngine()
|
------------------------------------------------------------------------------------------*/

PRIVATE void resetScriptEngine(void)
{
   s.curr.lineNum = 0;
   s.curr.flags = 0;
   s.paused = 0;
   s.numSubs = 0;
   s.skipSub = 0;
   s.stackCnt = 0;
   ZeroBytesU8( (void RAM_IS *)&s.curr.loop, sizeof(S_LoopCtl));  // Clear loop control until encounter a loop in the script
   s.started = _Now();    // mark the start time
   printMoreOnCurrentLine = 0;
}


/*-----------------------------------------------------------------------------------------
|
|  postScriptEvent()
|
------------------------------------------------------------------------------------------*/

PRIVATE void postScriptEvent(U8 event)
{
   Link_SendEvent( (T_ObjAddr)&Script_StateM, event);
}

/*-----------------------------------------------------------------------------------------
|
|  stopScript_PrintMsg().
|
|  Actions upon stoppng a script
|
------------------------------------------------------------------------------------------*/

PRIVATE void stopScript_PrintMsg(U8 CONST *msg)
{
   WrStrLiteral(msg);
   Script_Running = 0;                    // First clear the public status flag.
   postScriptEvent(_Script_Event_Stop);   // then tell anyone who needs to know (and may read the flag)
}

// ============================= end: PRIVATE ======================================





// ----------------- Script Processing State Machine --------------------------------

#define _sBoot    1     // check for a boot script a startup
#define _sStop    2     // waiting to run a script
#define _sRun     3     // running a script
#define _sDelay   4     // a delay period inside script
#define _sUserIn  5     // the script is waiting for keyboard input
#define _sWait    6     // waiting for an action or true expression


/*-----------------------------------------------------------------------------------------
|
|  entryFunc()
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 entryFunc(S_TinySM_EventList *events)
{
   if( TinySM_GetEvent(events, _Script_Event_Stop) )           // User aborted script? (Ctrl-Q)
   {
      stopScript_PrintMsg("exiting script\r\n");
      return _sStop;
   }
   else                                                        // else script is still sctive
   {
      if( TinySM_GetEvent(events, _Script_Event_Pause))
      {
         s.paused = 1;
         WrStrLiteral("pausing script; Ctrl-R to resume\r\n");
         postScriptEvent(_Script_Event_Pause);
      }
      if( TinySM_GetEvent(events, _Script_Event_Resume))
      {
         s.paused = 0;
         // This message seems unecessary. It clutters terminal printouts. Defeat it for now
         // WrStrLiteral("resuming script; Ctrl-P to pause\r\n");
         postScriptEvent(_Script_Event_Resume);
      }
      return TinySM_State_Unchanged;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  bootFunc()
|
|  If there's text in File 0, run it as a script.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 bootFunc(S_TinySM_EventList *events)
{
   if( (s.script = File_Read(0)) != 0 )      // file 0 not empty?
   {
      WrStrLiteral("Running (boot) script 0\r\n   Ctr-Q to quit; Ctrl-P to pause\r\n");
      s.mode = _Mode_Default;                // for now there's just one mode
      resetScriptEngine();
      Script_Running = 1;
      postScriptEvent(_Script_Event_Run);
      return _sRun;
   }
   else                                      // else file 0 is empty
   {
      return _sStop;                         // so goto wait
   }
}

/*-----------------------------------------------------------------------------------------
|
|  stopFunc()
|
|  Waiting to run a script
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 stopFunc(S_TinySM_EventList *events)
{
   if( TinySM_GetEvent(events, _Script_Event_Run))
   {
      resetScriptEngine();
      Script_Running = 1;                                   // we are running one now
      postScriptEvent(_Script_Event_Run);
      return _sRun;
   }
   else
   {
      return _sStop;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  runFunc()
|
------------------------------------------------------------------------------------------*/

#define _MaxSubroutineArgs 6        // Must match argLabels[] below

#if _TOOL_IS == TOOL_RIDE_8051 || _TOOL_IS == TOOL_Z8_ENCORE || _TOOL_IS == TOOL_CC430 || _TOOL_IS == TOOL_GCC_ARM || _TOOL_IS == TOOL_GCC_X86
PRIVATE C8 CONST * CONST argLabels[] = {"#1", "#2", "#3", "#4", "#5", "#6" };
#endif


PRIVATE U8 runFunc(S_TinySM_EventList *events)
{
   U8 c, rtn;

   if( s.paused )                                                 // paused?
   {
      return _sRun;                                               // then skip on by
   }
   else                                                           // else process the script
   {
      if( fetchScriptLine( lineBuf, s.script, s.curr.lineNum ) )  // got another line?
      {
         if(s.stackCnt)                                           // Within a subroutine call?
         {
            for( c = 0; c < RECORDS_IN(argLabels); c++)              // for each possible arg (1-6)
            {
               if( Str_WordInStr(lineBuf, argLabels[c]) )  // #c is in the script line?
               {
                  if( Str_WordCnt(s.curr.callArgs) <= c)          // But... Not enough args were passed?
                  {
                     _scriptErrorMsg("Too few args in subroutine call");
                  }
                  else                                            // else there are enough args
                  {
                     Str_Replace(lineBuf, argLabels[c], Str_GetNthWord(s.curr.callArgs, c), 1, 1);
                  }
               }
            }
         }

         if( (rtn = doScriptCmd(lineBuf, 1)) == _cmd_NoMatch )    // line was not a script command?
         {
            if( ((s.curr.flags & (_Flags_Break|_Flags_Skip)) == 0) && !s.skipSub)  // and don't skip user commands?
            {
               CLRB(s.curr.flags, _Flags_LastUserCmdFailed);      // Clear mark before trying user command

               if( doUserCmd(lineBuf) == _cmd_Error )             // then try to run it as a user command
               {
                  markCommandFailed();
               }                                                  // action depends on script.. it may quit
            }
            s.curr.lineNum++;                                     // executed or not, advance to the next script line
         }
         else if( rtn == _cmd_Error )                             // error executing a script command
         {
         }
         else if( rtn == _cmd_Quit )
         {
            stopScript_PrintMsg("");                              // Already printed a 'quit' message with a line number
            return _sStop;
         }

         /* Handle any events which may have been posted from script commands ( inside doScriptCmd()
            or doUserCmd() ). Do this now as, by default, unhandled events are cleared upon exiting
            the state function.
         */
         if( TinySM_GetEvent(events, _Script_Event_Delay))        // got a delay command?
            { return _sDelay; }
         else if( TinySM_GetEvent(events, _Script_Event_Wait))    // wait on a condition?
            { return _sWait; }
         else
            { return _sRun;  }

      }
      else
      {
         stopScript_PrintMsg("End of script\r\n");
         return _sStop;
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  delayFunc()
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 delayFunc(S_TinySM_EventList *events)
{
   if( Timeout( s.dlyTimer ) && !s.paused )
   {
      return _sRun;
   }
   else
   {
      return _sDelay;
   }
}



/*-----------------------------------------------------------------------------------------
|
|  inputFunc()
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 inputFunc(S_TinySM_EventList *events)
{
   return _sUserIn;
}



/*-----------------------------------------------------------------------------------------
|
|  waitFunc()
|
|  State entered upon getting a script 'wait' command.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 waitFunc(S_TinySM_EventList *events)
{
   if( s.paused )                      // paused?
   {
      return _sWait;                   // then stay in wait, don't evaluate exit conditions
   }
   else
   {
      if( Timeout( s.dlyTimer ) )      // timed out with expr still false?
      {
         if( BSET(s.curr.flags, _Flags_QuitIfTimeout) )
         {
            stopScript_PrintMsg( "script wait timed out" );
            return _sStop;
         }
         else
         {
            markCommandFailed();       // then mark as failed
            return _sRun;              // and continue down the script
         }
      }
      else if( EvalExpr(s.args) )      // expression now true?
      {
         return _sRun;                 // then continue down the script
      }
      else
      {
         return _sWait;                // else keep waiting
      }
   }
}


PRIVATE U8(* CODE stateFuncs[])(S_TinySM_EventList*) =
{
   entryFunc,     // handles Quit command
   bootFunc,      // at startup
   stopFunc,      // waiting to start / restart a script
   runFunc,       // exectuing a line of script
   delayFunc,     // implementing a delay specified in a script line
   inputFunc,     // Handles user input
   waitFunc       // Handles 'Wait' command.
};

PUBLIC S_TinySM Script_StateM;

PUBLIC S_TinySM_Cfg CONST Script_StateM_Cfg =
{
   RECORDS_IN(stateFuncs) - 1,   // number of state functions (entry function is excluded)
   0,                            // see note below
   _TicksSec_S(0.02),            // send tick every 20 msec
   stateFuncs,                   // list of state functions
   0                             // no init
};

/* *** N.B The script command line interpreter (CLI) sends events to the script state
   machine. These events are processed the NEXT time the state engine runs. Since the
   CLI runs inside the state machine the state machine musn't clear events automatically
   upon exit otherwise it will wipe CLI events.
*/


// 1 if running a script, else 0
PUBLIC BIT Script_Running = 0;


/*-----------------------------------------------------------------------------------------
|
|  UI_Script()
|
------------------------------------------------------------------------------------------*/

typedef enum { action_Run, action_Write, action_List } E_Actions_UI;
PRIVATE C8 CONST actionList[] = "run write list";

   #ifdef INCLUDE_HELP_TEXT
PUBLIC C8 CONST UI_Script_Help[] = "\
Usage: <script_number>  <'run' | 'write' | 'list'>\r\n\
\r\n\
Examples:  'script 1 run'   Run script in txtfile 1\r\n\
           'script 1 write' Write script (from terminal) to txtfile 1, stripping any comments\r\n\
           'script 2 list'  List script in txtfile 2\r\n\
\r\n\
   Ctrl-Q aborts an executing script\r\n\
   Ctrl-P to pause; Ctrl-R to resume\r\n\
\r\n\
";
   #endif // INCLUDE_HELP_TEXT

extern void File_SetupWrite(void);
extern S_TxtFile TxtF;

PUBLIC U8 UI_Script(C8 *args)
{
   U8  action,
       scriptNum;

   if( Str_WordInStr(args, "ver"))
   {
      sprintf( PrintBuf.buf, "Script Engine Rev is %s\r\n", _ScriptEngineRev);
      PrintBuffer();
      return 1;
   }
   else if( (action = UI_GetAction_MsgIfFail(args, actionList)) == _UI_NoActionMatched )  // 2nd arg is legal action?
      { return 0; }                                                           // quit of not
   else
   {
      if( !File_LegalBank_MsgIfNot(args, &scriptNum) )   // File name/number is legal?
         { return 0; }                                   // quit if not
      else                                               // else file name/number is legal
      {
         TxtF.currentBank = scriptNum;                   // Set current bank to that specified in command line

         if( action == action_Write )                    // Write a new script to this file?
         {
            File_SetupScriptWrite();                     // then setup to input files, stripping comments
         }
         else                                            // else list or run an exisitng script
         {
            if( !File_Read(scriptNum) )                  // This file has (text) content?
            {                                            // No... say the file is empty?
               sprintf( PrintBuf.buf, "No script of number %d\r\n", scriptNum);
               PrintBuffer();
               return 0;
            }
            else                                         // else this file has text
            {
               switch( action )
               {
                  case action_Run:                       // Either try to run the text as a script. OR...
                     Script_Run(scriptNum);
                     break;

                  case action_List:
                     File_List();                        // List the contents to the terminal.
                     break;
               }
            }
         }
      }
   }
   return 1;
}


/*-----------------------------------------------------------------------------------------
|
|  Script_Run
|
|  Run the script in file 'scriptNum'
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Script_Run(U8 scriptNum)
{
   if( !(s.script = File_Read(scriptNum)) )                    // Got (new) script text is at this address?
   {
      sprintf( PrintBuf.buf, "No script in File %d\r\n", scriptNum);
      PrintBuffer();
      return 0;                                                // and return fail
   }
   else
   {
      if( Script_Running == 1 )                                // already running a script?
      {        // this means the current script called a new one OR
               // the user switched scripts midstream using the keyboard
         sprintf( PrintBuf.buf, "Transfer to script %d\r\n", scriptNum);
         PrintBuffer();
         resetScriptEngine();                                  // then switch to this one (and remain in run)
       }
       else                                                     // else not currently running a script
       {
         sprintf( PrintBuf.buf, "Running script %d\r\n  Ctr-Q to quit; Ctrl-P to pause\r\n", scriptNum);
         PrintBuffer();
         s.mode = _Mode_Default;                               // for now there's just one mode
         TinySM_SendEvent(&Script_StateM, _Script_Event_Run);  // and start the script engine
      }
      return 1;                                                // and return success
   }
}



/*-----------------------------------------------------------------------------------------
|
|  Script_MarkCommandFailed
|
------------------------------------------------------------------------------------------*/

PUBLIC void Script_MarkCmdFailed(void)
{
   SETB(s.curr.flags, _Flags_LastUserCmdFailed);
}


/*-----------------------------------------------------------------------------------------
|
|  Script_HandleCtrlCh
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Script_HandleCtrlCh( U8 ch )
{
   #define CtrlP 0x010
   #define CtrlQ 0x011
   #define CtrlR 0x012

   if( Script_Running )
   {
      switch( ch )
      {
         case CtrlP:
            TinySM_SendEvent(&Script_StateM, _Script_Event_Pause); // then send pause event
            return 1;

         case CtrlQ:
            TinySM_SendEvent(&Script_StateM, _Script_Event_Stop); // then send quit event
            return 1;

         case CtrlR:
            TinySM_SendEvent(&Script_StateM, _Script_Event_Resume); // then send resume event
            return 1;
      }
   }
   return 0;
}


/*-----------------------------------------------------------------------------------------
|
|  Script_Paused
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Script_Paused(void) { return s.paused; }


/*-----------------------------------------------------------------------------------------
|
|  Script_CurrentLine
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 Script_CurrentLine(void) { return s.curr.lineNum; }


/*-----------------------------------------------------------------------------------------
|
|  Script_ReadElapsed
|
|  Return the time since script started in _IU_Sec(). If no script active then return 0.
|
|  At present _IU_Sec is 1/10ths second. So the function will return up to 3200 secs,
|  about 50 minutes. If the script runs longer than that then the value returned is
|  clipped to MAX_S16 (32768)
|
------------------------------------------------------------------------------------------*/
#define ElapsedL(t)  (Elapsed(&t))

PUBLIC S16 Script_ReadElapsed(void)
{
   if( !Script_Running )
   {
      return 0;
   }
   else
   {
      return ClipS32toS16( ElapsedL(s.started)/(TicksPerSec/_IU_Sec(1)) );
   }
}

PUBLIC S16 Script_ElapsedMinutes(void)
{
   if( !Script_Running )
   {
      return 0;
   }
   else
   {
      return ClipS32toS16( ElapsedL(s.started)/(60 * TicksPerSec) );
   }
}

// ----------------------------------- eof ----------------------------------------------

