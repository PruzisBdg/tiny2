/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "common.h"
#include "txtfile.h"
#include "systime.h"
#include "txtfile_hide.h"


/*-----------------------------------------------------------------------------------------
|
|  File_SendTimedOut()
|
|  Test if there was a timeout streaming data from a terminal to Flash.
|
|  Returns 1 if not in terminal mode (which means hand-typed) AND it's
|  more than 2 secs since last char was received. 
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT File_SendTimedOut(void)
{
   return                                                // File stream from a terminal is done if...
      inFileMode &&                                      // Actually in file mode? AND
      !terminalMode &&                                   // Not in terminal-mode (i.e hand-typed to line editor)? AND
      ElapsedS(file_WhenGotLastChar) > _TicksSec_S(2);   // more than 2 secs since last char?
}

 
