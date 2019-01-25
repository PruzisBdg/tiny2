/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Standard User Interface Command Handlers
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"


/*-----------------------------------------------------------------------------------------
|
|  Echo mode control
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT EchoOn = 1;     // Defaults to on - for use at terminal



/*-----------------------------------------------------------------------------------------
|
|  Quiet mode control
|
|  To block unprompted printouts which get in the way of typing
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT quiet = 0;      // If '1', don't send unprompted ASCII to Host

PUBLIC BIT UI_BeQuiet(void) { return quiet == 1; }

// -------------------------------------- eof ------------------------------------------------
