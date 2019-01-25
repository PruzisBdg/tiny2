/*---------------------------------------------------------------------------
|
|                    Framework 2 - Associations
|
|  So events can trigger multiple actions. 
|
|--------------------------------------------------------------------------*/

#ifndef LINKS_H
#define LINKS_H

#include "sysobj.h"

// An association between objects

typedef struct
{
   T_ObjAddr  sender;         // the sending object
   T_AnyAddr  dest;           // destination object or handler function
   U8         event;          // senders event to match
   U8         destEvent;      // event to be sent to dest.
   U8         linkType;       // precanned link type (below)
} S_Link;

// Precanned link types

#define _Link_Null               0
#define _Link_CopyToDest         1     // treat sender and dest as variables, U16, copy sender to dest
#define _Link_Execute_NoParms    2
#define _Link_Execute_wParms     3
#define _Link_MapEvent           4     // map to a different event, send to 'dest' with _SendEvent()

// Globally defined events (refer to sender)

#define _Link_Event_Null         0
#define _Link_Event_Created      1
#define _Link_Event_Destroyed    2
#define _Link_Event_Changed      3
#define _Link_Event_Started      4
#define _Link_Event_Stopped      5

PUBLIC void Link_SendEvent( T_ObjAddr sender, U8 event );

PUBLIC void Link_SenderChanged( T_ObjAddr sender );


#endif // LINKS_H
