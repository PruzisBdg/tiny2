/*---------------------------------------------------------------------------
|
|         Test Fixture Support - UART Tx / Rx Queues
|
|  The is the non-firmware-specifc code for the Host Comms queues. Firmware-
|  dependent code is on it's own source files, to be included directly in
|  projects, rather than linked.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "commsq.h"

/* ------------------------ UART0 Buffers ---------------------------------

   Used by UART0 ISR so must be public
*/   

PUBLIC S_CommsHostQ     Comms_InQ;
PUBLIC S_CommsHostQ     Comms_OutQ;
PUBLIC volatile BIT     DoingTx;

extern void UART0_Enable(void);     // Your application must have this


/*-----------------------------------------------------------------------------------------
|
|  FlushCommsQ
|
|  Flush a S_CommsHostQ. Clear the overrun flag.
|
------------------------------------------------------------------------------------------*/

PUBLIC void FlushCommsQ(S_CommsHostQ *q)
{
   q->cnt = 0;
   q->put = 0;
   q->get = 0;
   q->overrun = 0;
}



/*-----------------------------------------------------------------------------------------
|
|  initQ
|
|  initialise a S_CommsHostQ. 'buf' is the buffer
|
------------------------------------------------------------------------------------------*/

PRIVATE void initQ(S_CommsHostQ *q, S_ByteBuf CONST *buf)
{
   q->buf = buf;
   FlushCommsQ(q);
}

/*-----------------------------------------------------------------------------------------
|
|  StartCommsQ
|
|  Initialise queues to UART0 and enable UART..
|
|  PARAMETERS:
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

PUBLIC void StartCommsQ(S_ByteBuf CONST *txBuf, S_ByteBuf CONST *rxBuf)
{
   initQ(&Comms_InQ, rxBuf);
   initQ(&Comms_OutQ, txBuf);
   DoingTx = 0;
   UART0_Enable();
}

// --------------------------------- eof --------------------------------------------




