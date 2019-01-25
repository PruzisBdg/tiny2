/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"


/*-----------------------------------------------------------------------------------------
|
|  GetClass()
|
|  Returns the class whose ID is 'classID'. If 'classID' isn't valid returns the Null class.
|
------------------------------------------------------------------------------------------*/

extern CONST S_Class Class_List[];
extern U8 NumClasses(void);

PRIVATE U8 initNothing(T_ObjAddr obj, T_CodeAddr init) { return 1; }
PRIVATE void writeNothing(T_ObjAddr obj, S16 n) { return; }
PRIVATE S16 readNothing( T_ObjAddr obj ) { return 0; }

PRIVATE S_Class CONST nullClass = { _Class_Null,  0,  initNothing,   readNothing, writeNothing,   0 };


PUBLIC S_Class CONST * GetClass(U8 classID)
{
   U8 c;
   
   for( c = NumClasses(); c; c-- )
   {
      if( Class_List[c].id == classID )
         { return &Class_List[c]; }
   }
   return &nullClass;     // 1st entry in the class list is the null class
}

// ------------------------------ eof ----------------------------------- 
