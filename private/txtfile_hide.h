/*---------------------------------------------------------------------------
|
|                       Mini Text Files in Flash
|
|              Non-public stuff common to the txtfile group
|
|
|  $Workfile:   txtfile_hide.h  $                                  
|                                                 
|  $Header:   Q:/Software/Archive/CSI_Software_DataBase/archives/Common/spj/tiny1/txtfile/txtfile_hide.h-arc   1.1   Jan 26 2011 12:05:56   spruzina  $                                  
|
|  $Log:   Q:/Software/Archive/CSI_Software_DataBase/archives/Common/spj/tiny1/txtfile/txtfile_hide.h-arc  $
|   
|      Rev 1.1   Jan 26 2011 12:05:56   spruzina
|   Add  file_WhenGotLastChar'. This timer used to finish up streaming data frm a Host fiel to Flash.
|   
|      Rev 1.0   Jan 17 2011 12:53:16   spruzina
|   Initial revision.
|
|--------------------------------------------------------------------------*/

#ifndef TXTFILE_HIDE_H
#define TXTFILE_HIDE_H

#include "common.h"
#include "flash_basic.h"   // T_FlashAddr
#include "systime.h"

PUBLIC void file_setFlashBank(void);
PUBLIC void file_wrLineToStore(void);
PUBLIC BIT  file_wrCharToStore(U8 ch);
PUBLIC void file_WrCh_BumpPtr(U8 ch);
PUBLIC void file_WrS16_BumpPtr(S16 n);
PUBLIC void file_WrChAtIdx(S16 idx, U8 ch);
PUBLIC void file_WrS16AtIdx(S16 idx, S16 n);

extern S_TxtFile TxtF;              // Says what is the current text store 
extern BIT     inFileMode;          // Incoming chars from the terminal will be streamed to Flash
extern BIT     overflowedStore;     // More chars sent than the store will hold.
extern BIT     terminalMode;        // Characters are being hand-typed from a terminal (not streamed from a file)
extern T_Timer file_WhenGotLastChar;// Time last char sent from terminal. Used to autoclose when streaming from a file

// For number stores only
extern BIT storeBinaryInts;


PUBLIC U16         File_BankSize(void);
PUBLIC T_FlashAddr File_BankStart(void);
PUBLIC U16         File_BytesInBank(void);

PUBLIC C8 CONST * File_GetBankNames(void);

/* ----------------------- Numeric Store Identifer --------------------------------

   Numeric stores are identified by this integer in the at the start of the store.
   The integer is msb 1st
*/

#define _File_NumericStoreTag 0x5A7B

#endif // TXTFILE_HIDE_H


 
