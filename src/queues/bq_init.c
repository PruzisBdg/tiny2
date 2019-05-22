/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
|
|  Interlocked queues for fixed length data blocks.
|
|  Limits:
|
|     Data block size:              255 bytes
|     Queue capacity (blocks):      255
|
|  Storage for a 'BlockQueue' is allocated when the queue is created.
|
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "bq.h"

/* ------------------------------ Constructor -----------------------------------

   Return FALSE if couldn't allocate RAM for the queue.
*/

PUBLIC BOOLEAN bQ_Init( bq_S  bq_MEMSPACE  *bq,          // Make this
                        U8    bq_RAMSPACE  *ram,         // using this RAM
                        U8                 qSize,        // this many blocks
                        U8                 dataSize )    // bytes per block
{
   if(qSize == 0 || dataSize == 0)     /* Sizes legal? */
   {
      return FALSE;                    /* Fail if not */
   }
   else
   {
      bq->qSize = qSize;
      bq->dataSize = dataSize;
      bq->blockStep = dataSize;        /* No alignment. Block step is same as data size */

      bq->start = ram;
      bq->end = bq->start + ((U16)(bq->qSize-1) * bq->blockStep);    /* mark start of last slot */
      bQ_Flush(bq);                                                  /* 'put', 'get' set to queue start; 'cnt' = 0 */
      return TRUE;
   }
}

/* =================================== eof ================================= */
