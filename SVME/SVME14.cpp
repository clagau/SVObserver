// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIONIDAQ
// * .File Name       : $Workfile:   SVME14.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:43:58  $
// ******************************************************************************

#include "stdafx.h"

#include "SVME14.h"

typedef int(_cdecl *SVME14GetBoardVersionPtr) (int, int *);
typedef int(_cdecl *SVME14GetDLLVersionPtr) (void);
typedef int(_cdecl *SVME14DIOSetModePtr) (int, int, int);
typedef int(_cdecl *SVME14DIOSetPortDirectionPtr) (int, int, int);
typedef int(_cdecl *SVME14DIGetBitPtr) (int, int, int, int *);
typedef int(_cdecl *SVME14DIGetBytePtr) (int, int, int *);
typedef int(_cdecl *SVME14DOSetBitPtr) (int, int, int, int);
typedef int(_cdecl *SVME14DOSetBytePtr) (int, int, int);
typedef int(_cdecl *SVME14CntReadPtr) (int, int, int*);
typedef int(_cdecl *SVME14CntWritePtr) (int, int, int, int);
typedef int(_cdecl *SVME14DisableIntPtr) (int, int);
typedef int(_cdecl *SVME14EnableIntPtr) (int, SVME14CallbackPtr, int);
typedef int(_cdecl *SVME14GetDrvErrMessPtr) (char *);

SVME14GetBoardVersionPtr g_pME14GetBoardVersion = NULL;
SVME14GetDLLVersionPtr g_pME14GetDLLVersion = NULL;
SVME14DIOSetModePtr g_pME14DIOSetMode = NULL;
SVME14DIOSetPortDirectionPtr g_pME14DIOSetPortDirection = NULL;
SVME14DIGetBitPtr g_pME14DIGetBit = NULL;
SVME14DIGetBytePtr g_pME14DIGetByte = NULL;
SVME14DOSetBitPtr g_pME14DOSetBit = NULL;
SVME14DOSetBytePtr g_pME14DOSetByte = NULL;
SVME14CntReadPtr g_pME14CntRead = NULL;
SVME14CntWritePtr g_pME14CntWrite = NULL;
SVME14DisableIntPtr g_pME14DisableInt = NULL;
SVME14EnableIntPtr g_pME14EnableInt = NULL;
SVME14GetDrvErrMessPtr g_pME14GetDrvErrMess = NULL;

HMODULE g_hLibrary = NULL;

HRESULT g_ME14Open()
{
	HRESULT l_hrOk = S_OK;

	if ( g_hLibrary == NULL )
	{
		g_hLibrary = ::LoadLibrary( "me14_32.dll" );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		g_pME14GetBoardVersion = (SVME14GetBoardVersionPtr)::GetProcAddress( g_hLibrary, "_me14BoardVersion" );
		g_pME14GetDLLVersion = (SVME14GetDLLVersionPtr)::GetProcAddress( g_hLibrary, "_me14GetDLLVersion" );
		g_pME14DIOSetMode = (SVME14DIOSetModePtr)::GetProcAddress( g_hLibrary, "_me14DIOSetMode" );
		g_pME14DIOSetPortDirection = (SVME14DIOSetPortDirectionPtr)::GetProcAddress( g_hLibrary, "_me14DIOSetPortDirection" );
		g_pME14DIGetBit = (SVME14DIGetBitPtr)::GetProcAddress( g_hLibrary, "_me14DIGetBit" );
		g_pME14DIGetByte = (SVME14DIGetBytePtr)::GetProcAddress( g_hLibrary, "_me14DIGetByte" );
		g_pME14DOSetBit = (SVME14DOSetBitPtr)::GetProcAddress( g_hLibrary, "_me14DOSetBit" );
		g_pME14DOSetByte = (SVME14DOSetBytePtr)::GetProcAddress( g_hLibrary, "_me14DOSetByte" );
		g_pME14CntRead = (SVME14CntReadPtr)::GetProcAddress( g_hLibrary, "_me14CntRead" );
		g_pME14CntWrite = (SVME14CntWritePtr)::GetProcAddress( g_hLibrary, "_me14CntWrite" );
		g_pME14DisableInt = (SVME14DisableIntPtr)::GetProcAddress( g_hLibrary, "_me14DisableInt" );
		g_pME14EnableInt = (SVME14EnableIntPtr)::GetProcAddress( g_hLibrary, "_me14EnableInt" );
		g_pME14GetDrvErrMess = (SVME14GetDrvErrMessPtr)::GetProcAddress( g_hLibrary, "_me14GetDrvErrMess" );

		if ( g_pME14GetBoardVersion != NULL &&
		     g_pME14GetDLLVersion != NULL &&
		     g_pME14DIOSetMode != NULL &&
		     g_pME14DIOSetPortDirection != NULL &&
		     g_pME14DIGetBit != NULL &&
		     g_pME14DIGetByte != NULL &&
		     g_pME14DOSetBit != NULL &&
		     g_pME14DOSetByte != NULL &&
		     g_pME14CntRead != NULL &&
		     g_pME14CntWrite != NULL &&
		     g_pME14DisableInt != NULL &&
		     g_pME14EnableInt != NULL &&
		     g_pME14GetDrvErrMess != NULL )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = S_FALSE;

			g_ME14Close();
		}
	}

	return l_hrOk;
}

HRESULT g_ME14Close()
{
	HRESULT l_hrOk = S_OK;

	g_pME14GetBoardVersion = NULL;
	g_pME14GetDLLVersion = NULL;
	g_pME14DIOSetMode = NULL;
	g_pME14DIOSetPortDirection = NULL;
	g_pME14DIGetBit = NULL;
	g_pME14DIGetByte = NULL;
	g_pME14DOSetBit = NULL;
	g_pME14DOSetByte = NULL;
	g_pME14CntRead = NULL;
	g_pME14CntWrite = NULL;
	g_pME14DisableInt = NULL;
	g_pME14EnableInt = NULL;
	g_pME14GetDrvErrMess = NULL;

	if ( g_hLibrary != NULL )
	{
		if ( ::FreeLibrary( g_hLibrary ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			g_hLibrary = NULL;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	
	return l_hrOk;
}

int g_ME14GetBoardVersion(int iBoardNumber, int *piVersion)
{
	int l_iResult = 0;

	if ( g_pME14GetBoardVersion != NULL )
	{
		l_iResult = g_pME14GetBoardVersion( iBoardNumber, piVersion );
	}

	return l_iResult;
}

int g_ME14GetDLLVersion()
{
	int l_iResult = 0;

	if ( g_pME14GetDLLVersion != NULL )
	{
		l_iResult = g_pME14GetDLLVersion();
	}

	return l_iResult;
}

int g_ME14DIOSetMode(int iBoardNumber, int iPio, int iMode)
{
	int l_iResult = 0;

	if ( g_pME14DIOSetMode != NULL )
	{
		l_iResult = g_pME14DIOSetMode( iBoardNumber, iPio, iMode );
	}

	return l_iResult;
}

int g_ME14DIOSetPortDirection(int iBoardNumber, int iPort, int iDir)
{
	int l_iResult = 0;

	if ( g_pME14DIOSetPortDirection != NULL )
	{
		l_iResult = g_pME14DIOSetPortDirection( iBoardNumber, iPort, iDir );
	}

	return l_iResult;
}

int g_ME14DIGetBit(int iBoardNumber, int iPortNo, int iBitNo, int *piValue)
{
	int l_iResult = 0;

	if ( g_pME14DIGetBit != NULL )
	{
		l_iResult = g_pME14DIGetBit( iBoardNumber, iPortNo, iBitNo, piValue );
	}

	return l_iResult;
}

int g_ME14DIGetByte(int iBoardNumber, int iPortNo, int *piValue)
{
	int l_iResult = 0;

	if ( g_pME14DIGetByte != NULL )
	{
		l_iResult = g_pME14DIGetByte( iBoardNumber, iPortNo, piValue );
	}

	return l_iResult;
}

int g_ME14DOSetBit(int iBoardNumber, int iPortNo, int iBitNo, int iBitValue)
{
	int l_iResult = 0;

	if ( g_pME14DOSetBit != NULL )
	{
		l_iResult = g_pME14DOSetBit( iBoardNumber, iPortNo, iBitNo, iBitValue );
	}

	return l_iResult;
}

int g_ME14DOSetByte(int iBoardNumber, int iPortNo, int iValue)
{
	int l_iResult = 0;

	if ( g_pME14DOSetByte != NULL )
	{
		l_iResult = g_pME14DOSetByte( iBoardNumber, iPortNo, iValue );
	}

	return l_iResult;
}

int g_ME14CntRead(int iBoardNumber, int iCounter, int* piValue)
{
	int l_iResult = 0;

	if ( g_pME14CntRead != NULL )
	{
		l_iResult = g_pME14CntRead( iBoardNumber, iCounter, piValue);
	}

	return l_iResult;
}

int g_ME14CntWrite(int iBoardNumber, int iCounter,int iMode, int iValue)
{
	int l_iResult = 0;

	if ( g_pME14CntWrite != NULL )
	{
		l_iResult = g_pME14CntWrite( iBoardNumber, iCounter, iMode, iValue );
	}

	return l_iResult;
}

int g_ME14DisableInt(int iBoardNumber, int iServiceNo)
{
	int l_iResult = 0;

	if ( g_pME14DisableInt != NULL )
	{
		l_iResult = g_pME14DisableInt( iBoardNumber, iServiceNo );
	}

	return l_iResult;
}

int g_ME14EnableInt(int iBoardNumber, SVME14CallbackPtr IrqFunc, int iServiceNo)
{
	int l_iResult = 0;

	if ( g_pME14EnableInt != NULL )
	{
		l_iResult = g_pME14EnableInt( iBoardNumber, IrqFunc, iServiceNo );
	}

	return l_iResult;
}

int g_ME14GetDrvErrMess(char *pcErrorText)
{
	int l_iResult = 0;

	if ( g_pME14GetDrvErrMess != NULL )
	{
		l_iResult = g_pME14GetDrvErrMess( pcErrorText );
	}

	return l_iResult;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVME\SVME14.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:43:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Oct 2007 16:04:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Sleep after Load and Free Library to help system resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2003 15:37:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated callback information and changed calling convention type for Meilhaus functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:27:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Meilhaus Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/