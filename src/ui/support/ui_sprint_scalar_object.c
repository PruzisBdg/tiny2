/*---------------------------------------------------------------------------
|
|                Tiny Lib - Print an object to 'out'
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "tty_ui.h"
#include "wordlist.h"
#include "calv.h"
#include "tiny1util.h"
#include "arith.h"
#include "romstring.h"
#include "hostcomms.h"


#include <string.h>


/*-----------------------------------------------------------------------------------------
|
|  UI_SPrintScalarObject()
|
|  Print the value of a single object
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_SPrintScalarObject( C8 *out, S_Obj CONST *obj, U8 flags )
{
   S_ObjIO CONST * io;
   U8 idx;
   
   out[0] = '\0';

   if(BSET(flags, _UI_PrintObject_Raw)) 
      { io = 0; }
   else
      { io = GetObjIO(_StrConst(obj->name)); }
      
   if(BSET(flags, _UI_PrintObject_PrependName)) 
      { idx = sprintf(out, "%s = ", _StrConst(obj->name)); }
   else
      { idx = 0; }
      
   if( Obj_IsAScalar(obj) )
   {
      UI_SPrintScalar( out + idx, Obj_ReadScalar(obj), io, flags & _UI_PrintObject_AppendUnits );
   }
   else
   {
      strcat(out, "Can't read this\r\n");
   }
}

// ----------------------------- eof ----------------------------------------- 
