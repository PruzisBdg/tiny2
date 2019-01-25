/*---------------------------------------------------------------------------
|
|         Test Fixture Support - UART Tx / Rx Queues
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "commsq.h"

extern S_CommsHostQ      Comms_OutQ;

/*-----------------------------------------------------------------------------------------
|
|  Comms_TxBytesFree
|
|  Return the number of bytes free in the TX buffer
|
|  PARAMETERS:
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Comms_TxBytesFree(void)
{
   return Comms_OutQ.buf->size - Comms_OutQ.cnt;
}

// ----------------------------------- eof ----------------------------------------------- 
