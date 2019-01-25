/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins
|
|
|
|
|
|
|   
|      Rev 1.0   May 19 2009 12:56:26   spruzina
|   Initial revision.
|  
|--------------------------------------------------------------------------*/

#ifndef NANO_INPIN_H
#define NANO_INPIN_H

#include "common.h"

typedef struct
{
   BIT (*rdPin)(void);
} S_InPin;

PUBLIC BIT InPin_Read(S_InPin CONST *p);
PUBLIC S16 InPin_ReadInt(S_InPin CONST *p);

#endif // NANO_INPIN_H 
