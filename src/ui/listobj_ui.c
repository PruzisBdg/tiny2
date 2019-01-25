/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Objects Listing
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"

#include <string.h>
#include "sysobj.h"           // S_Obj
#include "class.h"
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"


/*-----------------------------------------------------------------------------------------
|
|  UI_ListObjs()
|
|  Message handler for 'objs' command. Lists all objects, on multiple lines if necessary.
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

extern CONST S_Obj  Objs_List[];
PUBLIC U8 Objs_NumObjs(void);

PUBLIC U8 CONST UI_ListObjs_Help[] = "Lists objects";

PUBLIC U8 UI_ListObjs( U8 *args )
{
   U8 c, lineLen;
   S_Obj CONST * p;
   U8 currClass = 0;
   
   WrStrLiteral("Objects are:\r\n    ");

   for( c = 0, 
        lineLen = 0, 
        p = Objs_List; 
        c < Objs_NumObjs(); c++, p++ )             // for each object
   {
      if( p->type != currClass && p->type != 0 )   // object is different class than last one printed?
      {
         currClass = p->type;                      // then note the class
         sprintf(PrintBuf.buf, "\r\n%s(s):\r\n   - ", _StrConst(GetClassName(p->type)) );  // drop one line and print class name, drop and indent
         PrintBuffer();
         lineLen = 4;                              // line starts at indent
      }

      lineLen += strlen((C8 GENERIC *)_StrConst(p->name));       // line length when this object name printed

      if( lineLen > 55 )                           // wrap line now?
      {
         WrStrLiteral("\r\n   > ");                // drop and print indent
         lineLen = 0;
      }
      Comms_WrStrConst(p->name);                   // print object name
      WrStrLiteral(", ");
   }
   return 1;
}

// -------------------------------------- eof ------------------------------------------------ 
