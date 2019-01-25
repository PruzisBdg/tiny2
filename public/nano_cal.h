/*---------------------------------------------------------------------------
|
|              Nano Application - Cal Support
|
|  For the smallest RAM footprint, systems with 256-512 bytes RAM
|
|
|
|
|
|
|   
|      Rev 1.2   Dec 28 2009 09:55:50   spruzina
|   Adds nCal_InsideLimits().
|   
|      Rev 1.1   Aug 20 2009 12:03:00   spruzina
|   All Cal_Read() etc changed to 'nCal_Read()', to avoid conflict with 'tiny1' calls of same name.
|   
|      Rev 1.0   May 19 2009 12:56:26   spruzina
|   Initial revision.
|  
|--------------------------------------------------------------------------*/

#ifndef NANO_CAL_H
#define NANO_CAL_H

#include "common.h"
#include "sysobj.h"     // T_CodeAddr

// ------------------------ Scalar (1 integer) ------------------------------

typedef struct
{
   S16 RAM_IS  *current;   // current value
   S16 CONST   *stored;    // stored value (in Flash)
   S16         dflt,       // default value
               min, max;   // limits
} S_NanoCal;

PUBLIC U8   nCal_Init( T_ObjAddr obj, T_CodeAddr init);
PUBLIC S16  nCal_Read( S_NanoCal CONST *cal );
PUBLIC S16  nCal_ReadStored( S_NanoCal CONST *cal );
PUBLIC void nCal_Write( S_NanoCal CONST *cal, S16 n );
PUBLIC void nCal_Increment( S_NanoCal CONST *cal, S16 n );
PUBLIC void nCal_ToDefault( S_NanoCal CONST *cal );
PUBLIC U8   nCal_Store( S_NanoCal CONST *cal );
PUBLIC U8   nCal_Recall( S_NanoCal CONST *cal );
PUBLIC BIT  nCal_InsideLimits( S_NanoCal CONST *cal, S16 n );

#endif // NANO_CAL_H

// ---------------------------------------- eof -------------------------------------
