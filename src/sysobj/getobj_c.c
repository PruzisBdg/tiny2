/*---------------------------------------------------------------------------
|
|                          Tiny1 - System Object Handling
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "wordlist.h"

extern CONST S_Obj   Objs_List[];         // CONST data for all system objects
extern U8 Objs_NumObjs(void);             // Sizes of each list
extern U8            Objs_Flags[];        // (RAM) flags for all system objects = "PUBLIC U8 Obj_Flags[RECORDS_IN(Objs_List)]"



/*-----------------------------------------------------------------------------------------
|
|  FUNCTION: GetObj_C()
|
|  Return the entry in the object list for object 'name'. Return 0 if 'name' isn't in the
|  list.
|
|  This is a version of GetOBj() which takes a ROM (CONST) ptr as its argument. Harvard CPUs
|  need separate versions for 'name' in ROM and RAM.
|
------------------------------------------------------------------------------------------*/

extern BIT Str_DiceComposites;

PUBLIC S_Obj CONST * GetObj_C( U8 CONST *name )
{
   U8            rIDATA c;
   S_Obj CONST * rIDATA p;

   Str_DiceComposites = 1;                                        // To make Str_FindWord consider '.' as a word delimiter.
   
   for( c = 0, p = Objs_List; c < Objs_NumObjs(); c++, p++ )      // for each object in list
   {
      if( Str_FindWord_CC( name, p->name ) == 0                   // matches first word 'name'?
          && BSET( Objs_Flags[c], 0x01) )                         // AND object was built?
      {
         Str_DiceComposites = 0;                                  // Return to normal 'Str_' parsing
         return p;                                                // then return list entry   
      }
   }
   Str_DiceComposites = 0;                                        // Return to normal 'Str_' parsing
   return 0;                                                      // else no match found
}

// ------------------------------ eof ----------------------------------- 
