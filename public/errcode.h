/*---------------------------------------------------------------------------
|
|                        Tiny1 Error Codes
|
|--------------------------------------------------------------------------*/

#ifndef ERRCODE_H
#define ERRCODE_H

#include "common.h"

// For the Tiny1 executive
#define _TinyErrCode_BadStateVar 0x01        // State variable was trashed (checked before entering state function)
#define _TinyErrCode_BadStateRtn 0x02        // State function returned and illegal state variable

// For Flash-resident loaders
#define _LoaderErrCode_NoApp        0x70  // Checksum was bad for existing application. (Or there wasn't an app).
#define _LoaderErrCode_BadLoad      0x71  // Did download but app. checksun is bad
#define _LoaderErrCode_IllegalJump  0x72  // Illegal Jump (into one of the Flash rountines)
#define _LoaderErrCode_Aborted      0x73  // Download was aborted.

// Codes above 0x80 are reserved for application-specific errors

// The application must supply this
PUBLIC void TrapSoftError(U8 errNum);

#endif // ERRCODE_H
 
