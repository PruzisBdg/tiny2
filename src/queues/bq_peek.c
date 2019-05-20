/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "bq.h"

/* ---------------------------- bQ_Peek --------------------------------------

   Return a reference directly to 'q->get' i.e the head. Return NULL if queue
   is empty.

   This call presumes that items on 'q' are stored as-is; not wrapped or transformed.
   It's only call to presume that.  If this isn't a guarantee then use bQ_Copy() to
   get the head with removing it.
*/

PUBLIC U8 bq_RAMSPACE const * bQ_Peek( bq_S bq_MEMSPACE *q )
{
   /* empty? */
   if( !q->cnt )
   {
      return NULL;
   }
   /* otherwise copy out the head item. */
   else
   {
      return q->get;
   }
}


// --------------------------------- eof -----------------------------------
