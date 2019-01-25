#include "common.h"
#include "sysobj.h"

// If object IO is used, then your system must define these.
extern CONST S_ObjIO Objs_IOList[];         // The master IO list
PUBLIC U8            Objs_NumObjIOs(void);  // Size of that list

/*-----------------------------------------------------------------------------------------
|
|  ObjIOExists_Nano()
|
|  Return 1 if the IO spec for object hdl' exists
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT ObjIOExists_Nano( U8 hdl )
{
   U8 rIDATA c;

   for( c = 0; c < Objs_NumObjIOs(); c++ )   // Try each object in list
   {
      if( Objs_IOList[c].hdl == hdl )        // Is a match to 'hdl'?      
      {
         return 1;                           // then the IO spec exists, return 1   
      }
   }
   return 0;                                 // else no match found
}

// ------------------------------ eof -----------------------------------    
