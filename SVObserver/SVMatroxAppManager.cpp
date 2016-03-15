// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxAppManager
// * .File Name       : $Workfile:   SVMatroxAppManager.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:30:30  $
// ******************************************************************************

// SVMatroxAppManager.cpp: implementation for the SVMatroxAppManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "SVMatroxAppManager.h"

#include "SVException.h"
#include "SVMessage.h"

#define SETMILEXCEPTION(e, eCode, eMsg, sSourceFileName, lSourceLine) e.SetException (eCode, _T(__DATE__), _T(__TIME__), eMsg, sSourceFileName, lSourceLine, _T(__TIMESTAMP__) );

bool SVMatroxAppManager::m_bDisplayErrors = true;
int SVMatroxAppManager::m_nRefCount = 0;
MIL_ID SVMatroxAppManager::m_MilApp = M_NULL;
MAPPHOOKFCTPTR SVMatroxAppManager::m_pfnMilErrorHandler = NULL;
void*          SVMatroxAppManager::m_pHandlerUserPtr = NULL;

SVMatroxAppManager::SVMatroxAppManager()
{
	m_nRefCount++;
	// just the creation of this class should initialize the 
	// Matrox Application. The MIL_ID for the application is not used elsewhere
	// THIS MUST HAPPEN IN THE MAIN APPLICATION THREAD.
	ASSERT( AfxGetApp() && ::GetCurrentThreadId() == AfxGetApp()->m_nThreadID );
	GetApp();
}

SVMatroxAppManager::~SVMatroxAppManager()
{
	m_nRefCount--;
	if ( m_nRefCount == 0 )
	{
		if ( m_MilApp != M_NULL )
		{
			// cleanup


			MappFree ( m_MilApp );  // Free the application
			m_MilApp = M_NULL;
		}
	}
}

MIL_ID SVMatroxAppManager::GetApp()
{
	if ( m_MilApp == M_NULL && m_nRefCount > 0 )
	{
		ASSERT( AfxGetApp() && ::GetCurrentThreadId() == AfxGetApp()->m_nThreadID );
		if ( AfxGetApp() && ::GetCurrentThreadId() == AfxGetApp()->m_nThreadID )
		{
			// Allocate an application
			m_MilApp = MappAlloc( M_DEFAULT, M_NULL );
			ASSERT( m_MilApp !=	M_NULL );

			// Disable MIL error message to be displayed by MIL
			MappControl(M_ERROR, M_PRINT_DISABLE);
			// Retrieve previous handler ptr and user handler ptr
			MappInquire(M_CURRENT_ERROR_HANDLER_PTR, &m_pfnMilErrorHandler);
			MappInquire(M_CURRENT_ERROR_HANDLER_USER_PTR, &m_pHandlerUserPtr);
			// disable previous
			MappHookFunction(M_UNHOOK + M_ERROR_CURRENT, m_pfnMilErrorHandler, m_pHandlerUserPtr);
			// Hook into MIL error handling
			MappHookFunction(M_ERROR_CURRENT, SVMatroxAppManager::HandleError, NULL);


			// check version of MIL
			double dMilVersion = 0.0;
			MappInquire( M_VERSION, &dMilVersion );

			if ( fabs( M_MIL_CURRENT_VERSION - dMilVersion ) > 0.09 )
			{
				CString sMsg;
				sMsg.Format(_T("This version of SVObserver will not run with the installed MIL library. SVObserver needs MIL %4.1f. The installed version is %4.1f"),
					M_MIL_CURRENT_VERSION, dMilVersion );
				::MessageBox(NULL, sMsg, _T("SVObserver"), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
				exit( EXIT_FAILURE );
			}

		}
	}

	return m_MilApp;
}

/*static*/ void SVMatroxAppManager::SetErrorDisplay( bool bDisplay )
{
	m_bDisplayErrors = bDisplay;
}

long SVMatroxAppManager::GetStatusCode( TCHAR* p_pszSourceFile, long p_SourceLine )
{
	long l_StatusCode( M_NULL_ERROR );

	MappGetError( M_CURRENT + M_THREAD_CURRENT, &l_StatusCode );

	if( l_StatusCode != M_NULL_ERROR )
	{
		CString l_StatusString( GetStatusString() );

		SVException e;
		SETMILEXCEPTION( e, SVMSG_SVO_36_MIL_ERROR, l_StatusString, p_pszSourceFile, p_SourceLine );
		e.LogException( l_StatusString );
	}

	return l_StatusCode;
}

long SVMatroxAppManager::GetStatusCode( TCHAR* p_pszSourceFile, long p_SourceLine, const SVMatroxLongSet& p_rExcludeStatusCodesFromLogging )
{
	long l_StatusCode( M_NULL_ERROR );

	MappGetError( M_CURRENT + M_THREAD_CURRENT, &l_StatusCode );

	if( l_StatusCode != M_NULL_ERROR )
	{
		SVMatroxLongSet::const_iterator l_Iter;

		l_Iter = p_rExcludeStatusCodesFromLogging.find( l_StatusCode );

		if( l_Iter == p_rExcludeStatusCodesFromLogging.end() )
		{
			CString l_StatusString( GetStatusString() );

			SVException e;
			SETMILEXCEPTION( e, SVMSG_SVO_36_MIL_ERROR, l_StatusString, p_pszSourceFile, p_SourceLine );
			e.LogException( l_StatusString );
		}
	}

	return l_StatusCode;
}

CString SVMatroxAppManager::GetStatusString()
{
	CString	l_StatusMessage;
	CString l_TempString;

	long l_Code( M_NULL_ERROR );
	MIL_TEXT_CHAR l_Message[ M_ERROR_MESSAGE_SIZE ] = "";

	long l_StatusSubCodeCount( 0 );

	MappGetError( M_CURRENT + M_THREAD_CURRENT, &l_Code );
	MappGetError( M_CURRENT + M_MESSAGE + M_THREAD_CURRENT, l_Message );

	l_TempString.Format("Status - %d: %s\n", l_Code, l_Message );
	l_StatusMessage += l_TempString;

	MappGetError( M_CURRENT_FCT + M_THREAD_CURRENT, &l_Code );
	MappGetError( M_CURRENT_FCT + M_MESSAGE + M_THREAD_CURRENT, l_Message );

	l_TempString.Format("Function - %d: %s\n", l_Code, l_Message );
	l_StatusMessage += l_TempString;

	MappGetError( M_CURRENT_SUB_NB + M_THREAD_CURRENT, &l_StatusSubCodeCount );

	if ( l_StatusSubCodeCount > 0 )
	{
		l_StatusMessage += "Status Details\n";

		MappGetError( M_CURRENT_SUB_1 + M_THREAD_CURRENT, &l_Code );	
		MappGetError( M_CURRENT_SUB_1 + M_MESSAGE + M_THREAD_CURRENT, l_Message );	

		l_TempString.Format("%d: %s\n", l_Code, l_Message );
		l_StatusMessage += l_TempString;
	}
	
	if ( l_StatusSubCodeCount > 1 )
	{
		MappGetError( M_CURRENT_SUB_2 + M_THREAD_CURRENT, &l_Code );	
		MappGetError( M_CURRENT_SUB_2 + M_MESSAGE + M_THREAD_CURRENT, l_Message );	

		l_TempString.Format("%d: %s\n", l_Code, l_Message );
		l_StatusMessage += l_TempString;
	}

	if ( l_StatusSubCodeCount > 2 )
	{
		MappGetError( M_CURRENT_SUB_3 + M_THREAD_CURRENT, &l_Code );	
		MappGetError( M_CURRENT_SUB_3 + M_MESSAGE + M_THREAD_CURRENT, l_Message );	

		l_TempString.Format("%d: %s\n", l_Code, l_Message );
		l_StatusMessage += l_TempString;
	}
	
	return l_StatusMessage;
}

/*static*/ long SVMatroxAppManager::GetError()
{
	long lErrorCode=0;
	MappGetError( M_CURRENT, &lErrorCode );
	return lErrorCode;
}

CString SVMatroxAppManager::GetErrorString()
{
	char    szErrorMessage[M_ERROR_MESSAGE_SIZE]         = "";
	MappGetError( M_MESSAGE + M_CURRENT, szErrorMessage );
	return CString( szErrorMessage );
}

CString SVMatroxAppManager::GetErrorStringFull()
{
	char    szErrorMessageFunction[M_ERROR_MESSAGE_SIZE] = "";
	char    szErrorMessage[M_ERROR_MESSAGE_SIZE]         = "";
	char    szErrorSubMessage1[M_ERROR_MESSAGE_SIZE]     = "";
	char    szErrorSubMessage2[M_ERROR_MESSAGE_SIZE]     = "";
	char    szErrorSubMessage3[M_ERROR_MESSAGE_SIZE]     = "";
	long    lNumSubCodes;
	CString	strErrorMessage;
	long lErrorCode=0;
	long lSubErrorCode[3]={0};
	
	MappGetError( M_MESSAGE + M_CURRENT_FCT, szErrorMessageFunction );
	MappGetError( M_MESSAGE + M_CURRENT, szErrorMessage );
	MappGetError( M_CURRENT, &lErrorCode );
	MappGetError( M_CURRENT_SUB_NB, &lNumSubCodes );
	if ( lNumSubCodes > 2 )
	{
		MappGetError( M_MESSAGE + M_CURRENT_SUB_3, szErrorSubMessage3 );	
		MappGetError( M_CURRENT_SUB_3, &(lSubErrorCode[2]) );	
	}
	if ( lNumSubCodes > 1 )
	{
		MappGetError( M_MESSAGE + M_CURRENT_SUB_2, szErrorSubMessage2 );
		MappGetError( M_CURRENT_SUB_2, &(lSubErrorCode[1]) );	
	}
	if ( lNumSubCodes > 0 )
	{
		MappGetError( M_MESSAGE + M_CURRENT_SUB_1, szErrorSubMessage1 );
		MappGetError( M_CURRENT_SUB_1, &(lSubErrorCode[0]) );	
	}
	
	strErrorMessage = szErrorMessageFunction;
	strErrorMessage = strErrorMessage + "\n";
	CString sErrorCode;
	sErrorCode.Format("%d", lErrorCode);
	strErrorMessage = strErrorMessage + sErrorCode + CString(": ") + szErrorMessage;
	
	if( lNumSubCodes > 0 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage1;
	}
	
	if( lNumSubCodes > 1 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage2;
	}
	
	if( lNumSubCodes > 2 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage3;
	}

	return strErrorMessage;
}

/*static*/ long MFTYPE SVMatroxAppManager::HandleError(long lHookType, MIL_ID EventId, void MPTYPE* pUserData)
{
	//SVMatroxAppManager* pThis = (SVMatroxAppManager*) pUserData;
	

	char    szErrorMessageFunction[M_ERROR_MESSAGE_SIZE] = "";
	char    szErrorMessage[M_ERROR_MESSAGE_SIZE]         = "";
	char    szErrorSubMessage1[M_ERROR_MESSAGE_SIZE]     = "";
	char    szErrorSubMessage2[M_ERROR_MESSAGE_SIZE]     = "";
	char    szErrorSubMessage3[M_ERROR_MESSAGE_SIZE]     = "";
	long    lNumSubCodes;
	CString	strErrorMessage;
	long lErrorCode=0;
	long lSubErrorCode[3]={0};
	
	MappGetHookInfo( EventId, M_MESSAGE + M_CURRENT_FCT, szErrorMessageFunction );
	MappGetHookInfo( EventId, M_MESSAGE + M_CURRENT, szErrorMessage );
	MappGetHookInfo( EventId, M_CURRENT, &lErrorCode );
	MappGetHookInfo( EventId, M_CURRENT_SUB_NB, &lNumSubCodes );
	if ( lNumSubCodes > 2 )
	{
		MappGetHookInfo( EventId, M_MESSAGE + M_CURRENT_SUB_3, szErrorSubMessage3 );	
		MappGetHookInfo( EventId, M_CURRENT_SUB_3, &(lSubErrorCode[2]) );	
	}
	if ( lNumSubCodes > 1 )
	{
		MappGetHookInfo( EventId, M_MESSAGE + M_CURRENT_SUB_2, szErrorSubMessage2 );
		MappGetHookInfo( EventId, M_CURRENT_SUB_2, &(lSubErrorCode[1]) );	
	}
	if ( lNumSubCodes > 0 )
	{
		MappGetHookInfo( EventId, M_MESSAGE + M_CURRENT_SUB_1, szErrorSubMessage1 );
		MappGetHookInfo( EventId, M_CURRENT_SUB_1, &(lSubErrorCode[0]) );	
	}
	
	strErrorMessage = szErrorMessageFunction;
	strErrorMessage = strErrorMessage + "\n";
	CString sErrorCode;
	sErrorCode.Format("%d", lErrorCode);
	strErrorMessage = strErrorMessage + sErrorCode + CString(": ") + szErrorMessage;
	
	if( lNumSubCodes > 0 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage1;
	}
	
	if( lNumSubCodes > 1 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage2;
	}
	
	if( lNumSubCodes > 2 )
	{
		strErrorMessage = strErrorMessage + "\n";
		strErrorMessage = strErrorMessage + szErrorSubMessage3;
	}


	TRACE("MIL Error: %s\n", strErrorMessage);

	// special case ignore these errors until matrox fixes them
	// they should be fixed with 7.5 Patch 6
	/*
	if ( lErrorCode == 2320 && CString( szErrorMessageFunction ) == "MdigFree" )// error writing to camera register
	{
		return M_NULL;
	}
	*/

	if ( m_bDisplayErrors )
	{
		static int bShow = -1;
		if ( bShow == -1 && AfxGetApp() != NULL )
		{
			bShow = AfxGetApp()->GetProfileInt("Debug", "ShowAllMILErrors", FALSE);
			AfxGetApp()->WriteProfileInt("Debug", "ShowAllMILErrors", bShow);
		}

		bool l_bContinue = false;

		if ( bShow == TRUE )
		{
			#ifdef _DEBUG
				CString l_csTemp;

				l_csTemp.Format( "%s\n\nDo you wish to Assert?", strErrorMessage );

				l_bContinue = ::MessageBox(NULL, l_csTemp, "SVObserver", MB_YESNO | MB_SYSTEMMODAL ) != IDYES;
			#else
				::MessageBox(NULL, strErrorMessage, "SVObserver", MB_OK | MB_SYSTEMMODAL );
			#endif
		}

		ASSERT( l_bContinue );
	}
	
	/*
	// Chain hook function
	if( m_pfnMilErrorHandler )
		(*m_pfnMilErrorHandler)(lHookType, EventId, m_pHandlerUserPtr);
	//*/
	
	return M_NULL;
	
}

