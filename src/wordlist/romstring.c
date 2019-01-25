/*---------------------------------------------------------------------------
|
|  ROM String support for Z8 and other Harvard machines without typed pointers
|
|  ROM strings are copied into a round robin buffer so they can be used by
|  functions which require strings in RAM; e.g printf(), strcat().
|
|  Pruzina 3/22/06
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "romstring.h"
#include <string.h>

#if 0
PUBLIC void strcpy_C(U8 *dest, U8 CONST *src)
{
   for( ; *src != '\0'; dest++, src++ ) *dest = *src;
   *dest = '\0';
}

PUBLIC U16 strlen_C(U8 CONST *str)
{
   U16 n;

   for( n = 0; *str != '\0'; n++, str++ )
   return n;
}

PUBLIC void strcat_C(U8 *dest, U8 CONST *src)
{
   strcpy_C( dest + strlen(dest), src );
}
#endif

#if 1

typedef U16 T_BufSize;

typedef struct
{
   C8         *buf;      // the buffer
   T_BufSize  len,       // its size
              free;      // next free
} S_StrBuf;

/*-----------------------------------------------------------------------------------------
|
|  RamStr()
|
|  Copy a ROM string to a RAM buffer and return that buffer. This allows generic string
|  functions to use ROM strings in the Z8 under ZDSII compiler.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 * RamStrCore(S_StrBuf *b, U16 src, U8 cnt, U8 isROM)
{
   U8 c, ch;
   T_BufSize start;

   for( c = 0, start = b->free;; )                    // for each char to be copied
   {
      if( b->free >= b->len-1 )                       // at end of buffer?
      {
         if( c == b->free )                           // and source string was copied into start of buffer?
         {                                            // then source string is too big for buffer
            b->buf[b->len-1] = '\0';                  // just terminate what we got so far
            return b->buf;                            // and return the (truncated) string
         }
         else                                         // else source string was copied into middle of buffer
         {
            b->free = 0;                              // so reset free ptr
            start = 0;                                // mark revised string start
            c = 0;                                    // and reset source counter
            continue;                                 // and start over, copying into start of buffer
         }                                            
      }
      else                                            // else not at end of buffer
      {
         if( isROM )                                  // source is ROM?
            { ch = ((U8 CONST*)src)[c]; }             // so get char from ROM (LDC)
         else 
            { ch = ((U8*)src)[c]; }                   // else get form RAM (LDX)

         b->buf[b->free] = ch;                        // write char to buffer                     
         c++;                                         // bump source count
         b->free++;                                   // bump dest cnt past char.

         if( (!cnt && ch == '\0') ||                  // source is a string? AND end of this string?
              (cnt && cnt == c) )                     // OR cnt specifies source? length AND have reached end of source?
         {                                            // then done!
            return b->buf + start;                    // return start of this string in the buffer
         }
         else                                         // else more source to copy
         {
                                                      // so continue
         }
      }
   }
   return b->buf;                                     // will never return from here.. but keeps compiler happy
}

#define _StringBufBytes 400
PRIVATE C8 stringBuf[_StringBufBytes];    // to hold strings from ROM

PRIVATE S_StrBuf strBuf1 = {stringBuf, _StringBufBytes, 0};


PUBLIC C8 * RamStr(U8 CONST *src)
{
   return RamStrCore(&strBuf1, (U16)src, 0, 1);
}

PUBLIC C8 * RamStrCnt(U16 src, U8 cnt, U8 isROM)
{
   return RamStrCore(&strBuf1, src, cnt, isROM);
}

#else

/*-----------------------------------------------------------------------------------------
|
|  RamStr()
|
|  Copy a ROM string to a RAM buffer and return that buffer. This allows generic string
|  functions to use ROM strings in the Z8 under ZDSII compiler.
|
------------------------------------------------------------------------------------------*/

#define _StringBufBytes 150
PRIVATE U8 stringBuf[_StringBufBytes];    // to hold strings from ROM
PRIVATE U8 free = 0;                      // next free space in buffer

PUBLIC U8 * RamStr(U8 CONST *src)
{
   U8 c, start;

   for( c = 0, start = free;; )                       // for each char to be copied
   {
      if( free >= _StringBufBytes-1 )                 // at end of buffer?
      {
         if( c == free )                              // and source string was copied into start of buffer?
         {                                            // then source string is too big for buffer
            stringBuf[_StringBufBytes-1] = '\0';      // just terminate what we got so far
            return stringBuf;                         // and return the (truncated) string
         }
         else                                         // else source string was copied into middle of buffer
         {
            free = 0;                                 // so reset free ptr
            start = 0;                                // mark revised string start
            c = 0;                                    // and reset source counter
            continue;                                 // and start over, copying into start of buffer
         }                                            
      }
      else if( (stringBuf[free] = src[c]) == '\0' )   // do copy... end of source string?
      {
         free++;                                      // free starts one beyond '\0'
         return stringBuf + start;                    // done! return start of this string in the buffer
      }
      else                                            // else copy more
      {
         c++; free++;                                 // so bump source and dest indices
      }
   }
   return stringBuf;                                  // will never return from here.. but keeps compiler happy
}


#endif
// ------------------------------------------ eof ------------------------------------------
