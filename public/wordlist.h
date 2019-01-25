/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|  These handle strings of space-delimited words. Words are one or more of anything 
|  except Spc,Tab CR,LF or '\0'. Words are sepatated by one or more spaces, CR or 
|  LF (NOT tabs at present, but might add those).
|
|--------------------------------------------------------------------------*/

#ifndef WORDLIST_H
#define WORDLIST_H

#include "common.h"

PUBLIC U8           Str_WordCnt(     U8 GENERIC const *str );
PUBLIC U8           Str_FindWord(    U8 GENERIC const *lst, U8 GENERIC const *str);
PUBLIC U8           Str_FindWord_CC( U8 CONST *lst, U8 CONST *str );
PUBLIC U8 GENERIC * Str_GetNthWord(  U8 GENERIC const *lst, U8 n );
PUBLIC U8           Str_CopyNthWord( U8 GENERIC const *lst, U8 *out, U8 n, U8 maxChars );
PUBLIC BIT          Str_WordInStr(   U8 GENERIC const *str, U8 GENERIC const *theWord );
PUBLIC U8           Str_WordsInStr(  U8 GENERIC const *str, U8 GENERIC const *wordsToMatch );
PUBLIC BIT          Str_1stWordsMatch( U8 GENERIC const *w1, U8 GENERIC const *w2 );
PUBLIC U8           Str_WordCharCnt( U8 GENERIC *w );
PUBLIC U8 GENERIC * Str_GetEndWord( U8 GENERIC const *lst, U8 n );
PUBLIC void         Str_Delete( U8 GENERIC *lst, U8 start, U8 cnt );
PUBLIC void         Str_Insert( U8 GENERIC *dest, U8 GENERIC const *src, U8 start, U8 cnt );
PUBLIC void         Str_Replace( U8 GENERIC *str, U8 GENERIC const *toFind, U8 GENERIC const * replaceWith, U8 numToReplace, U8 numToInsert );

PUBLIC BIT          Str_Delimiter(U8 ch);          // Word are divied by CRLF or ' '
PUBLIC BIT          Str_EndOfLineOrString(U8 ch);
PUBLIC U8           Str_1stWordHasChar( U8 GENERIC const *lst, U8 ch );

#define _Str_NoMatch 0xFF      // No match found in Str_FindWord()

/* This bit is set by GetObj(). It forces Sre_FindWord and Str_Delimiter() to treat
   '.' the same as a Spc, as a word delimiter. This allows GetObj() to retrieve the
   object name from constructions like 'MyVec.min' ('MyVec' is the object, 'min' is
   the qualifier).
*/
extern BIT Str_DiceComposites;    

#endif // WORDLIST_H
