/*---------------------------------------------------------------------------
|
|                   ANSI Terminal Multi-line editor
|
|  Multiple single-line edit buffers. Allows editing of a command line before entry
|  and recall of previous command lines for modification and reuse. Requires
|  ANSI (VT100) terminal.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "hostcomms.h"
#include "lineedit.h"
#include <string.h>
#include "systime.h"

extern BIT EchoOn;

PRIVATE BIT insertMode = 1;


// ================================ PRIVATE =====================================


#define _LineBufLen 40              // Each line buffer is this long

typedef struct                      // Specifies a line buffer
{
   U8 cnt,                          // number of chars in buffer
      curs,                         // the current cursor position
      buf[_LineBufLen];             // holds chars
} S_LineBuf;

typedef struct
{
   U8 buf[_LineBufLen];             // holds chars
} S_StoreBuf;

#define _MaxLinesStored 5           // Line editor has this many individual line buffers

typedef struct                      // Specifies a line editor
{
   U8         put,                  // next empty slot in 'stored'
              get,                  // most recent command in 'stored'
              backCnt;              // counts how far we have scrolled back through previos commands
   S_LineBuf  edit;                 // to edit the current line
   S_StoreBuf stored[_MaxLinesStored]; // to store previous lines
   T_Timer    lastChar;             // when last char was recieved
}  S_LineEditor;

PRIVATE S_LineEditor lineEditor;    // Make just one

// ------------------------- Debug ----------------------------------------

//#define LINE_EDIT_DBG

#ifdef LINE_EDIT_DBG



PRIVATE void showDebug(void)
{
   sprintf(PrintBuf.buf, "\r\nlineEdit: get %d put %d back %d\r\n", lineEditor.get, lineEditor.put, lineEditor.backCnt );
   PrintBuffer();
}

#define _showDebug() showDebug()

#else

#define _showDebug()

#endif

// ---------------------- end: debug -------------------------------------------


/*-----------------------------------------------------------------------------------------
|
|  clearScreenLine
|
|  Clear the edit line (on the terminal) by overwriting with spaces
|
------------------------------------------------------------------------------------------*/

PRIVATE void clearScreenLine( U8 len )
{
   WrStrLiteral("\r");                       // from start
   for( ; len; len-- ) WrStrLiteral(" ");    // write 'len' spaces
   WrStrLiteral("\r");                       // and back to start
}


/*-----------------------------------------------------------------------------------------
|
|  clearLineBuffer
|
|  clear line buffer indexed by 'n'
|
------------------------------------------------------------------------------------------*/

PUBLIC void clearLineBuffer(S_LineBuf *b)
{
   b->cnt = 0;
   b->curs = 0;
   b->buf[0] = '\0';
}


/*-----------------------------------------------------------------------------------------
|
|  fetchGetBuffer
|
|  Fetch a string from current 'get' buffer into the line edit buffer. If the
|  buffer contains an empty string then don't fetch.
|
------------------------------------------------------------------------------------------*/

PRIVATE void fetchGetBuffer( void )
{
   U8 len;

   #define le lineEditor

   if( (len = strlen((C8*)le.stored[le.get].buf)) )                   // this buffer holds a line?
   {
      strcpy( (C8*)le.edit.buf, (C8 const *)le.stored[le.get].buf );      // then copy it to current buffer
      le.edit.cnt = len;
      le.edit.curs = le.edit.cnt;
   }
   #undef le
}


/*-----------------------------------------------------------------------------------------
|
|  refreshScreenLine
|
|  Refresh the line on the Host terminal
|
------------------------------------------------------------------------------------------*/

PRIVATE void refreshScreenLine(void)
{
   #define e lineEditor.edit

   e.buf[e.cnt] = '\0';                               // Null-terminate buffer, just to be sure

   if( EchoOn )                                       // echoing to Host?
   {
      sprintf(PrintBuf.buf, "\x1B[2K\r> %s", e.buf);  // erase current line then rewrite
      PrintBuffer();
   }
   #undef e
}



/*-----------------------------------------------------------------------------------------
|
|  fetchStoredLine
|
|  Fetch next/prev stored line into current buffer.
|
------------------------------------------------------------------------------------------*/

#define _FetchPrevLine 0
#define _FetchNextLine 1

PRIVATE void fetchStoredLine( U8 direction )
{
   #define le lineEditor

   if( direction == _FetchPrevLine )
   {
      if( le.backCnt < _MaxLinesStored )                       // NOT already backed up to oldest line?
      {                                                        // then back up one
         if( le.get == 0 ) le.get = _MaxLinesStored-1; else le.get--;
         fetchGetBuffer();
         le.backCnt++;
      }
   }
   else                                                        // else get next
   {
      if( le.backCnt > 0 )                                     // weren't already at newest line? (one behind 'put')
      {
         le.get++; if( le.get >= _MaxLinesStored ) le.get = 0; // Get index to one-ahead of 'get'
         fetchGetBuffer();
         le.backCnt--;
      }
   }
   #undef le
}


// ============================= end: PRIVATE =====================================




/*-----------------------------------------------------------------------------------------
|
|  LineEdit_ClearCurrentBuffer
|
|  Clear the current line buffer.
|
------------------------------------------------------------------------------------------*/

PUBLIC void LineEdit_ClearCurrentBuffer(void)
{
   clearLineBuffer(&lineEditor.edit);
}

/*-----------------------------------------------------------------------------------------
|
|  LineEdit_CurrentBuf
|
|  Return the current line buffer (just the data portion).
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * LineEdit_CurrentBuf(void)
{
   return lineEditor.edit.buf;
}


/*-----------------------------------------------------------------------------------------
|
|  LineEdit_CharsInCurrentBuf
|
|  Return the number of chars in the current buffer
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 LineEdit_CharsInCurrentBuf(void)
{
   return lineEditor.edit.cnt;
}


/*-----------------------------------------------------------------------------------------
|
|  LineEdit_HostIsQuiet
|
|  Return 1 if more than (5)secs elapsed since last char from Host
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT LineEdit_HostIsQuiet(void)
{
   return ElapsedS(lineEditor.lastChar) > _TicksSec_S(5);
}


/*-----------------------------------------------------------------------------------------
|
|  LineEdit_Init
|
|  Clear all line buffers.
|
------------------------------------------------------------------------------------------*/

PUBLIC void LineEdit_Init(void)
{
   U8 rIDATA c;

   clearLineBuffer(&lineEditor.edit);

   for( c = 0; c < RECORDS_IN(lineEditor.stored); c++ )
   {
      lineEditor.stored[c].buf[0] = 0;
   }
   lineEditor.put = 0;
   lineEditor.get = 0;
   lineEditor.backCnt = 0;
   lineEditor.lastChar = _Now();
}


/*-----------------------------------------------------------------------------------------
|
|  LineEdit_AcceptLine
|
|  Store the current line editor contents in the next line buffer slot; this provided the
|  new line isn't a repeat of the previous one.
|  Set command replay to start at the newly entered command.
|
------------------------------------------------------------------------------------------*/

PUBLIC void LineEdit_AcceptLine(void)
{
   U8 c;

   #define le lineEditor

   c = le.put;
   if( c == 0 ) c = _MaxLinesStored-1; else c--;                  // index previous stored command

   if( strcmp( (C8*)le.stored[c].buf, (C8*)le.edit.buf ) != 0 )   // didn't just repeat the last command?
   {
      strcpy( (C8*)le.stored[le.put].buf, (C8 const *)le.edit.buf );   // Store line at current put
      le.put++; if( le.put >= _MaxLinesStored ) le.put = 0;       // Advance put to next slot
   }
   le.get = le.put;                                               // The most recent command is the one just entered
   le.backCnt = 0;                                                // and reset the scrollback counter

   _showDebug();
   #undef le
}


   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST LineEdit_Help[] =
"\r\nANSI terminal line editor with 6 line recall\r\n\
   cursor:\r\n\
      <arrow-left/right> = left/right one\r\n\
      <Ctrl-Z> = to line start   <Ctrl-X> to line end\r\n\
   edit:\r\n\
      <Bkspc> = delete back  <Del> = delete at cursor\r\n\
      <Ctrl-A> = clear line   <Ctrl-E> clear to end of line\r\n\
      <Ctrl-I> = toggle insert / overwrite\r\n\
   line buffer:\r\n\
      <arrow-up> = last line entered  <arrow-down> = next line\r\n";
   #else
PUBLIC U8 CONST LineEdit_Help[] = "";
   #endif // INCLUDE_HELP_TEXT


/*-----------------------------------------------------------------------------------------
|
|  LineEdit_AddCh
|
|  Process new 'ch', which may be a printable char for the buffer, or an ANSI control.
|
|  Returns 1 if <Enter> pressed, indicating a complete line; else 0.
|
------------------------------------------------------------------------------------------*/

#define _BkSpc 8
#define _Del   127
#define _Enter 13


// ANSI terminal Ctrl codes
#define _Ctrl_A 0x01
#define _Ctrl_B 0x02

#define _Ctrl_D 0x04
#define _Ctrl_E 0x05
#define _Ctrl_F 0x06
#define _Ctrl_G 0x07
#define _Ctrl_H 0x08
#define _Ctrl_I 0x09
#define _Ctrl_J 0x0A
#define _Ctrl_K 0x0B
#define _Ctrl_L 0x0C
#define _Ctrl_M 0x0D
#define _Ctrl_N 0x0E
#define _Ctrl_O 0x0F
#define _Ctrl_P 0x10
#define _Ctrl_Q 0x11
#define _Ctrl_R 0x12
#define _Ctrl_S 0x13
#define _Ctrl_T 0x14
#define _Ctrl_U 0x15
#define _Ctrl_V 0x16
#define _Ctrl_W 0x17
#define _Ctrl_X 0x18
#define _Ctrl_Y 0x19
#define _Ctrl_Z 0x1A

// Up arrow = <Esc 91 65>  down arrow = <Esc 91 66>
#define _Esc   27
#define _Esc_2ndCh 91
#define _Esc_Up_Arrow 65
#define _Esc_Down_Arrow 66
#define _Esc_Right 67
#define _Esc_Left 68


PUBLIC U8 LineEdit_AddCh( U8 ch )
{
   U8 c, rtn;
   static BIT esc = 0, esc2;
   S_LineBuf * rIDATA e;
   BIT useCh, didInsert, cursAtEOL, roomToAddCh;

   #define le lineEditor
   #define _PromptLen 2

   lineEditor.lastChar = _Now();                       // Mark time we got this char, for 'quiet' handling

   rtn = 0;                   // unless line completed with <Enter>
   useCh = 0;
   e = &le.edit;

   if( esc )                                          // parsing Esc sequence
   {
      if( !esc2 )                                     // waiting for '['?
      {
         if( ch == _Esc_2ndCh )                       // got '['? then mark
            { esc2 = 1; }
         else                                         // any other char
         {
            esc = 0; esc2 = 0;                        // then exit escape
            e->cnt = 0; e->curs = 0;                  // and clear the line
            useCh = 1;                                // will use this char as the start of a new line
            refreshScreenLine();
         }
      }
      else                                            // else got 2nd escape sequence char
      {
         switch(ch)
         {
            case _Esc_Down_Arrow:                           // down (next line)
               if( EchoOn )
                  { clearScreenLine( e->cnt+_PromptLen ); } // clear out current text on line editor
               fetchStoredLine(_FetchNextLine);             // next line buffer
               _showDebug();
               refreshScreenLine();                         // and display the retrieved line
               break;

            case _Esc_Up_Arrow:                             // up (prev line)
               if( EchoOn )
                  { clearScreenLine( e->cnt+_PromptLen ); } // clear out current text on line editor
               fetchStoredLine(_FetchPrevLine);             // prev line buffer
               _showDebug();
               refreshScreenLine();                         // and display the retrieved line
               break;

            case _Esc_Right:                                // right arrow?
               if(e->curs < e->cnt)                         // not at end of line?
               {
                  WrStrLiteral("\x1B[C");                   // then screen cursor right
                  e->curs++;
               }
               break;

            case _Esc_Left:                                 // left arrow?
               if(e->curs)                                  // not at start of line?
               {
                  WrStrLiteral("\x1B[D");                   // then screen cursor left
                  e->curs--;
               }
               break;

            default:
               break;
         }
         esc = 0;                                           // Done with <Esc> sequence; clear flags
         esc2 = 0;
      }
   }
   else                                                     // else not parsing Esc sequence;
   {
      useCh = 1;                                            // so decode the char directly
   }
   if( useCh)                                               // got single char from a keystroke?
   {
      switch(ch)
      {
         case _Esc:                                         // <Esc>?
            esc = 1;                                        // next 2 chars should be from Esc sequence,
            esc2 = 0;                                       // so set flags to parse these
            break;

         case _Enter:                                       // <Enter> so got new line
            e->buf[e->cnt] = '\0';                          // null-terminate the buffer
            rtn = 1;                                        // and indicate complete line
            break;

         case _Del:                                         // Delete char under cursor?
            if( e->cnt && (e->curs < e->cnt))
            {
               for( c = e->curs; c < e->cnt; c++ )          // from cursor to end of current line
               {
                  e->buf[c] = e->buf[c+1];                  // overwrite each char with the one beyond
               }
               e->cnt--;                                    // line is one char shorter

               WrStrLiteral("\x1B[s");                      // save cursor
               refreshScreenLine();                         // then remake the whole line
               WrStrLiteral("\x1B[u");                      // restore cursor
            }
            break;

         case _BkSpc:                                       // <Delete before cursor?
            if(e->curs > 0)                                 // cursor beyond start of line?
            {
               for( c = e->curs-1; c < e->cnt; c++ )        // from one before cursor to end of current line
               {
                  e->buf[c] = e->buf[c+1];                  // overwrite each char with the one beyond
               }
               e->cnt--; e->curs--;                         // cursor is now back one and line one char shorter

               WrStrLiteral("\x1B[s");                      // save cursor
               refreshScreenLine();                         // then remake the whole line
               WrStrLiteral("\x1B[u\x1B[D");                // restore cursor, backspace one

            }
            break;


         case _Ctrl_E:                                      // Delete to end of line?
            e->cnt = e->curs;                               // Make lineas long as the cursor position
            WrStrLiteral("\x1B[K");                         // and delete to EOL on the screen
            break;

         case _Ctrl_I:                                      // toggle insert/overwrite mode
            if( insertMode ) { insertMode = 0; } else { insertMode = 1; }
            break;

         case _Ctrl_L:                                      // Delete whole line?
            e->curs = 0;                                    // which is now zero length
            e->cnt = 0;
            refreshScreenLine();
            break;

         case _Ctrl_Z:                                      // Cursor to LHS?
            sprintf(PrintBuf.buf, "\x1B[%dD", e->curs);     // send screen cursor back 'e->curs' spaces
            PrintBuffer();
            e->curs = 0;                                    // is now at LHS
            break;

         case _Ctrl_X:                                      // Cursor to end of line?
            sprintf(PrintBuf.buf, "\x1B[%dC", (e->cnt - e->curs) );
            PrintBuffer();
            e->curs = e->cnt;                               // Screen cursor fwd by 'cnt-curs'
            break;

         default:                                              // ASCII char (presumably)
            if( (ch > 0x20 && ch < 0x07E) || ch == ' ')        // char is printable (space included)?
            {
               cursAtEOL = e->curs >= e->cnt;                  // cursor at end of line?
               roomToAddCh = e->cnt < _LineBufLen-2;           // can add another char?

               if( insertMode && roomToAddCh && !cursAtEOL )   // insert char into line?
               {
                  for( c =  _LineBufLen - 2; c > e->curs; c-- )// then from insertion point to end
                  {
                     e->buf[c] = e->buf[c-1];                  // move chars up one
                  }
                  didInsert = 1;
                  WrStrLiteral("\x1B[s");                      // save current cursor position
               }
               else                                            // else either char or overwrite another char
               {
                  didInsert = 0;
               }

               e->buf[e->curs] = ch;                           // Add new char at insertion point

               if(didInsert || cursAtEOL)                      // char was appended OR inserted?
               {
                  e->cnt++;                                    // then bump char count
               }

               if( didInsert )                                 // inserted a char?
               {
                  refreshScreenLine();                         // then write out the revised line
                  WrStrLiteral("\x1B[u\x1B[C");                // and restore cursor (saved above) one right of the insertion point
               }
               else                                            // else appended a char
               {
                  if( EchoOn )                                 // echo?
                  {
                     Comms_PutChar(ch);                        // then write it back to the terminal
                  }                                            // which will advance the screen cursor
               }
               e->curs++;                                      // and advance the cursor count
            }
      }
   }

   #undef l
   return rtn;
}

/* ===================================== eof ====================================== */



