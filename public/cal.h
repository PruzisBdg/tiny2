/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#ifndef CAL_H
#define CAL_H

#include "common.h"
#include "sysobj.h"     // T_CodeAddr

// ------------------------ Scalar (1 integer) ------------------------------

typedef struct
{
   U8       storeHdl;   // handle to stored value
   S16      dflt,       // default value
            min, max;   // limits
} S_CalCfg;

typedef struct
{
   S16             current;    // current value
   S_CalCfg CONST *cfg;        // ROM configuration
   U8              valid;      // 1 = valid
} S_Cal;


PUBLIC U8   Cal_HandleUI( C8 *args );
PUBLIC U8   Cal_Init( T_ObjAddr obj, T_CodeAddr init);
PUBLIC S16  Cal_Read( S_Cal RAM_IS *cal );
PUBLIC U8   Cal_ReadStored( S_Cal RAM_IS *cal, S16 *out );
PUBLIC void Cal_Write( S_Cal RAM_IS *cal, S16 n );
PUBLIC void Cal_Write_NoLink( S_Cal RAM_IS *cal, S16 n );
PUBLIC void Cal_Increment( S_Cal RAM_IS *cal, S16 n );
PUBLIC void Cal_ToDefault( S_Cal RAM_IS *cal );
PUBLIC U8   Cal_Store( S_Cal RAM_IS *cal );
PUBLIC U8   Cal_Recall( S_Cal RAM_IS *cal );


#endif // CAL_H

// ---------------------------------------- eof -------------------------------------
