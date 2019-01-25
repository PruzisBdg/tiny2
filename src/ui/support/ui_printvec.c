/*---------------------------------------------------------------------------
|
|                 TTY User Interface - UI_PrintVec()
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"
#include "vec.h"

#include <string.h>
#include "sysobj.h"           // S_Obj
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"


PRIVATE void newVecLine(void) 
   { WrStrLiteral("\r\n   "); }  // Insert continuation chars, newline and indent


/*-----------------------------------------------------------------------------------------
|
|  UI_PrintVec()
|
|  Format vector 'v' into 'pbuf' for printing. 
|
|  Each row of 'v' is printed on one line. If the width of a line exceeds '_WrapWidth' then
|  the line is wrapped with '...' as continuation markers.
|
|  Each number is printed with format specifier 'format'. If 'scale' != 0 then each vector
|  element is multiplied by 'scale' and printed as a floating point number' Then 'format'
|  should be floating point; else it should be integer.
|
|  If 'pbuf' is not large enough to hold all of 'v' then puts as many numbers as will fit.
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_PrintVec( S_Vec *v, U8 CONST *format, float scale )
{
   U8    r,c;           // vector row, col indices
   S16   n;
   U16   charCnt;       // counts chars printed so far in the current row, to check for wrap.

   #define _WrapWidth 70

   for( r = 0; ; )                                          // for each row
   {
      if( v->rows > 1 )                                     // more than 1 row?
      {
         if( r == 0 )                                       // 1st row yet to print?
            { WrStrLiteral("\r\n"); }                       // then drop to new line
            
         sprintf( PrintBuf.buf, "row %d:  ", r );           // more than 1 row? ... then label each row
         PrintBuffer();
         
         if( v->cols > 10 )                                 // More than 10 columns?
            { newVecLine(); }                           // then drop to new line for a pretty multiple-row print            
      }

      for( c = 0, charCnt = 9; c < v->cols; c++ )           // for each column
      {
         n = _Vec_Read1(v, r, c);                           
         
         if( scale != 0 )                                   // floating point scale provided?
         {
            charCnt += sprintf( PrintBuf.buf, (C8 CONST *)_StrConst(format), scale * n );        // and format as a float
         }
         else
         {
            charCnt += sprintf( PrintBuf.buf, (C8 CONST *)_StrConst(format), n );        // else format as an int
         }
         PrintBuffer();
         
         if( charCnt > _WrapWidth || c % 10 == 9 )          // row too long OR 10 numbers in row, need to wrap?
         {
            newVecLine();                                   // insert continuation chars, newline and indent
            charCnt = 0;
         }
      }
      if( ++r >= v->rows )                                  // all rows printed?
         { break; }
      else                                                  // else more rows to print
         { WrStrLiteral("\r\n"); }
   }
   #undef _WrapWidth
}

// ----------------------------------------- eof ----------------------------------------------- 
