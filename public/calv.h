/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibrations
|
|--------------------------------------------------------------------------*/

#ifndef CALV_H
#define CALV_H

#include "common.h"
#include "sysobj.h"     // T_CodeAddr
#include "vec.h"        // uses S_Vec
#include "cal.h"

typedef struct
{
   S_CalCfg  cfg;
   U8 CONST  *vecName;
} S_CalVCfg;

typedef struct
{
   S_CalVCfg CONST *cfg;       // ROM configuration
   S_Vec           *vec;
   U8              valid;      // 1 = valid
} S_CalV;

PUBLIC U8   CalV_HandleUI( U8 *args );
PUBLIC U8   CalV_Init( T_ObjAddr obj, T_CodeAddr init);
PUBLIC S16  CalV_Read1( S_CalV *cal, T_VecRows row, T_VecCols col );
PUBLIC U8   CalV_ReadStored( S_CalV *cal, S16 *out );
PUBLIC void CalV_Write1( S_CalV *cal, S16 n, T_VecRows row, T_VecCols col );
PUBLIC void CalV_ToDefault( S_CalV *cal );
PUBLIC U8   CalV_Store( S_CalV *cal );
PUBLIC U8   CalV_Recall( S_CalV *cal );
PUBLIC U8   CalV_GotStored( S_CalV *cal );


#endif
