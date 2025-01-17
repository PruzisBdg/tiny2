/*---------------------------------------------------------------------------
|
|                       Tiny1 - System Objects
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
#include "links.h"
#include "wordlist.h"
#include "romstring.h"

// The following must be made for your project

extern CONST S_Obj   Objs_List[];         // CONST data for all system objects
extern U8            Objs_Flags[];        // (RAM) flags for all system objects = "PUBLIC U8 Obj_Flags[RECORDS_IN(Objs_List)]"
extern CONST S_ObjIO Objs_IOFormats[];    // List of all IO formats
extern CONST S_Class Class_List[];

PUBLIC U8 Objs_NumObjs(void);             // Sizes of each list
PUBLIC U8 Objs_NumIOFormats(void);


// To print an error message if desired.
extern void PrintMakeObjErr(U8 CONST *objName);

/*-----------------------------------------------------------------------------------------
|
|  MakeObjs()
|
|  Run initialisation on all objects in Objs_List[]. The list is scanned repeatedly until
|  either all objects are made or a pass through the list gives no new makes. This so all
|  objects are made even if child objects are further down the list than thier parents.
|
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 MakeObjs(void)
{
   U8            rIDATA c,
                        leftToMake,
                        passCnt;
   S_Obj CONST * rIDATA p;
   BIT                  madeObj,
                        madeObjThisPass;

   U8(rIDATA *initFunc)(T_ObjAddr obj, T_CodeAddr init);

   // Clear all object flags
   for( c = 0; c < Objs_NumObjs(); c++ )
      { Objs_Flags[c] = 0; }

   leftToMake = Objs_NumObjs();
   passCnt = 0;

   do                                                          // scan the object list repeatedly
   {
      madeObjThisPass = 0;                                     // until make an object on this pass of the list

      for( c = 0,
           p = Objs_List;
           c < Objs_NumObjs();                                 // for each entry in the object list
           c++, p++ )
      {
         madeObj = 0;                                          // until/unless made below

         if( BCLR(Objs_Flags[c], 0x01) )                       // object hasn't already been made?
         {
            if( NULL != (initFunc = GetClass(p->type)->initFunc) )      // AND object has an init function?
            {
               if( !(*initFunc)(p->addr, p->init) )            // then try to init!
               {
                  if( passCnt > 0 )                            // failed AND not the 1st pass?
                  {                                            // then send message
                     PrintMakeObjErr(_StrConst(p->name));
                  }
               }
               else                                            // else init succeeded
               {
                  Link_SendEvent(p->addr, _Link_Event_Created); // then post event to link list in case there are any dependencies
                  madeObj = 1;
               }
            }
            else                                               // else object has no init func.
            {
               madeObj = 1;
            }
         }
         else
         {
         }
         if( madeObj )
         {
            Objs_Flags[c] = 0x01;                              // mark the object as made.
            madeObjThisPass = 1;
            if( !--leftToMake ) { break; };                    // all made? then quit this pass rightaway
         }
      }
      passCnt++;
   }
   while( madeObjThisPass && leftToMake );                     // scan until all made or can't make any more.

   return leftToMake;
}


// ---------------------------- eof ---------------------------------------------
