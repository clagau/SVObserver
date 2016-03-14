//******************************************************************************
//* COPYRIGHT (c) 2001 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCrash
//* .File Name       : $Workfile:   svcrash.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:49:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVCrash.h"
#include "dbghelp.h"
#include "time.h"

// The original unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER g_pfnOrigFilter = NULL;

// The stack frame used in walking the stack
static STACKFRAME g_stFrame;

static TCHAR g_szBuff [ 1024 ];

static CRITICAL_SECTION oCriticalSection;

LPCTSTR szThreadNames[] = 
{
	_T("Unknown Thread"),		_T("Coreco Acq Thread"),
	_T("Display Thread"),		_T("Inspection Thread"),
	_T("Matrox Acq Thread"),	_T("Application Thread"),
	_T("DDE Pump Thread"),		_T("DDE Process Thread"),
	_T("Coreco Grab Thread"),	_T("Error Event Thread"),
	_T("Matrox Grab Thread"),	_T("PPQ Checker Thread"),
	_T("PPQ Timer Tread"),		_T("Timer Helper Thread"),
	_T("Pipe Connect Thread"),	_T("Pipe Open Thread"),
	_T("Pipe Shutdown Thread"),	_T("Pipe Close Thread"),
	_T("Pipe Read Thread"),		_T("Pipe Write Thread"),
	_T("RC Write Thread"),		_T("RC Receive Thread"),
	_T("Destroy Pipe Thread")
};// end szThreadNames

LPCTSTR __stdcall GetFirstStackTraceString( EXCEPTION_POINTERS *pExPtrs );

LPCTSTR __stdcall GetNextStackTraceString( EXCEPTION_POINTERS *pExPtrs );

// The internal function that does all the stack walking
LPCTSTR __stdcall InternalGetStackTraceString( EXCEPTION_POINTERS *pExPtrs );

// Our unhandled exception filter
LONG __stdcall CrashExceptionFilter( EXCEPTION_POINTERS *pExPtrs )
{

    LONG lRet = EXCEPTION_CONTINUE_SEARCH;
#ifdef USE_CRASH
	
    // If the exception is an EXCEPTION_STACK_OVERFLOW, there isn't much
    // you can do because the stack is blown. If you try to do anything,
    // the odds are great that you'll just double-fault and bomb right
    // out of your exception filter. 
    // Fortunately, EXCEPTION_STACK_OVERFLOW doesn't happen very often.
	
    if ( EXCEPTION_STACK_OVERFLOW == pExPtrs->ExceptionRecord->ExceptionCode )
    {
        OutputDebugString ( _T("EXCEPTION_STACK_OVERFLOW occurred\n") );
		return lRet;
    }
	
    EnterCriticalSection( &oCriticalSection );
	__try
    {
		long lType = GetThreadType();
		FILE *pfile;
		char pname[64];
		time_t ptime;

		time( &ptime );
		sprintf( pname, "C:\\SVObserver\\%d.txt", ptime );
		pfile = fopen( pname, "w" );

        fprintf ( pfile, "%s crashed!\n\n", szThreadNames[lType] );
        fflush ( pfile );

		LPCTSTR szBuff = GetFirstStackTraceString( pExPtrs );
        do
        {
            fprintf ( pfile, "%s\n", szBuff );
            fflush ( pfile );
            szBuff = GetNextStackTraceString( pExPtrs );
        }
        while ( NULL != szBuff );

		fclose( pfile );

	}
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        lRet = EXCEPTION_CONTINUE_SEARCH;
    }

	LeaveCriticalSection( &oCriticalSection );
#endif
    return ( lRet );
}

BOOL __stdcall EnableCrashFilter()
{
#ifdef USE_CRASH
	InitializeCriticalSection( &oCriticalSection );
	g_pfnOrigFilter = SetUnhandledExceptionFilter( CrashExceptionFilter );
#endif
	return TRUE;
}

BOOL __stdcall DisableCrashFilter()
{
#ifdef USE_CRASH
	DeleteCriticalSection( &oCriticalSection );
	SetUnhandledExceptionFilter( g_pfnOrigFilter );
#endif
	return TRUE;
}


// A static function to get the TIB.
static PTIB GetTIB ( void )
{
    PTIB pTib=nullptr;
#ifdef USE_CRASH

    __asm
    {
        MOV  EAX, FS:[18h]
        MOV  pTib, EAX
    }
#endif
    return ( pTib );

}

BOOL SetThreadType( long lType )
{
#ifdef USE_CRASH
    // Grab the TIB.
    PTIB pTib = GetTIB();

    // If someone has already written to the arbitrary field, I don't
    // want to be overwriting it.
    if ( NULL != pTib->pvArbitrary )
    {
        return ( FALSE );
    }

    // Nothing's there.  Set the name.
    pTib->pvArbitrary = (void*)lType;
#endif
    return ( TRUE );
}

long GetThreadType( void )
{
#ifdef USE_CRASH
    // Grab the TIB.
    PTIB pTib = GetTIB();

    return ( (long)pTib->pvArbitrary );
#else
	return 0;
#endif
}

LPCTSTR __stdcall GetFirstStackTraceString( EXCEPTION_POINTERS *pExPtrs )
{
#ifdef USE_CRASH
    // All the error checking is in the InternalGetStackTraceString
    // function.

    // Initialize the STACKFRAME structure.
    ZeroMemory ( &g_stFrame, sizeof ( STACKFRAME ) );

    g_stFrame.AddrPC.Offset       = pExPtrs->ContextRecord->Eip;
    g_stFrame.AddrPC.Mode         = AddrModeFlat;
    g_stFrame.AddrStack.Offset    = pExPtrs->ContextRecord->Esp;
    g_stFrame.AddrStack.Mode      = AddrModeFlat;
    g_stFrame.AddrFrame.Offset    = pExPtrs->ContextRecord->Ebp;
    g_stFrame.AddrFrame.Mode      = AddrModeFlat;

    return ( InternalGetStackTraceString( pExPtrs ) );
#else
	return NULL;
#endif
}

LPCTSTR __stdcall GetNextStackTraceString( EXCEPTION_POINTERS *pExPtrs )
{
#ifdef USE_CRASH
    // All error checking is in InternalGetStackTraceString.
    // Assume that GetFirstStackTraceString has already initialized the
    // stack frame information.
    return ( InternalGetStackTraceString( pExPtrs ) );
#else
	return NULL;
#endif
}

// The internal function that does all the stack walking
LPCTSTR __stdcall InternalGetStackTraceString( EXCEPTION_POINTERS *pExPtrs )
{

    // The value that is returned
    LPCTSTR szRet = NULL;
    // The module base address. I look this up right after the stack
    // walk to ensure that the module is valid.
#ifdef USE_CRASH

    __try
    {

#define CH_MACHINE IMAGE_FILE_MACHINE_I386

        // Note:  If the source file and line number functions are used,
        //        StackWalk can cause an access violation.
        BOOL bSWRet = StackWalk ( CH_MACHINE,
                                  (HANDLE)GetCurrentProcessId(),
                                  GetCurrentThread(),
                                  &g_stFrame,
                                  pExPtrs->ContextRecord,
                                  (PREAD_PROCESS_MEMORY_ROUTINE)
                                   ReadProcessMemory,
                                  SymFunctionTableAccess,
                                  SymGetModuleBase,
                                  NULL );
        if ( ( FALSE == bSWRet ) || ( 0 == g_stFrame.AddrFrame.Offset ))
        {
            szRet = NULL;
            __leave;
        }

        int iCurr = 0;

        // At a minimum, put in the address.
        iCurr += wsprintf ( g_szBuff + iCurr,
                            _T ( "%04X:%08X" ),
                            pExPtrs->ContextRecord->SegCs,
                            g_stFrame.AddrPC.Offset        );

        szRet = g_szBuff;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        ASSERT ( !"Crashed in InternalGetStackTraceString" );
        szRet = NULL;
    }
#endif
    return ( szRet );
}

