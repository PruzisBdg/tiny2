/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|                 Filter for writing a script to a text file.
|
|  Tiny 1 scripts can contain comments. These are from a semicolon to end-of-line.
|  Comments take a lot of space and may overfill text stores, which range from
|  2K-8K. So comments are stripped out if text is written to a stored using
|  'script nn write'. Duplicate spaces are also removed; but tabs are left
|  alone.
|
|--------------------------------------------------------------------------*/


#include "common.h"
#include "txtfile.h"

PUBLIC BIT File_WritingScript;   // 1 if a script is written to the text store.

PRIVATE BIT insideComment;    // Currently parsing through a comment
PRIVATE BIT stripSpc;         // The previous char was whitespace
PRIVATE BIT stripLine;        // Current line is being stripped
PRIVATE BIT gotCR;            // Last char was a CR.

/*-----------------------------------------------------------------------------------------
|
|  stripComments
|
|  Comments in a script are from any semicolon ';' to end-of-line. This filter strips
|  comments and converts successive spaces to single spaces. Tabs are preserved but
|  spaces after any tab will be deleted.
|
|  Return 0 if 'ch' is to be stripped, else return 'ch' itself.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 stripComments(U8 ch)
{
   if( insideComment )                    // Currently parsing through a comment
   {
      if( ch == '\r' )                    // EOL?
      {
         insideComment = 0;               // then we are done with a comment
         return ch;                       // so return 'ch'
      }
      else                                // else still inside a comment
      {
         return 0;                        // so 'ch' will be stripped
      }
   }
   else                                   // else not parsing a comment
   {
      if( ch == ';' )                     // Start of comment?
      {
         insideComment = 1;               // so set flag which says we are now inside a comment
         return 0;                        // and strip the ';' itself
      }
      else                                // else anything other than a ';'
      {
         if( ch == ' ' )                  // Space?
         {
            if( !stripSpc )               // Prev char was not a Spc?
            {
               stripSpc = 1;              // Mark that this one is
               return ' ';                // but return it; if the next char is Spc it will be stripped
            }
            else                          // else prev char was a Spc
            {
               return 0;                  // so strip this one
            }
         }
         else                             // else anything but a space
         {
            stripSpc = 0;                 // so clear 'Spc' flag
            return ch;                    // and keep the char
         }
      }
   }
}

/*-----------------------------------------------------------------------------------------
|
|  stripBlankLines
|
|  Drops all chars from a CRLF to the next char which isn't a CR,LF or Spc. This skips
|  through blank lines and continues on through any leading whitespace to the next
|  visible char.
|
|  Return 0 if 'ch' is to be stripped, else return 'ch' itself.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 stripBlankLines( U8 ch )
{
   if( stripLine )                     // Stripping a line now?
   {
      if( ch != '\r' &&                // Anything other then CR,LF or Spc?
          ch != '\n' &&
          ch != ' ')
      {
         stripLine = 0;                // then stop stripping
         return ch;                    // and will use this char
      }
      else                             // else got CR,LF or Spc
         { return 0; }                 // so continue to strip
   }
   else                                // else not stripping a line
   {
      if( gotCR )                      // Last char was CR?
      {
         gotCR = 0;                    // clear flag
         stripLine = 1;                // strip future chars (until something printable arrrives)

         if( ch == '\n' )              // Also got LF (after the CR)?
            { return ch; }             // then keep it
         else                          // else no LF; line break was just CR
            { return 0;  }             // so strip this char
      }
      else                             // else last ch was not CR
      {
         if( ch == '\r' )              // Got CR now?
            { gotCR = 1; }             // then set flag
         return ch;                    // Is either CR or printable, use it.
      }
   }
}


/*-----------------------------------------------------------------------------------------
|
|  File_StripScript
|
|  Strip comments and blanks lines from a script file.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 File_StripScript(U8 ch)
{
   if( (ch = stripComments(ch)) != 0 )
      return stripBlankLines(ch);
   else
      return ch;
}


/*-----------------------------------------------------------------------------------------
|
|  File_SetupScriptWrite()
|
|  Setup a file for writing a script. Comments and whitespace will be stripped
|  and the remaining text written to the file.
|
------------------------------------------------------------------------------------------*/

PUBLIC void File_SetupScriptWrite(void)
{
   File_WritingScript = 1;       // For comment/whitespace filtering, mark that we are writing a script
   insideComment = 0;            // and that we are outside comments until a ';' encountered.
   stripLine = 0;                // Do not strip lines until 1st CRLF encountered
   gotCR = 0;                    // Until we get a CR
   File_SetupWrite();            // Do all initialisation for a regualr file write.
}



// ------------------------------ eof ------------------------------------------
