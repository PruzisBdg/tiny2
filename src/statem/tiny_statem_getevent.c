/*---------------------------------------------------------------------------
|
|                        Tiny State Machine
|--------------------------------------------------------------------------*/


#include "common.h"
#include "tiny_statem.h"

/*--------------------------------------------------------------------------------
|
|  TinySM_GetEvent()
|
|  Searches for either a specific event or for any event in the event list of a
|  state machine.
|
|  Look for 'reqEvent' in the event list of 'sm'. If 'reqEvent' found return
|  'reqEvent' else 0.
|
|  If 'reqEvent' == 0 then return the first (earliest) event found searching
|  up the list. '_Tick' events are matched first.
|
|  The returned event is deleted from the list.
|
|  PARAMETERS: 'sm'        - the state machine
|              'reqEvent'  - event to get 
|
|  RETURNS:  an event OR 0 if no events or requested event not on list
|
-----------------------------------------------------------------------------------*/

PUBLIC TinySM_Event TinySM_GetEvent( S_TinySM_EventList RAM_IS *ev, TinySM_Event reqEvent )
{
   TinySM_EventCnt rIDATA c;
   TinySM_Event rIDATA e;

   if( reqEvent == TinySM_Event_Tick && ev->ticks == 0 ) {  // asked for a tick and there were none?
      return 0;                                             // then return 0 (no matching event)
      }
                                                            // else requested either any-event OR a tick AND there's at least one tick?
   else if( (reqEvent == 0 || reqEvent == TinySM_Event_Tick) && ev->ticks ) {                  
      ev->ticks = 0;                                        // then clear the entire tick count
      return TinySM_Event_Tick;                             // and return a tick
      }
   else {                                                   // else check other events
      if( !ev->cnt ) {                                      // no events?
         return 0;                                          // then quit with FAIL
         }
      else {
         for( c = 0; c < TinySM_MaxEvents; c++ ) {          // scan event list
            e = ev->buf[c];
   
            if( (reqEvent && e == reqEvent) ||              // requested specific event and found match? 
                 (!reqEvent && e) )                         // OR requested any event and found 1st?
               {
               ev->buf[c] = 0;                              // then clear it from list
               if(ev->cnt) ev->cnt--;                       // one less (Check to be sure)
               return e;                                    // and return event code
               }
            }
         return 0;                                          // requested specific event but no matches - return FAIL
         }
      }
}

// ------------------------------ eof ------------------------------------

 
