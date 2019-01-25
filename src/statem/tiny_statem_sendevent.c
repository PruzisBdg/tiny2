/*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|
|--------------------------------------------------------------------------*/


#include "common.h"
#include "tiny_statem.h"

/*--------------------------------------------------------------------------------
|
|  TinySM_SendEvent()
|
|  Add 'event' to the event list of 'sm'
|
-----------------------------------------------------------------------------------*/


PUBLIC U8 TinySM_SendEvent( S_TinySM RAM_IS *sm, TinySM_Event event )
{
    TinySM_EventCnt c, d;

   if( event == TinySM_Event_Tick ) {                       // a timer tick
      sm->events.ticks = 1;                                 // Since TinySM_ itself sends the ticks and then excutes
      return TRUE;
      }                                                     // the stateM, no need to increment, just set to 1
   else {
      if( sm->events.cnt >= TinySM_MaxEvents ) {            // event list full?
         return FALSE;
         }
      else {                                                // else add new event
         /* Events may have been read from the list out of order. So first
            pack the list if necessary
         */
         for( c = 0; c < sm->events.cnt; c++ )              // for events 1st.. last
         {
            if( sm->events.buf[c] == 0 )                    // slot for event n empty?
            {
               for( d = c+1; d < TinySM_MaxEvents; d++ )    // search from next slot to end of list
               {
                  if( sm->events.buf[d] != 0 )              // (1st) non-empty slot?
                  {
                     sm->events.buf[c] = sm->events.buf[d]; // then copy event to last empty slot
                     break;                                 // and onto next event
                  }
               }
            }
         }
         // Now list is packed so slot 'cnt' must be the 1st open one. Place the new
         // event there.
         sm->events.buf[sm->events.cnt] = event;
         sm->events.cnt++;
         return TRUE;
         }
      }
}

// ------------------------------- eof --------------------------------------- 
