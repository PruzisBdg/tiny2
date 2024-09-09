/*---------------------------------------------------------------------------
|
|            Test Fixtures Support - System Objects
|
|  The test fixture support package maintains a global list of systems objects. Many
|  of the objects (Vectors and Scalars) can be read. The package maintains a set of
|  IO formats which control the presentation of objects to the Host (TTY).
|
|--------------------------------------------------------------------------*/

#ifndef SYSOBJ_H
#define SYSOBJ_H

#include "libs_support.h"
#include "common.h"

/* Define untyped addresses for code, objects and for the larger of either
   DATA_SPACE_IS and CODE_SPACE_IS are defined in 'bldcfg.h".
 */

#ifndef CODE_SPACE_IS
   #error "CODE_SPACE_IS must be defined"
#else
   #if CODE_SPACE_IS == CODE_SPACE_16BIT
      typedef U16 T_CodeAddr;
      typedef U16 T_AnyAddr;
   #elif CODE_SPACE_IS == CODE_SPACE_20BIT
      typedef U32 T_CodeAddr;
      typedef U32 T_AnyAddr;
   #else
      #error "CODE_SPACE_IS must be one of these choices"
   #endif
#endif


#ifndef DATA_SPACE_IS
   #error "DATA_SPACE must be defined"
#else
   #if DATA_SPACE_IS == DATA_SPACE_8BIT
      typedef U8 T_ObjAddr; 
#elif DATA_SPACE_IS == DATA_SPACE_16BIT
   typedef U16 T_ObjAddr;
#elif DATA_SPACE_IS == DATA_SPACE_32BIT
   typedef U32 T_ObjAddr;
   #else
      #error "DATA_SPACE must be one of these choices"
   #endif
#endif

// Specification of an object.

   #if _TARGET_IS == _TARGET_8051_LIB_LGE_NANO

// Small objects, for memory-constrained systems
typedef struct
{
   T_ObjAddr   addr;       // address of the object
   U8          hdl,        // it's unique handle
               type;       // e.g '_Class_Scalar'
   T_CodeAddr  init;       // initialisation data
} S_Obj;

PUBLIC BIT MakeObjsNano(void);

PUBLIC BIT ObjExists_Nano( U8 hdl );
PUBLIC S_Obj CONST * GetObj_Nano( U8 hdl );

// For IO formatting of read/write objects

typedef struct 
{
   U8         hdl;         // object handle
   U8 CONST * name;        // object name
   U8 CONST * outFormat;   // Format applied to each vector element by sprintf (usually a 'float' format)
   U8 CONST * units;       // Units displayed after the values
   float      outScale;    // To scale the vector for display
   U8 CONST * desc;        // A description, usually for help. To make a tidy table,
                           // keep this the last field.
} S_ObjIO;

PUBLIC S_ObjIO CONST * GetObjIO_Nano( U8 hdl );
PUBLIC BIT ObjIOExists_Nano( U8 hdl );

   #else

// Regular objects, for testers etc with text interfaces
typedef struct
{
   C8 CONST    *name;      // objects are identified by name
   C8 CONST    *desc;      // description; used by 'help'
   T_ObjAddr   addr;       // address of the object
   U8          type;       // e.g '_Class_Scalar'
   T_CodeAddr  init;       // initialisation data
} S_Obj;

PUBLIC U8 MakeObjs(void);

PUBLIC S_Obj CONST * GetObj( C8 GENERIC const *name );
PUBLIC S_Obj CONST * GetObj_C( C8 CONST *name );
PUBLIC S_Obj CONST * GetObjByClass( C8 GENERIC const *name, U8 theClass );
PUBLIC C8 CONST * GetObjName( T_ObjAddr addr );
PUBLIC S_Obj CONST * GetObjBase( T_ObjAddr addr );
PUBLIC S_Obj CONST * GetObjByIdx( U8 idx );

// For IO formatting of read/write objects

typedef struct 
{
   C8 CONST * name;        // object name
   C8 CONST * outFormat;   // format applied to each vector element by sprintf (usually a 'float' format)
   C8 CONST * units;       // units displayed after the values
   float      outScale;    // to scale the vector for display
} S_ObjIO;

PUBLIC S_ObjIO CONST * GetObjIO( C8 GENERIC const *name );

   #endif // 



// List of object names

typedef struct
{
   U8 CONST **names;
   U8       cnt;
} S_ObjNames;



// Wrapper for making S_Vec
PUBLIC U8 Vec_Init( T_ObjAddr obj, T_CodeAddr init);


#endif // SYSOBJ_H

