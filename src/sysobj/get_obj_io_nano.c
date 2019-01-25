#include "common.h"
#include "sysobj.h"

// If object IO is used, then your system must define these.
extern CONST S_ObjIO Objs_IOList[];         // The master IO list
PUBLIC U8            Objs_NumObjIOs(void);  // Size of that list

/*-----------------------------------------------------------------------------------------
|
|  GetObjIO_Nano()
|
|  Return the IO format spec for for object 'hdl'. Return 0 if 'hdl' isn't in the
|  list.
|
------------------------------------------------------------------------------------------*/

PUBLIC S_ObjIO CONST * GetObjIO_Nano( U8 hdl )
{
   U8 rIDATA c;

   for( c = 0; c < Objs_NumObjIOs(); c++ )   // Try each object in list
   {
      if( Objs_IOList[c].hdl == hdl )        // Is a match to 'hdl'?      
      {
         return &Objs_IOList[c];             // then return list entry   
      }
   }
   return 0;                                 // else no match found
}

// ------------------------------ eof -----------------------------------   
