#include "common.h"
#include "sysobj.h"

/*-----------------------------------------------------------------------------------------
|
|  ObjExists_Nano()
|
|  Return the entry in the object list for object 'hdl'. Return 0 if 'hdl' isn't in the
|  list.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT ObjExists_Nano( U8 hdl )
{
   return GetObj_Nano(hdl) != 0;
}

// ------------------------------ eof -----------------------------------   
