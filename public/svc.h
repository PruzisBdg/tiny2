/*---------------------------------------------------------------------------
|
|            Test Fixture Support - User and Service Mode
|
|--------------------------------------------------------------------------*/

#ifndef SVC_H
#define SVC_H

#include "common.h"

PUBLIC void Svc_Init( void );
PUBLIC void Svc_Init_AlwaysInSvc(void);      // For machine with no user/service. Everything always allowed.
PUBLIC U8   InServiceMode(void);
PUBLIC BIT  InServiceMode_MsgIfNot(void);
PUBLIC U8   Svc_HandleUI( U8 *args );

#endif
