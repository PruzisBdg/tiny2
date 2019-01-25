/*---------------------------------------------------------------------------
|
|                 Tiny1 - Mini Flash Text File System
|
|--------------------------------------------------------------------------*/

#ifndef TXTFILE_H
#define TXTFILE_H

#include "libs_support.h"
#include "common.h"

#ifndef CODE_SPACE_IS
   #error "CODE_SPACE_IS must be defined"
#else
   #if CODE_SPACE_IS == CODE_SPACE_16BIT
   #elif CODE_SPACE_IS == CODE_SPACE_20BIT
      typedef U32 T_FlashAddr;
   #else
      #error "CODE_SPACE_IS must be one of these choices"
   #endif
#endif


// Specifies a bank of flash, one or more sectors, used to hold text

typedef struct
{
   T_FlashAddr start;        // start address of the 1st flash sector in the bank
   U8          sectors;      // number of (e.g 1024 byte) sectors
} S_TxtFileBank;

// Holds current text file system status
// Used by txtfile_UI only.
typedef struct
{
   U8    currentBank;   // the current bank in an array of S_TxtFileBank
   U16   cnt;           // character counter while writing to a store
} S_TxtFile;


// Specifies the entire Flash text file structure.
// Your application must make an instance of this.
typedef struct
{
   S_TxtFileBank CONST  *flashBanks;   // the text store, each one or more contiguous flash sectors
   U8                   numBanks;      // the number of 'flashBanks'
   U16                  sectorBytes;   // flash sector size
   U8 CONST *           bankNames;     // space delimited string of names, one for each bank
} S_TxtFiles;

// Gets the start of 'fileNum', if that is a legal file.
PUBLIC U8 CONST * File_Read( U8 fileNum );
// Checks that a file has some content
PUBLIC BIT File_NotEmpty(U8 fileNum);
// Check that bank name / number is legal 
PUBLIC BIT File_LegalBank_MsgIfNot(U8 *args, U8 *bank);

// Operations on the currently open file, specified by the global 'TxtF.currentBank'.
PUBLIC BIT  File_IsOpen(void);
PUBLIC void File_WrCh(U8 ch);
PUBLIC void File_WrCh_Num(U8 ch);
PUBLIC void File_List(void);
PUBLIC BIT  File_SendTimedOut(void);

// For script comment stripping, if scripts supported
extern BIT  File_WritingScript;      // We are writing a script to a file. Comemnts are stripped

PUBLIC void File_SetupWrite(void);
PUBLIC void File_SetupScriptWrite(void);
PUBLIC U8   File_StripScript(U8 ch);

// For number stores, if supported.
PUBLIC BIT         File_IsANumberStore(U8 fileNum);
PUBLIC S16         File_NumberStoreCount(U8 fileNum);
PUBLIC S16 CONST * File_NumberArrayStart(U8 fileNum);
PUBLIC S16         File_GetS16AtPtr(S16 CONST *p);
PUBLIC S16         File_GetS16AtIdx(S16 CONST *fileStart, S16 idx);

/* -------------- Specifying a Text File Store ----------------------------------

   To set up the text stores, the following must be defined in the applications 
   firmware file, usually 'firm.c'
*/

// Returns the 'S_TxtFiles' in which you have srt up your text file structure
extern S_TxtFiles CONST * File_GetStructure(void);

/* Reads a char (from the correct Flash bank).

   If the text is in another bank, then this function must block interrupts and switch
   banks before the read, and restore the context when done.
*/
extern U8 File_GetCh(U8 CONST *p);

// Returns the MCU bank for Flash writes. E.g for C8051F121 systems, this could be
// 0..3, and is usually 2.
extern U8 File_GetFlashBank(void);

// This must restores the original PSBANK settings (if PSBANK exists)
extern void File_ResetBankSw(void);

/* Called before reading from the from the Flash textfiles. If there is bank switching 
   then this call must disable interrupts and then switch to the text file bank.
   
   Interrupts must be off because they may execute MOVC and read const data from the
   wrong bank.
*/
extern void File_EnterTextBank(void);

// This call must return to the normal code bank and then reenable interrupts.
extern void File_LeaveTextBank(void);


/* ----------------------- end: Text Store Spec -------------------------------- */


#endif
