// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxApplicationInterface
// * .File Name       : $Workfile:   SVMatroxApplicationInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxEnums.h"
#include "SVMatroxImagingLibrary.h"
#include "SVStatusLibrary\SVException.h"
#include "SVMessage\SVMessage.h"

#include <assert.h>

long _stdcall SVMatroxApplicationInterface::SVMatroxHookHandler( long HookType, SVMatroxIdentifier EventId, void* UserDataPtr )
{
	MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];
	SVMatroxStatusInformation l_StatusInfo;

//	assert (0);
	MappGetHookInfo( EventId, M_CURRENT, &( l_StatusInfo.m_StatusCode ) );
	MappGetHookInfo( EventId, M_CURRENT + M_MESSAGE, l_strMilText );
	l_StatusInfo.m_StatusString = l_strMilText;
	
	MappGetHookInfo( EventId, M_CURRENT_FCT, &( l_StatusInfo.m_FunctionCode ) );
	MappGetHookInfo( EventId, M_CURRENT_FCT + M_MESSAGE, l_strMilText );
	l_StatusInfo.m_FunctionString = l_strMilText;

	MappGetHookInfo( EventId, M_CURRENT_SUB_NB, &( l_StatusInfo.m_StatusSubCodeCount ) );

	switch( l_StatusInfo.m_StatusSubCodeCount )
	{
		case 3:
		{
			MappGetHookInfo( EventId, M_CURRENT_SUB_3, &( l_StatusInfo.m_StatusSubCode[ 2 ] ) );
			MappGetHookInfo( EventId, M_CURRENT_SUB_3 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 2 ] = l_strMilText;
		}
		case 2:
		{
			MappGetHookInfo( EventId, M_CURRENT_SUB_2, &( l_StatusInfo.m_StatusSubCode[ 1 ] ) );
			MappGetHookInfo( EventId, M_CURRENT_SUB_2 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 1 ] = l_strMilText;
		}
		case 1:
		{
			MappGetHookInfo( EventId, M_CURRENT_SUB_1, &( l_StatusInfo.m_StatusSubCode[ 0 ] ) );
			MappGetHookInfo( EventId, M_CURRENT_SUB_1 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 0 ] = l_strMilText;
		}
		default:
		{
			break;
		}
	}

	if ((l_StatusInfo.m_StatusCode == 23002) &&			// Operation error
		(l_StatusInfo.m_FunctionCode == -2078767097) && // MocrGetResult
		(l_StatusInfo.m_StatusSubCodeCount == 1) &&	
		(l_StatusInfo.m_StatusSubCode[0] == 1))			// Specified result 
														// buffer does not 
														// contain valid read
														// or verify result 
														// data
	{
//-		JAB110708 - This case is put here to specifically exclude the case 
//-		when we attempt to retreive an OCR result when an OCR string was not 
//-		found.
	}
	else
	{
//-		JAB110708 - I put the following code here to force the Log function, and 
//-		the Event Viewer to consider this a critical error.  If there are 
//-		exceptions, then code them, but every error bringing us here should be
//-		assumed critical until demonstrated otherwise.
		l_StatusInfo.m_StatusCode = l_StatusInfo.m_StatusCode | SVMEE_MATROX_ERROR;
	}

	SVMatroxApplicationInterface::Log( l_StatusInfo );
	return l_StatusInfo.m_StatusCode;
}

void SVMatroxApplicationInterface::Log( SVMatroxStatusInformation &p_rStatusInfo )
{
	if( p_rStatusInfo.m_StatusCode != 0 )
	{
		SVException e;

		if( SUCCEEDED( p_rStatusInfo.m_StatusCode ) )
		{
			e.SetException( SVMSG_SVMATROXLIBRARY_NO_ERROR, -15408, p_rStatusInfo.m_StatusCode  );
		}
		else
		{
			e.SetException( SVMSG_SVMATROXLIBRARY_UNKNOWN_FATAL_ERROR, -15408, p_rStatusInfo.m_StatusCode  );
		}

		e.LogException( p_rStatusInfo.GetCompleteString().c_str() );
	}
}

void SVMatroxApplicationInterface::LogMatroxException( )
{
	SVMatroxStatusInformation l_info;
	l_info.m_StatusCode = SVMEE_MATROX_THREW_EXCEPTION;
	l_info.m_StatusString = "Matrox Threw an Exception";
	Log( l_info );
}

/**
@SVOperationName Shutdown

@SVOperationDescription

*/
void SVMatroxApplicationInterface::Shutdown()
{
	LocalClear();
}

/**
@SVOperationName Startup

@SVOperationDescription

*/
void SVMatroxApplicationInterface::Startup()
{
	LocalInitialize();
}

/**
@SVOperationName Get Last Status

@SVOperationDescription

*/
SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetLastStatus()
{
	SVStatusCode l_Status( SVMEE_STATUS_OK );
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		l_Status = MappGetError( M_THREAD_CURRENT + M_CURRENT, NULL );

		#ifdef _DEBUG
		if( l_Status != M_NULL_ERROR )
		{
			SVMatroxStatusInformation l_info;
			SVMatroxApplicationInterface::GetLastStatus( l_info );
			TCHAR buf[M_ERROR_MESSAGE_SIZE * 2];
			_stprintf_s( buf, M_ERROR_MESSAGE_SIZE * 2, _T("%d - %s\n"), l_info.m_FunctionCode, l_info.m_FunctionString.c_str() );
			::OutputDebugString(buf );
		}
		#endif

		if (l_Status != 0)
		{
			l_Status = l_Status | SVMEE_MATROX_ERROR;
		}	
	}
	else
	{
		l_Status = SVMEE_INVALID_HANDLE;
	}

	return l_Status;
}

/**
@SVOperationName Get Last Status to Status Information

@SVOperationDescription

*/
SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetLastStatus( SVMatroxStatusInformation& p_rStatus )
{
	SVStatusCode l_Status( M_NULL_ERROR );
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];

		p_rStatus.clear();

		l_Status = MappGetError( M_THREAD_CURRENT + M_CURRENT, &( p_rStatus.m_StatusCode ) );
		MappGetError( M_THREAD_CURRENT + M_CURRENT + M_MESSAGE, l_strMilText );
		p_rStatus.m_StatusString = l_strMilText;

		MappGetError( M_THREAD_CURRENT + M_CURRENT_FCT, &( p_rStatus.m_FunctionCode ) );
		MappGetError( M_THREAD_CURRENT + M_CURRENT_FCT + M_MESSAGE, l_strMilText );
		p_rStatus.m_FunctionString = l_strMilText;

		MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_NB, &( p_rStatus.m_StatusSubCodeCount ) );

		switch( p_rStatus.m_StatusSubCodeCount )
		{
			case 3:
			{
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_3, &( p_rStatus.m_StatusSubCode[ 2 ] ) );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_3 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 2 ] = l_strMilText;
			}
			case 2:
			{
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_2, &( p_rStatus.m_StatusSubCode[ 1 ] ) );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_2 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 1 ] = l_strMilText;
			}
			case 1:
			{
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_1, &( p_rStatus.m_StatusSubCode[ 0 ] ) );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_1 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 0 ] = l_strMilText;
			}
			default:
			{
				break;
			}
		}
	}
	else
	{
		l_Status = SVMEE_INVALID_HANDLE;
	}

	return l_Status;
}

/**
@SVOperationName Get First Error

@SVOperationDescription

*/
SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetFirstError()
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		l_Code = MappGetError( M_THREAD_CURRENT + M_GLOBAL, NULL );
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}

	return l_Code;
}

/**
@SVOperationName Get First Error to Status Information

@SVOperationDescription Gets the error and extended information for the last Matrox operation.

*/
SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetFirstError( SVMatroxStatusInformation& p_rStatus )
{
	SVStatusCode l_Status( M_NULL_ERROR );
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];

		p_rStatus.clear();

		l_Status = MappGetError( M_THREAD_CURRENT + M_GLOBAL, &( p_rStatus.m_StatusCode ) );
		MappGetError( M_THREAD_CURRENT + M_GLOBAL + M_MESSAGE, l_strMilText );
		p_rStatus.m_StatusString = l_strMilText;

		MappGetError( M_THREAD_CURRENT + M_GLOBAL_FCT, &( p_rStatus.m_FunctionCode ) );
		MappGetError( M_THREAD_CURRENT + M_GLOBAL_FCT + M_MESSAGE, l_strMilText );
		p_rStatus.m_FunctionString = l_strMilText;

		MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_NB, &( p_rStatus.m_StatusSubCodeCount ) );

		switch( p_rStatus.m_StatusSubCodeCount )
		{
			case 3:
			{
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_3, &( p_rStatus.m_StatusSubCode[ 2 ] ) );
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_3 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 2 ] = l_strMilText;
			}
			case 2:
			{
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_2, &( p_rStatus.m_StatusSubCode[ 1 ] ) );
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_2 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 1 ] = l_strMilText;
			}
			case 1:
			{
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_1, &( p_rStatus.m_StatusSubCode[ 0 ] ) );
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_1 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 0 ] = l_strMilText;
			}
			default:
			{
				break;
			}
		}
	}
	else
	{
		l_Status = SVMEE_INVALID_HANDLE;
	}

	return l_Status;
}

SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetSystemCount( long& p_lCount )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);
	if( appID != M_NULL )
	{
		#ifdef USE_TRY_BLOCKS
		try
		#endif
		{
			MappInquire(M_INSTALLED_SYSTEM_COUNT, &p_lCount);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		#ifdef USE_TRY_BLOCKS
		catch(...)
		{
			l_Code = SVMEE_MATROX_THREW_EXCEPTION;
			SVMatroxApplicationInterface::LogMatroxException();
		}
		#endif

		assert( l_Code == SVMEE_STATUS_OK );
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}

	return l_Code;
}

SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetSystemName( long p_lSystemNumber, SVMatroxString& p_rSystemName )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		#ifdef USE_TRY_BLOCKS
		try
		#endif
		{
			char name[256];
			MappInquire(M_INSTALLED_SYSTEM_DESCRIPTOR + p_lSystemNumber, &name);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if ( l_Code == SVMEE_STATUS_OK )
			{
				p_rSystemName = name;
			}
		}
		#ifdef USE_TRY_BLOCKS
		catch(...)
		{
			l_Code = SVMEE_MATROX_THREW_EXCEPTION;
			SVMatroxApplicationInterface::LogMatroxException();
		}
		#endif

		assert( l_Code == SVMEE_STATUS_OK );
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}

	return l_Code;
}
/**
@SVOperationName Local Initialize

@SVOperationDescription Initializes the application and sets the error hook.

*/
void SVMatroxApplicationInterface::LocalInitialize()
{
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);
	if( appID == M_NULL )
	{
		appID = MappAlloc( M_DEFAULT, M_NULL );

		if( appID != M_NULL )
		{
			double l_MilVersion( 0.0 );

			SVMatroxHookFunctionPtr l_pHandlerFunction = NULL;
			void* l_pHandlerUserData = NULL;

			// Disable MIL error message to be displayed by MIL
			MappControl( M_ERROR, M_PRINT_DISABLE );

			// Retrieve previous handler ptr and user handler ptr
			MappInquire( M_CURRENT_ERROR_HANDLER_PTR, &l_pHandlerFunction );
			MappInquire( M_CURRENT_ERROR_HANDLER_USER_PTR, &l_pHandlerUserData );

			// disable previous
			MappHookFunction( M_ERROR_CURRENT + M_UNHOOK, l_pHandlerFunction, l_pHandlerUserData );

			// Hook into MIL error handling
			MappHookFunction( M_ERROR_CURRENT, SVMatroxHookHandler, NULL );

			// check version of MIL
			MappInquire( M_VERSION, &l_MilVersion );

			long l_lMilVersion = static_cast<long>(l_MilVersion * 0x100);

			if ( SV_CURRENT_MIL_VERSION != l_lMilVersion )
			{
				TCHAR l_szMsg[ 1024 ];

				_stprintf_s( l_szMsg, 1024,
					_T("This version of SVObserver will not run with the installed MIL library. ")
					_T("SVObserver needs MIL %4.2f. The installed version is %4.2f"),
					SV_CURRENT_MIL_VERSION, 
					l_MilVersion );

				::MessageBox( NULL, l_szMsg, _T("SVObserver"), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );

				::exit( EXIT_FAILURE );
			}
		}
		else
		{
			long l_Status = MappGetError( M_THREAD_CURRENT + M_CURRENT, NULL );

			#ifdef _DEBUG
			if( l_Status != M_NULL_ERROR )
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				TCHAR buf[M_ERROR_MESSAGE_SIZE * 2];
				_stprintf_s( buf, M_ERROR_MESSAGE_SIZE * 2, _T("%d - %s\n"), l_info.m_FunctionCode, l_info.m_FunctionString.c_str() );
				::OutputDebugString(buf );
			}
			#endif
		}
	}
}

/**
@SVOperationName Local Clear

@SVOperationDescription Frees the application

*/
void SVMatroxApplicationInterface::LocalClear()
{
	long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);
	if( appID != M_NULL )
	{
		// Disable MIL error message to be displayed by MIL
		MappControl( M_ERROR, M_PRINT_DISABLE );

		// Unhook Error handler
		MappHookFunction( M_ERROR_CURRENT + M_UNHOOK, SVMatroxHookHandler, NULL );
		
		MappFree( appID );
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxApplicationInterface.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Sep 2011 14:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 Jul 2010 08:12:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status information sent to event log to be more specific on error information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Mar 2010 11:07:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated project to remove MFC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 11:32:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to allow for new OCR training functionality and fixed issues with unicode compiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 09:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2009 15:48:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSytemCount method
 * Added GetSystemName method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Nov 2008 14:57:48   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Increased error handling in
 * 
 * SVMatroxHookHandler ()
 * SVMatroxApplicationInterface::Log ()
 * SVMatroxApplicationInterface::GetLastStatus ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
