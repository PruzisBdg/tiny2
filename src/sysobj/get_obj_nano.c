#include "common.h"
#include "sysobj.h"

extern CONST S_Obj   Objs_List[];     // The master object list
PUBLIC U8 Objs_NumObjs(void);         // Size of that list

/*-----------------------------------------------------------------------------------------
|
|  GetObj_Nano()
|
|  Return the entry in the object list for object 'hdl'. Return 0 if 'hdl' isn't in the
|  list.
|
------------------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * GetObj_Nano( U8 hdl )
{
   U8 rIDATA c;

   for( c = 0; c < Objs_NumObjs(); c++ )  // Try each object in list
   {
      if( Objs_List[c].hdl == hdl )       // Is a match to 'hdl'?      
      {
         return &Objs_List[c];            // then return list entry   
      }
   }
   return 0;                              // else no match found
}

// ------------------------------ eof -----------------------------------  
