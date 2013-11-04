//******************************************************************************
//* COPYRIGHT (c) 2001 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCrash
//* .File Name       : $Workfile:   svcrash.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 19:38:10  $
//******************************************************************************

#ifndef SVCRASH_H
#define SVCRASH_H

//===========================================================
// Taken from ....
// Author: Matt Pietrek
// From: Microsoft Systems Journal "Under the Hood", May 1996
//===========================================================
#pragma pack(1)

typedef struct _EXCEPTION_REGISTRATION_RECORD
{
    struct _EXCEPTION_REGISTRATION_RECORD * pNext;
    FARPROC                                 pfnHandler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

typedef struct _TIB
{
PEXCEPTION_REGISTRATION_RECORD pvExcept; // 00h Head of exception record list
PVOID   pvStackUserTop;     // 04h Top of user stack
PVOID   pvStackUserBase;    // 08h Base of user stack

union                       // 0Ch (NT/Win95 differences)
{
    struct  // Win95 fields
    {
        WORD    pvTDB;          // 0Ch TDB
        WORD    pvThunkSS;      // 0Eh SS selector used for thunking to 16 bits
        DWORD   unknown1;       // 10h
    } WIN95;

    struct  // WinNT fields
    {
        PVOID SubSystemTib;     // 0Ch
        ULONG FiberData;        // 10h
    } WINNT;
} TIB_UNION1;

PVOID   pvArbitrary;        // 14h Available for application use
struct _tib *ptibSelf;      // 18h Linear address of TIB structure

union                       // 1Ch (NT/Win95 differences)
{
    struct  // Win95 fields
    {
        WORD    TIBFlags;           // 1Ch
        WORD    Win16MutexCount;    // 1Eh
        DWORD   DebugContext;       // 20h
        DWORD   pCurrentPriority;   // 24h
        DWORD   pvQueue;            // 28h Message Queue selector
    } WIN95;

    struct  // WinNT fields
    {
        DWORD unknown1;             // 1Ch
        DWORD processID;            // 20h
        DWORD threadID;             // 24h
        DWORD unknown2;             // 28h
    } WINNT;
} TIB_UNION2;

PVOID*  pvTLSArray;         // 2Ch Thread Local Storage array

union                       // 30h (NT/Win95 differences)
{
    struct  // Win95 fields
    {
        PVOID*  pProcess;           // 30h Pointer to owning process database
    } WIN95;
} TIB_UNION3;
    
} TIB, *PTIB;
#pragma pack()

static const long UNKNOWNTHREAD		= 0x00000000;
static const long CORECOACQTHREAD	= 0x00000001;
static const long DISPLAYTHREAD		= 0x00000002;
static const long INSPECTIONTHREAD	= 0x00000003;
static const long MATROXACQTHREAD	= 0x00000004;
static const long APPLICATIONTHREAD	= 0x00000005;
static const long CORECOGRABTHREAD	= 0x00000008;
static const long ERROREVENTTHREAD	= 0x00000009;
static const long MATROXGRABTHREAD	= 0x0000000A;
static const long PPQCHECKERTHREAD	= 0x0000000B;
static const long PPQTIMERTHREAD	= 0x0000000C;
static const long TIMERHELPERTHREAD	= 0x0000000D;
static const long PIPECONNECTTHREAD	= 0x0000000E;
static const long PIPEOPENTHREAD	= 0x0000000F;
static const long PIPESHUTDOWNTHREAD= 0x00000010;
static const long PIPECLOSETHREAD	= 0x00000011;
static const long PIPEREADTHREAD	= 0x00000012;
static const long PIPEWRITETHREAD	= 0x00000013;
static const long RCWRITETHREAD		= 0x00000014;
static const long RCRECEIVETHREAD	= 0x00000015;
static const long DESTROYPIPETHREAD	= 0x00000016;
static const long STREAMDATATHREAD	= 0x00000017;
static const long DISPLAYOBJTHREAD	= 0x00000018;
static const long INSPECTPROCTHREAD	= 0x00000019;
static const long OUTPUTDELAYTHREAD	= 0x0000001A;
static const long OUTPUTRESETTHREAD	= 0x0000001B;
static const long CAMERAPROCTHREAD	= 0x0000001C;
static const long DEVICECLASSTHREAD	= 0x0000001D;
static const long INTERFACETHREAD	= 0x0000001E;
static const long TIMERCLASSTHREAD	= 0x0000001F;
static const long PPQPROCESSTHREAD	= 0x00000020;

extern BOOL SetThreadType( long lType );
extern long GetThreadType( void );

extern BOOL __stdcall EnableCrashFilter();
extern BOOL __stdcall DisableCrashFilter();

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\svcrash.h_v  $
 * 
 *    Rev 1.1   06 May 2013 19:38:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Apr 2013 14:15:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2009 12:44:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 17:30:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:49:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Oct 2001 09:47:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  228
 * SCR Title:  Add context information to help the debugging of multi-threaded code
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   New code to store thread type identifiers in the TIB block for each thread that is run.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/