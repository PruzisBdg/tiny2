/*---------------------------------------------------------------------------
|
|              Test Fixture Support - Vector Class Support
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "vec.h"           // _Vec_Make()


/*----- ------------------------------------------------------------------------------
|
|  Vec_Init()
|
|  Framework wrapper for initialising S_Vec (vec.c)
|
|  PARMS:   'obj'    - the S_Vec to be constructed
|           'init'   - the initialisating S_VecCfg
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 Vec_Init( T_ObjAddr obj, T_CodeAddr init)
{
   _Vec_Make( (S_Vec *)obj,
              ((S_VecCfg CONST *)init)->buf,
              ((S_VecCfg CONST *)init)->rows,
              ((S_VecCfg CONST *)init)->cols
            );
   Vec_Fill((S_Vec *)obj, 0);       // Be tidy! All vectors start zero-filled.
   return 1;
}


// ------------------------------------ eof ------------------------------------------ 
