/* ---------------------------------------------------------------------
|
|  Small MCU <stdio>
|
|  These are compact printf()s etc for small MCU's. printf() support for small
|  MCU's is pretty variable; sometimes there are compact quality libraries,
|  other times they're a monster which is barely usable on the target. Sometimes
|  the low-level UART interface is what you need; other times it's not and
|  inserting your own is a project.
|
|  These printf()s are compact and buffer-less. They are far from completely
|  ANSI, but have all you need for embedded debug. They build to full, non-float
|  and super-tiny versions.
|
|  To use these, your application must supply putchar() and putBuf() to be linked
|  into this lib.
|
|     for printf().... extern void TPrint_PutCh(U8 ch);
|
|     for sprintf().... extern C8 *TPrint_BufPtr;
|                       extern void TPrint_ChIntoBuf(U8 ch);
|
|   $Revision$
|
|   $Id$
|
|--------------------------------------------------------------------------*/

#ifndef TINY1_STDIO_H
#define TINY1_STDIO_H
#include <stdarg.h>

// --------------------------- Versions -------------------------------------

#define TPRINT_FLOAT    1
#define TPRINT_NO_FLOAT 2     // Just integers
#define TPRINT_TINY     3     // No longs, Formats and printouts 255 bytes max..

// The cross-checks below are because we often use funky build tricks to include this module.
#ifdef TPRINT_IS_FLOAT
   #define _TPRINT_IS TPRINT_FLOAT

   #ifdef TPRINT_IS_NO_FLOAT
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
   #ifdef TPRINT_IS_TINY
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
#endif

#ifdef TPRINT_IS_NO_FLOAT
   #define _TPRINT_IS TPRINT_NO_FLOAT

   #ifdef TPRINT_IS_FLOAT
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
   #ifdef TPRINT_IS_TINY
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
#endif

#ifdef TPRINT_IS_TINY
   #define _TPRINT_IS TPRINT_TINY

   #ifdef TPRINT_IS_FLOAT
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
   #ifdef TPRINT_IS_NO_FLOAT
      #error "There are conflicting definitions of TPRINT_IS_"
   #endif
#endif

#ifndef _TPRINT_IS
   #error "Specify _TPRINT_IS float/no-float/tiny  (In your bldcfg.h)"
#endif

#include "GenericTypeDefs.h"  // S16, PUBLIC

#if _TPRINT_IS == TPRINT_FLOAT 
   #include <float.h>
#endif

#ifndef _TOOL_IS
   #error "Must define _TOOL_IS"
#else
   #if _TOOL_IS == TOOL_RIDE_STM32     // RIDE Arm, 
      #define FMT_QUALIFIER            // printf() formatter is not const
      typedef int T_PrintCnt;          // printf() sprintf() return 'int' (32bit)
   #else                               // Raisonance C8051 and TI MPS340 Code Composer, 
      #define FMT_QUALIFIER CONST      // printf() formatter is CONST i.e in 8051 code space
      
      // printf(), sprintf() return is 8 or 16bits depending on build.
      #if _TPRINT_IS == TPRINT_FLOAT || _TPRINT_IS == TPRINT_NO_FLOAT
         typedef S16 T_PrintCnt;
		 #define _Max_T_PrintCnt MAX_S16
      #elif _TPRINT_IS == TPRINT_TINY  
         typedef U8 T_PrintCnt;					// Prints only 255 chars
		 #define _Max_T_PrintCnt MAX_U8
      #else
         #error "Must define _TPRINT_IS for T_PrintCnt"
      #endif
   #endif
#endif

/* printf() and sprintf(). They are prefixed because the MicroChip MPLabX, at least, doesn't let
   you override the library printf()  and sprintf().  Use #def to rename calls to these to the 'tiny1_'
   versions here.
*/
typedef struct { C8 *put; T_PrintCnt cnt, maxCh; } tiny1_S_SafeSprintf;
typedef struct { C8 *buf; T_PrintCnt maxCh; } tiny1_S_SafeBuf;

PUBLIC T_PrintCnt tiny1_sprintf(C8 *buf, C8 FMT_QUALIFIER *fmt, ...);
PUBLIC T_PrintCnt tiny1_sprintf_safe(tiny1_S_SafeBuf const *out, C8 FMT_QUALIFIER *fmt, ...);
PUBLIC T_PrintCnt tiny1_printf(C8 FMT_QUALIFIER *fmt, ...);
PUBLIC T_PrintCnt tprintf_internal(void (*putChParm)(U8), C8 CONST *fmt, va_list arg);

	#ifdef TPRINT_USE_STRING_HEAP
PUBLIC C8 const * tiny1_print_heap1w(C8 FMT_QUALIFIER *fmt, ...);
	#endif
	
#define  EOF   (-1)

#endif // TINY1_STDIO_H

