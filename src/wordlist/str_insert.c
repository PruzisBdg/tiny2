/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include <string.h>

/*-----------------------------------------------------------------------------
|
|  Make a 'gap' at 'p' by moving the remainder of the string forward
|
------------------------------------------------------------------------------*/

PRIVATE void makeGap( U8 *p, U8 gap)
{
   U8 bytesToMove;
   
   /* Rem strlen(p) returns to number of chars to the end of 'dest' EXCLUDING the '\0', 
      which we must also move; so '+1'   
   */
   bytesToMove = strlen((C8*)p)+1;

   do
   { 
      bytesToMove--;
      p[bytesToMove+gap] = p[bytesToMove]; 
   }
   while(bytesToMove);
}

/*-----------------------------------------------------------------------------
|
|  Str_Insert()
|
|  Insert 'cnt' words from 'src' into 'dest', starting in front of word 'start'
|
|  If 'start' exceeds the number of words in 'dest' then words from 'src' are
|  appended to 'dest'.
|
|  If 'cnt' exceeds the number of words in 'src' then all words in 'src' are
|  inserted into 'dest'.
|
------------------------------------------------------------------------------*/

PUBLIC void Str_Insert( U8 GENERIC *dest, U8 GENERIC const *src, U8 start, U8 cnt )
{
   U8 *p;
   U8 bytesToInsert;
   
   if( cnt > 0 )                                         // insert at least one word
   {   
      p = (U8*)Str_GetNthWord(dest, start);              // Get the insertion point
      
      /* Will insert from start of 'src' to end of nth word. (if start and finish are
         the same letter then there's one byte to insert).   
      */
      bytesToInsert = (U8)(Str_GetEndWord((U8 GENERIC*)src, cnt-1) - src + 1); // 
      
      /* Make a gap in 'dest' to fit the word to be inserted plus one space.
      */
      makeGap(p, bytesToInsert+1); 
                          
      strncpy((C8*)p, (C8 GENERIC *)src, bytesToInsert); // Copy it in the word to be inserted
      p[bytesToInsert] = ' ';                            // and add the space after it
   }
}

// -------------------------------- eof ----------------------------------------- 
