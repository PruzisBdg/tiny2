/*---------------------------------------------------------------------------
|
|                          Tiny1 - System Object Handling
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"

extern CONST S_Obj   Objs_List[];         // CONST data for all system objects
extern U8 Objs_NumObjs(void);             // Sizes of each list


/*-----------------------------------------------------------------------------------------
|
|  FUNCTION: GetObjBase()
|
|  Given the address of an object's contents, return the object itself
|
------------------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * GetObjBase( T_ObjAddr addr )
{
   U8            rIDATA c;
   S_Obj CONST * rIDATA p;

   for( c = 0, p = Objs_List; c < Objs_NumObjs(); c++, p++ )   // for each object in list
   {
      if( p->addr == addr )                                    // AND object was built?
      {
         return p;                                             // then addr of the object   
      }
   }
   return 0;                                                   // else no match found
}

// ------------------------------ eof ---------------------------------------
