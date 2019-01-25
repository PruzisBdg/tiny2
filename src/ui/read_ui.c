/*---------------------------------------------------------------------------
|
|                 TTY User Interface - Objects Listing
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"
#include "calv.h"

#include <string.h>
#include "sysobj.h"           // S_Obj
#include "class.h"
#include "wordlist.h"      // Str_FindWord()
#include "tiny1util.h"
#include "romstring.h"
#include <ctype.h>

/*-----------------------------------------------------------------------------------------
|
|  printList()
|
|  Print the objects in a List.
|
------------------------------------------------------------------------------------------*/

PRIVATE void printList(S_Obj CONST *obj, U8 flags)
{
   U8       c;
   U8 CONST * name;

   for( c = 0; c < ((S_ObjNames CONST *)(obj->addr))->cnt; c++ )  // For each list element
   {
      name = ((S_ObjNames CONST *)(obj->addr))->names[c];         // Get element name
 
      if( !(obj = GetObj(_StrConst(name))) )                      // list element not a valid object
      {
         WrStrLiteral("List object not found\r\n");               // then don't print it
      }
      else
      {                                                           // else valid so do print it.
         UI_PrintObject(obj, flags);              
      }
      WrStrLiteral("  ");                                         // 2 spaces between list elements
   }
}


/*-----------------------------------------------------------------------------------------
|
|  UI_ReadObj()
|
|  Print value(s) of an object or list of objects. 
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 CONST UI_ReadObj_Help[] = 
"Reads the value of Scalars, Vectors or Calibrations\r\n\
Usage: read <object_name> ['raw' = print raw numbers] ['numsOnly' = no names or units]";

PUBLIC U8 UI_ReadObj( U8 *args )
{
   S_Obj CONST    *obj;
   U8             flags;      // Flags controlling the print format

   flags = 0;
   
   if(UI_RawInArgList(args))                          // 2nd (or last) arg is 'raw'
      { SETB(flags, _UI_PrintObject_Raw); }           // then will print raw numbers, else formatted.
      
   if( !Str_WordInStr( args, _U8Ptr("numsOnly")) )    // Add name and units?
      { SETB(flags, _UI_PrintObject_AppendUnits | _UI_PrintObject_PrependName); }

   while(obj = UI_GetAnyObj_MsgIfFail(args))          // Get object named by current arg...
   {                                                  // ... continue until get something which isn't an object
      if( obj->type == _Class_List )                  // Object is a list?
         { printList(obj, flags); }                   // then will print all list members
      else if( Str_1stWordHasChar(args, '.') != _Str_NoMatch )
      { 
         if(BSET(flags, _UI_PrintObject_PrependName)) 
         {
            Str_CopyNthWord(args, PrintBuf.buf, 0, 30);
            PrintBuffer();
            WrStrLiteral(" = ");
         }
   
         UI_PrintScalar( 
            UI_GetIntFromObject(obj, args), 
            BSET(flags, _UI_PrintObject_Raw) 
               ? 0 : 
               GetObjIO(_StrConst(obj->name)), 
            flags & _UI_PrintObject_AppendUnits ); 
      }
      else                                                  
         { UI_PrintObject(obj, flags); }              // else just a single object, print it.
            
      args = (U8*)Str_GetNthWord(args,1);             // Get next arg      
            
      if( Str_EndOfLineOrString(args[0]) )            // Break if end-of-line or string
         { break; }
         
      if( Str_FindWord(args, "raw") == 0 ||           // Next arg is 'raw'?
          Str_FindWord(args, "numsOnly") == 0 )       // OR 'justNums'?      
         { break; }                                   // then we're done
         
      WrStrLiteral("  ");                             // Add 2 spaces between printed elements
   }
   return 1;
}

// -------------------------------------- eof ------------------------------------------------  
