/*---------------------------------------------------------------------------
|
|                Comms Queues
|
|--------------------------------------------------------------------------*/

#ifndef COMMSQ_H
#define COMMSQ_H

#include "libs_support.h"
#include "common.h"

#ifndef _COMMSQ_LEN_IS
    #error "_COMMSQ_SIZE_IS must be defined"
#else
    #if _COMMSQ_LEN_IS == COMMSQ_SMALL
        typedef U8 CommsQ_T_Size;
    #elif _COMMSQ_LEN_IS == COMMSQ_LARGE
        typedef U16 CommsQ_T_Size;
    #else
        #error "_COMMSQ_SIZE_IS must be defined"
    #endif
#endif

// A buffer of bytes
typedef struct
{
   CommsQ_T_Size size;      // size of 'theData[]'
   U8            *theData;  // holds the queue data
} S_ByteBuf;

// A queue up to 255 bytes long
typedef struct
{
   CommsQ_T_Size
      put,                 // next write to queue
      get,                 // next read
      cnt;                 // number of bytes currently in queue
   U8 overrun;             // If 1, then there was at least one overrun
   S_ByteBuf CONST *buf;   // buffer holds queue contents
} S_CommsHostQ;

extern S_CommsHostQ     Comms_OutQ;
extern S_CommsHostQ     Comms_InQ;
extern BIT volatile     DoingTx;    // UART send buffer is in use.
extern BIT volatile     Uart0_ISR_GotAByte;  // Semaphore to for power management etc


PUBLIC void StartCommsQ(S_ByteBuf CONST *txBuf, S_ByteBuf CONST *rxBuf);
PUBLIC void FlushCommsQ(S_CommsHostQ *q);
PUBLIC U8 Comms_TxBytesFree(void);
PUBLIC void Comms_UnwedgeTX( void );

#endif // COMMSQ_H 
