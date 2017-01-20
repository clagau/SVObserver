// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxApplicationInterface
// * .File Name       : $Workfile:   SVMatroxApplicationInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:08:08  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxEnums.h"
#include "SVMatroxImagingLibrary.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

static const long MATROX_FILTER_EVENT = 2622995;
static const long MATROX_FILTER_EVENT_STATUS_CODE = 4455;
static const long MATROX_FILTER_EVENT_SUBCODE_COUNT = 3;

SVMatroxInt _stdcall SVMatroxApplicationInterface::SVMatroxHookHandler( SVMatroxInt HookType, SVMatroxIdentifier EventId, void* UserDataPtr )
{
	MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];
	SVMatroxStatusInformation l_StatusInfo;

	SVMatroxInt l_TempCode = 0;
	MappGetHookInfo( EventId, M_CURRENT, &l_TempCode );
	l_StatusInfo.m_StatusCode = SVMatroxIntToHRESULT( l_TempCode );
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
			l_TempCode = 0;
			MappGetHookInfo( EventId, M_CURRENT_SUB_3, &l_TempCode );
			l_StatusInfo.m_StatusSubCode[ 2 ] = SVMatroxIntToHRESULT( l_TempCode );
			MappGetHookInfo( EventId, M_CURRENT_SUB_3 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 2 ] = l_strMilText;
			// no break
		}
		case 2:
		{
			l_TempCode = 0;
			MappGetHookInfo( EventId, M_CURRENT_SUB_2, &l_TempCode );
			l_StatusInfo.m_StatusSubCode[ 1 ] = SVMatroxIntToHRESULT( l_TempCode );
			MappGetHookInfo( EventId, M_CURRENT_SUB_2 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 1 ] = l_strMilText;
			// no break
		}
		case 1:
		{
			l_TempCode = 0;
			MappGetHookInfo( EventId, M_CURRENT_SUB_1, &l_TempCode );
			l_StatusInfo.m_StatusSubCode[ 0 ] = SVMatroxIntToHRESULT( l_TempCode );
			MappGetHookInfo( EventId, M_CURRENT_SUB_1 + M_MESSAGE, l_strMilText );
			l_StatusInfo.m_StatusSubString[ 0 ] = l_strMilText;
			// no break
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
	else if ((l_StatusInfo.m_FunctionCode == MATROX_FILTER_EVENT) &&
			 (l_StatusInfo.m_StatusCode  == MATROX_FILTER_EVENT_STATUS_CODE) &&
			 (l_StatusInfo.m_StatusSubCodeCount == MATROX_FILTER_EVENT_SUBCODE_COUNT) &&	
			 (l_StatusInfo.m_StatusSubCode[0] == S_FALSE))	
	{
//-		RPY240215 - This case is put here to filter out
//-		the EventGigEVisionError.  Most of the GigE Cameras 
//-		do not support Events and should not show up in the event
//-		log as an error
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
		DWORD OsError = static_cast<DWORD> ( p_rStatusInfo.m_StatusCode );
		DWORD MessageCode( 0 );
		DWORD ProgramCode( 0 );
		if( SUCCEEDED( p_rStatusInfo.m_StatusCode ) ) 
		{
			MessageCode = SVMSG_SVMATROXLIBRARY_NO_ERROR;
			ProgramCode = SvOi::Err_25020_MatroxLibraryNone;
		}
		else
		{
			MessageCode = SVMSG_SVMATROXLIBRARY_UNKNOWN_FATAL_ERROR;
			ProgramCode = SvOi::Err_25021_MatroxLibraryFatal;
		}
		SVStringVector msgList;
		msgList.push_back( SvUl_SF::Format( _T("%d"), OsError ));
		msgList.push_back( SvUl_SF::Format( _T("0X%08X"), OsError ));
		msgList.push_back( p_rStatusInfo.GetCompleteString() );
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( MessageCode, SvOi::Tid_OS_Error_Message, msgList, SvStl::SourceFileParams(StdMessageParams), ProgramCode);
	}
}

void SVMatroxApplicationInterface::LogMatroxException( )
{
	SVMatroxStatusInformation l_info;
	l_info.m_StatusCode = SVMEE_MATROX_THREW_EXCEPTION;
	l_info.m_StatusString = _T("Matrox Threw an Exception");
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
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );

	if( appID != M_NULL )
	{
		l_Status = SVMatroxIntToHRESULT( MappGetError( M_THREAD_CURRENT + M_CURRENT, nullptr ) );

		#ifdef _DEBUG
		if( l_Status != M_NULL_ERROR )
		{
			SVMatroxStatusInformation l_info;
			SVMatroxApplicationInterface::GetLastStatus( l_info );
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			TCHAR buf[M_ERROR_MESSAGE_SIZE * 2];
			_stprintf_s( buf, M_ERROR_MESSAGE_SIZE * 2, _T("%d - %s\n"), l_info.m_FunctionCode, l_info.m_FunctionString.c_str() );
			::OutputDebugString(buf );
#endif
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
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );

	if( appID != M_NULL )
	{
		MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];

		p_rStatus.clear();

		SVMatroxInt l_TempStatus = 0;
		l_Status = SVMatroxIntToHRESULT( MappGetError( M_THREAD_CURRENT + M_CURRENT, &l_TempStatus ) );
		p_rStatus.m_StatusCode = SVMatroxIntToHRESULT( l_TempStatus );
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
				SVMatroxInt l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_3, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 2 ] = SVMatroxIntToHRESULT( l_TempCode );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_3 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 2 ] = l_strMilText;
				// no break;
			}
			case 2:
			{
				SVMatroxInt l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_2, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 1 ] = SVMatroxIntToHRESULT( l_TempCode );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_2 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 1 ] = l_strMilText;
				// no break
			}
			case 1:
			{
				SVMatroxInt l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_1, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 0 ] = SVMatroxIntToHRESULT( l_TempCode );
				MappGetError( M_THREAD_CURRENT + M_CURRENT_SUB_1 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 0 ] = l_strMilText;
				// no break
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
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );

	if( appID != M_NULL )
	{
		l_Code = SVMatroxIntToHRESULT( MappGetError( M_THREAD_CURRENT + M_GLOBAL, nullptr ) );
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
	SVMatroxInt appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);

	if( appID != M_NULL )
	{
		MIL_TEXT_CHAR l_strMilText[M_ERROR_MESSAGE_SIZE];

		p_rStatus.clear();

		SVMatroxIdentifier l_TempStatus = 0;
		SVMatroxIdentifier l_Error = MappGetError( M_THREAD_CURRENT + M_GLOBAL, &l_TempStatus );
		p_rStatus.m_StatusCode = SVMatroxIntToHRESULT( l_TempStatus );
		l_Status = SVMatroxIntToHRESULT( l_Error );
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
				SVMatroxIdentifier l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_3, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 2 ] = SVMatroxIntToHRESULT( l_TempCode );
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_3 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 2 ] = l_strMilText;
			}
			case 2:
			{
				SVMatroxIdentifier l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_2, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 1 ] = SVMatroxIntToHRESULT( l_TempCode );
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_2 + M_MESSAGE, l_strMilText );
				p_rStatus.m_StatusSubString[ 1 ] = l_strMilText;
			}
			case 1:
			{
				SVMatroxIdentifier l_TempCode = 0;
				MappGetError( M_THREAD_CURRENT + M_GLOBAL_SUB_1, &l_TempCode );
				p_rStatus.m_StatusSubCode[ 0 ] = SVMatroxIntToHRESULT( l_TempCode );
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

	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );
	if( appID != M_NULL )
	{
		#ifdef USE_TRY_BLOCKS
		try
		#endif
		{
			p_lCount = static_cast< long >( MappInquire( M_INSTALLED_SYSTEM_COUNT, M_NULL ) );
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

SVMatroxApplicationInterface::SVStatusCode SVMatroxApplicationInterface::GetSystemName( SVMatroxInt p_lSystemNumber, SVString& p_rSystemName )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );

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
	//long appID = MappInquire(M_CURRENT_APPLICATION, M_NULL);
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, 0 );
	if( appID == M_NULL )
	{
		appID = MappAlloc( M_DEFAULT, M_NULL );

		if( appID != M_NULL )
		{
			double l_MilVersion( 0.0 );

			SVMatroxHookFunctionPtr l_pHandlerFunction = nullptr;
			void* l_pHandlerUserData = nullptr;

			// Disable MIL error message to be displayed by MIL
			MappControl( M_ERROR, M_PRINT_DISABLE );

			// Retrieve previous handler ptr and user handler ptr
			MappInquire( M_CURRENT_ERROR_HANDLER_PTR, &l_pHandlerFunction );
			MappInquire( M_CURRENT_ERROR_HANDLER_USER_PTR, &l_pHandlerUserData );

			// disable previous
			MappHookFunction( M_ERROR_CURRENT + M_UNHOOK, l_pHandlerFunction, l_pHandlerUserData );

			// Hook into MIL error handling
			MappHookFunction( M_ERROR_CURRENT, SVMatroxHookHandler, nullptr );

			// check version of MIL
			MappInquire( M_VERSION, &l_MilVersion );

			if ( SV_CURRENT_MIL_VERSION != l_MilVersion )
			{
				SVStringVector msgList;
				msgList.push_back(SvUl_SF::Format(_T("%4.2f"), SV_CURRENT_MIL_VERSION));
				msgList.push_back(SvUl_SF::Format(_T("%4.2f"), l_MilVersion));
				
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_ERROR, SvOi::Tid_MilVersion_Error, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10249 );
				::exit( EXIT_FAILURE );
			}
		}
		else
		{
			SVMatroxIdentifier l_Status = MappGetError( M_THREAD_CURRENT + M_CURRENT, nullptr );

			#ifdef _DEBUG
			if( l_Status != M_NULL_ERROR )
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TCHAR buf[M_ERROR_MESSAGE_SIZE * 2];
				_stprintf_s( buf, M_ERROR_MESSAGE_SIZE * 2, _T("%d - %s\n"), l_info.m_FunctionCode, l_info.m_FunctionString.c_str() );
				::OutputDebugString(buf );
#endif
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
	SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, M_NULL );
	if( appID != M_NULL )
	{
		// Disable MIL error message to be displayed by MIL
		MappControl( M_ERROR, M_PRINT_DISABLE );

		// Unhook Error handler
		MappHookFunction( M_ERROR_CURRENT + M_UNHOOK, SVMatroxHookHandler, nullptr );
		
		MappFree( appID );
	}
}

HRESULT SVMatroxApplicationInterface::SVMatroxIntToHRESULT( SVMatroxIdentifier p_Int )
{
	HRESULT l_Retval = E_UNEXPECTED;

	if ( ( p_Int & 0x1111111100000000 ) == 0 )
	{
		l_Retval = static_cast< HRESULT >( p_Int );
	}

	return l_Retval;
}

