/*---------------------------------------------------------------------------
|
|              Test Fixture Support - System Objects
|
|  The test fixture support package maintains a global list of systems objects. Many
|  of the objects (Vectors and Scalars) can be read. The package maintains a set of
|  IO formats which control the presentation of objects to the Host (TTY).
|
|  Public:
|
|     GetObj()    - get an object
|     GetObjIO()  - get IO format for an object
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include <string.h>        // strcmp()
#include "links.h"
#include "wordlist.h"

// The following must be made for your project

extern CONST S_Obj   Objs_List[];         // CONST data for all system objects
extern U8            Objs_Flags[];        // (RAM) flags for all system objects = "PUBLIC U8 Obj_Flags[RECORDS_IN(Objs_List)]"
extern CONST S_ObjIO Objs_IOFormats[];    // List of all IO formats

PUBLIC U8 Objs_NumObjs(void);             // Sizes of each list
PUBLIC U8 Objs_NumIOFormats(void);


/*-----------------------------------------------------------------------------------------
|
|  GetObjIO_C()
|
|  Returns IO format for object 'name'; 0 if no IO specification exists.
|
|  This is a version of GetOBjIO() which takes a ROM (CONST) ptr as its argument. Harvard 
|  CPUs need separate versions for 'name' in ROM and RAM.
|
------------------------------------------------------------------------------------------*/

PUBLIC S_ObjIO CONST * GetObjIO_C( U8 CONST *name )
{
   U8              rIDATA c;
   S_ObjIO CONST * rIDATA p;

   for( c = 0, p = Objs_IOFormats; c < Objs_NumIOFormats(); c++, p++ )     // for each object in list
   {
      if( Str_FindWord_CC( name, p->name ) == 0 )                          // matches 'name' supplied
      {
         return p;                                                         // then return list entry   
      }
   }
   return 0;                                                               // else no match found
}

// ------------------------------ eof ------------------------------------------ 
