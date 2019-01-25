/*---------------------------------------------------------------------------
|
|                          Tiny1 - System Object Handling
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "wordlist.h"
#include "romstring.h"

extern CONST S_Obj   Objs_List[];         // CONST data for all system objects
extern U8 Objs_NumObjs(void);             // Sizes of each list
extern U8            Objs_Flags[];        // (RAM) flags for all system objects = "PUBLIC U8 Obj_Flags[RECORDS_IN(Objs_List)]"



/*-----------------------------------------------------------------------------------------
|
|  FUNCTION: GetObjByIdx()
|
|  Return the entry in the object list at 'idx'. Return 0 if 'idx' is outside the object
|  table.
|
------------------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * GetObjByIdx( U8 idx )
{
   if( idx >= Objs_NumObjs() ||              // Index is beyond object table? OR
       BCLR( Objs_Flags[idx], 0x01))         // this object not yet built?
      { return 0; }                          // then return fail
   else
      { return &Objs_List[idx]; }            // else return the object
}

// ------------------------------ eof ----------------------------------- 
