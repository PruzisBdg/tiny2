/*---------------------------------------------------------------------------
|
|              Tiny Lib - On/Off Controls (OutPins) - Nano versions
|
|  For small-RAM systems
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

#ifndef NANO_OUTPIN_H
#define NANO_OUTPIN_H

#include "common.h"
#include "sysobj.h"

typedef struct
{
   U8   RAM_IS  *flags;    // Holds the current state of the pin, and the 'locked' flag
   void (*wrPin)(U8);      // method to write the pin
   U8    resetState;       // Whether starts 0 or 1
} S_OutPin;

PUBLIC U8   OutPin_Init( T_ObjAddr obj, T_CodeAddr init );
PUBLIC void OutPin_UpdatePin(S_OutPin CONST *p, U8 n);
PUBLIC BIT  OutPin_Reset( S_OutPin CONST *p );
PUBLIC BIT  OutPin_Write( S_OutPin CONST *p, U8 n );
PUBLIC void OutPin_WriteInt( S_OutPin CONST *p, S16 n );
PUBLIC BIT  OutPin_Read( S_OutPin CONST *p );
PUBLIC BIT  OutPin_Toggle( S_OutPin CONST *p );
PUBLIC S16  OutPin_ReadInt( S_OutPin CONST *p );       // does the same as _Read() but is the read function for the class

#endif // NANO_OUTPIN_H
 
