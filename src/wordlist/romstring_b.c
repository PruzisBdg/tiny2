/*---------------------------------------------------------------------------
|
|     ROM String support for Z8 and other Harvard machines
|
|  Pruzina 3/22/06
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "romstring.h"
#include <string.h>

/*-----------------------------------------------------------------------------------------
|
|  RamStr_B()
|
|  A 2nd instantiation of RamStr()
|
|  Copy a ROM string to a RAM buffer and return that buffer. This allows generic string
|  functions to use ROM strings in the Z8 under ZDSII compiler.
|
------------------------------------------------------------------------------------------*/


#define _StringBufBytes 80

PRIVATE U8 stringBuf[_StringBufBytes];

PUBLIC U8 * RamStr_B(U8 CONST *src)
{
   U8 c;

   /* Copy 'src' string to 'stringBuf', including the '\0'. Limits the number of
      bytes copied to length of 'stringBuf'
   */
   for( c = 0; c < _StringBufBytes-1; c++ )
   {
      if( (stringBuf[c] = src[c]) == '\0' )
         break;
   }
   // In case 'src' string is truncated in 'stringBuf', terminate with '\0'.
   stringBuf[_StringBufBytes-1] = '\0';

   return stringBuf;
}


// ------------------------------------------ eof ------------------------------------------
