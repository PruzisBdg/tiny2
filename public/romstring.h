/*---------------------------------------------------------------------------
|
|     Tiny1 - ROM String support for Z8 and other Harvard machines
|
|  This brings ROM strings across in RAM so they can be used by functions
|  which only address RAM. Also, Zilog has tags to specifiy the memory space
|  for string literals.
|
|--------------------------------------------------------------------------*/

#ifndef ROMSTRING_H
#define ROMSTRING_H

#include "libs_support.h"
#include "common.h"

#ifndef _TOOL_IS
       #define _StrConst(s) (s)
       #define _StrLit(s) s
       #define _RomStr(s) (s)
#else

    #if _TOOL_IS == TOOL_Z8_ENCORE

       // Zilog ZDS compiler string literals - are prefixed with memspace
       #define _NearStr(s) N##s
       #define _FarStr(s)  F##s
       #define _RomStr(s)  ((U8 CONST *)(R##s))

       // ROM strings copied to RAM so generic string functions can use RAM strings
       PUBLIC C8 const * RamStr(U8 CONST *src);

       #define _StrConst(s) (RamStr((s)))
       #define _StrLit(s)  _StrConst(_RomStr(s))

    #elif _TOOL_IS == TOOL_CC430
       #define _StrConst(s) (s)
       #define _StrLit(s) s
       #define _RomStr(s) ((U8 const *)(s))
    #else
       #define _StrConst(s) (s)
       #define _StrLit(s) s
       #define _RomStr(s) (s)
    #endif
#endif

#endif // ROMSTRING_H
