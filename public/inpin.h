/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins
|
|--------------------------------------------------------------------------*/

#ifndef INPIN_H
#define INPIN_H

#include "common.h"

typedef struct
{
   BIT (*rdPin)(void);
} S_InPinCfg;

typedef struct
{
   S_InPinCfg CONST *cfg;   
} S_InPin;

PUBLIC BIT InPin_Read(S_InPin RAM_IS *p);
PUBLIC S16 InPin_ReadInt(S_InPin RAM_IS *p);

#endif // INPIN_H 
