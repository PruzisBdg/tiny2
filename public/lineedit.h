/*---------------------------------------------------------------------------
|
|                          Multi-line editor
|
|  Multiple single-line edit buffers. Allows editing of a command line before entry
|  and recall of previous command lines for modification and reuse.
|
|--------------------------------------------------------------------------*/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include "common.h"

PUBLIC void LineEdit_ClearCurrentBuffer(void);
PUBLIC U8 * LineEdit_CurrentBuf(void);
PUBLIC U8 LineEdit_CharsInCurrentBuf(void);
PUBLIC void LineEdit_Init(void);
PUBLIC U8   LineEdit_AddCh( U8 ch );
PUBLIC void LineEdit_AcceptLine(void);
PUBLIC BIT LineEdit_HostIsQuiet(void);

#endif // LINEEDIT_H
 
