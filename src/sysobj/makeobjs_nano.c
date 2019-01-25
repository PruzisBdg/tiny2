/*---------------------------------------------------------------------------
|
|              Tiny1 - System Object Creation - 'Nano' Version
|
|  Initialisation of all objects in the master object list
|
|  Public:
|
|     GetObjsNano()    - get an object
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

// The following must be made for your project

extern CONST S_Obj       Objs_List[];        // The master object list
extern CONST S_Class     Class_List[];       // The class information

PUBLIC U8 Objs_NumObjs(void);             // Sizes of each list
PUBLIC U8 Objs_NumIOFormats(void);

/*-----------------------------------------------------------------------------------------
|
|  getInitFunc()
|
|  Returns the 'initFunc'() for the object at 'idx' in the master object list. 
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 (*getInitFunc(U8 idx))(T_ObjAddr, T_CodeAddr)
{
   return GetClass(Objs_List[idx].type)->initFunc;
}

/*-----------------------------------------------------------------------------------------
|
|  MakeObjs()
|
|  Run initialisation on all objects in Objs_List[]. Quit rightway if any initialisation
|  fails.
|
|  Return 1 if all inits, succeeded, 0 if any failed.
|  
------------------------------------------------------------------------------------------*/

PUBLIC BIT MakeObjsNano(void)
{
   U8 c;

   for( c = 0; c < Objs_NumObjs(); c++ )                                   // For each object in the list
   {
      if( getInitFunc(c) )                                                 // Object has an init function?
      {
         if( !(*getInitFunc(c))(Objs_List[c].addr, Objs_List[c].init) )    // then execute the init, quit if fail
         {
            return 0;                                                      // quit if fail
         }
         else                                                              // else init succeeded 
         {
         }
      }
   }
   return 1;
}


// ---------------------------- eof ---------------------------------------------
