/*---------------------------------------------------------------------------
|
|                    Tiny Lib - 16-wide output port
|
|--------------------------------------------------------------------------*/

#ifndef OUTPORT_H
#define OUTPORT_H

#include "common.h"
#include "sysobj.h"

typedef struct
{
   U16  resetState;
   void (*wrPin)(U16);
} S_OutPortCfg;


typedef struct
{
   S_OutPortCfg CONST *cfg;
   U16 state;
   U8  locked;
} S_OutPort;

PUBLIC U8   OutPort_Init( T_ObjAddr obj, T_CodeAddr init );
PUBLIC BIT  OutPort_Reset( S_OutPort *p );
PUBLIC BIT  OutPort_Write( S_OutPort *p, U16 n );
#define OutPort_WriteInt OutPort_Write       // for class write functions, which pass S16
PUBLIC U16  OutPort_Read( S_OutPort *p );
PUBLIC S16 OutPort_ReadInt( S_OutPort *p );
#define  OutPort_ReadInt OutPort_Read        // for class read functions, which expect S16
PUBLIC U8   OutPort_HandleUI( C8 *args );

#endif // OUTPIN_H
 
