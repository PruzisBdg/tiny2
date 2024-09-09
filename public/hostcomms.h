/*---------------------------------------------------------------------------
|
|         Test Fixture Support - Host Interface
|
|--------------------------------------------------------------------------*/

#ifndef HOSTCOMMS_H
#define HOSTCOMMS_H

#include "common.h"
#include "vec.h"
#include "tiny_statem.h"

PUBLIC U8 Comms_WrBuf( U8 GENERIC const *buf, U8 len);    // Write buffer

#if _HAS_TEXT_UI == 1

// the Host interface state machine
extern S_TinySM CommsIn_StateM;                      
extern S_TinySM_Cfg CONST CommsIn_StateM_Cfg;

#define _WrTxt_Flags_OneLine  _BitM(0)    // Write just a line, including CRLF
#define _WrTxt_Flags_OneWord  _BitM(1)    // Write just a word


#define _WrStr_AddLF _BitM(0)
#define _WrStr_Gated _BitM(1)
PUBLIC U8 WrStr( C8 GENERIC const *str, U8 flags );   // Write a line
extern BIT Comms_WroteAStr;                           // Is set when a string is written

#if _TOOL_IS == TOOL_RIDE_8051
   #define WrStrConst_Flag WrStr
#else
   PUBLIC U8 WrStrConst_Flag( U8 CONST *str, U8 flags );
#endif

PUBLIC U8   Comms_WrLine( U8 GENERIC const *str );    // Write a line; terminate with CR-LF

PUBLIC U8 Comms_WrStr( C8 GENERIC const *str );       // Print a string to Host
PUBLIC U8 Comms_WrStr_Gated( C8 GENERIC const *str ); // Print only if Host is not sending chars

PUBLIC void Comms_WrStrConst( C8 CONST *str );        // Same as Comms_WrStr but for CONST - saves code
PUBLIC void Comms_WrStrConst_Gated( C8 CONST *str );  // and the 'gated' version

PUBLIC U8   Comms_PutChar( U8 ch );                         // write single char to the Host

PUBLIC void PrintBuffer(void);                        // send print buffer to Host
PUBLIC void PrintBuffer_Gated(void);                  // and the 'gated' version

PUBLIC void Comms_Printf_ConstFmt(C8 CONST *fmt, ...);

PUBLIC U8   Comms_TxBytesFree(void);                  // bytes free in the TX buffer

#if _TOOL_IS == TOOL_Z8_ENCORE

   // Zilog ZDS compiler string literals - are prefixed with memspace
   #define _NearStr(s) N##s
   #define _FarStr(s)  F##s
   #define _RomStr(s)  ((U8 CONST *)(R##s))

   #define WrStrConst(str) Comms_WrStrConst(str)
   #define WrStrLiteral(str)  Comms_WrStrConst((U8 CONST *)_RomStr(str))

#elif _TOOL_IS == TOOL_CC430

   #define WrStrConst Comms_WrStrConst
   #define WrStrLiteral(str)  Comms_WrStrConst((C8 const *)(str))

#else
   #define WrStrConst Comms_WrStrConst
   #define WrStrLiteral Comms_WrStrConst

#endif

#elif _HAS_TEXT_UI == 0
   #define Comms_PutChar(ch)
   #define WrStr(str, flags)
   #define Comms_WrStr(str) 
   #define Comms_WrStr_Gated(str) 
   #define Comms_WrStrConst(str) 
   #define Comms_WrStrConst_Gated(str) 
   #define PrintBuffer()
#else
   #error "_HAS_TEXT_UI must be defined, 1 or 0"
#endif

// Print Buffer.. enough to print longest reply or help string. Currently counted by a byte
// so 254 bytes max.
#define _PrintBufLen 220

#if _TOOL_IS == TOOL_CC430
typedef struct
   {
      U16 len;
      C8  buf[_PrintBufLen];
   } S_PrintBuf;
#else
typedef struct
   {
      U16 len;
      U8  buf[_PrintBufLen];
   } S_PrintBuf;
#endif

extern S_PrintBuf PrintBuf;

PUBLIC int tty_printf(const char GENERIC *format, ...);

#endif // HOSTCOMMS_H 
