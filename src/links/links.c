/*---------------------------------------------------------------------------
|
|              Tiny1 Framework - Associations
|
|  Public:
|     Link_SendEvent()
|     Link_SenderChanged()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "links.h"
#include "sysobj.h"           // T_ObjAddr

#ifndef USE_NANO_STATEM
   #error "USE_NANO_STATEM must be defined"
#else
   #if USE_NANO_STATEM == 1
      #include "nano_statem.h"      // NanoSM_SendEvent
   #else
      #include "tiny_statem.h"      // TinySM_SendEvent
   #endif
#endif

extern BOOL Firm_IsALegalExecutionAddr(T_AnyAddr n);
extern BOOL Firm_IsaWordWritableRAMAddr(T_AnyAddr n);



extern CONST S_Link Links_List[];
extern U8 Links_NumLinks();


/* The Links_ module calls link functions via function pointers. 8051 compilers typically
   overlay automatic variables. To do this they must determine the function call tree for the
   entire application. The Raisonance compiler misses some functions and so overlays when
   it shouldn't. Avoid this by disabling overlays for this module.
*/
#if _TOOL_IS == TOOL_RIDE_8051
#pragma NOOVERLAY
#endif

/*-----------------------------------------------------------------------------------
|
|  Link_SendEvent()
|
|  Send 'event' from 'sender' to the link handler. If the link table contains an entry
|  matching 'sender' and 'event' then the actions in the entry will be executed.
|
--------------------------------------------------------------------------------------*/

PUBLIC void Link_SendEvent( T_ObjAddr sender, U8 event )
{
   U8 c;
   S_Link CONST * p;

   for( c = Links_NumLinks(), p = Links_List; c; c--, p++ )             // for each entry in the link table
   {
      if( p->sender == sender &&                                        // entry matches sender? AND
          (!p->event || (p->event == event)) )                          // entry requires a matching event AND event does match?
      {
         switch(p->linkType)                                            // then perform action based on link type
         {
            case _Link_CopyToDest:
               if(Firm_IsaWordWritableRAMAddr(p->dest))
                  { *(U16 *)p->dest = *(U16 *)p->sender; }
               break;

            // Treat destination as a function; call it without parms

            case _Link_Execute_NoParms:
               if( Firm_IsALegalExecutionAddr(p->dest))
                  { ((void(*)(void))((T_CodeAddr)(p->dest)))(); }
               break;

            // Treat destination as a function to be passed sender and event

            case _Link_Execute_wParms:
               if( Firm_IsALegalExecutionAddr(p->dest))
                  { ((void(*)(T_ObjAddr, U8))((T_CodeAddr)(p->dest)))(p->sender, p->destEvent); }
               break;
               
            // 'dest' is a state machine; pass it the translated event   
               
            case _Link_MapEvent:
               #if USE_NANO_STATEM == 1
               NanoSM_SendEvent((S_NanoSM RAM_IS *)((T_ObjAddr)(p->dest)), p->destEvent);
               #else
               TinySM_SendEvent( (S_TinySM RAM_IS *)((T_ObjAddr)(p->dest)), p->destEvent);
               #endif
               break;
         }
      }
   }
}


/*-----------------------------------------------------------------------------------
|
|  Link_SenderChanged()
|
--------------------------------------------------------------------------------------*/

PUBLIC void Link_SenderChanged( T_ObjAddr sender )
{
   Link_SendEvent(sender, _Link_Event_Changed);
}


// -------------------------------- eof --------------------------------------------
