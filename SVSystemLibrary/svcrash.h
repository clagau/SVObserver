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

#pragma once

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

