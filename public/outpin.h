/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
|--------------------------------------------------------------------------*/

#ifndef OUTPIN_H
#define OUTPIN_H

#include "common.h"
#include "sysobj.h"

typedef struct
{
   U8  resetState;
   void (*wrPin)(U8);
} S_OutPinCfg;


typedef struct
{
   S_OutPinCfg CONST *cfg;
   U8  state;
   U8  locked;
} S_OutPin;

PUBLIC U8   OutPin_Init( T_ObjAddr obj, T_CodeAddr init );
PUBLIC BIT  OutPin_Reset( S_OutPin RAM_IS *p );
PUBLIC BIT  OutPin_Write( S_OutPin RAM_IS *p, U8 n );
PUBLIC void OutPin_WriteInt( S_OutPin RAM_IS *p, S16 n );
PUBLIC BIT  OutPin_Read( S_OutPin RAM_IS *p );
PUBLIC S16  OutPin_ReadInt( S_OutPin RAM_IS *p );       // does the same as _Read() but is the read function for the class
PUBLIC U8   OutPin_HandleUI( U8 *args );

#endif // OUTPIN_H
 
