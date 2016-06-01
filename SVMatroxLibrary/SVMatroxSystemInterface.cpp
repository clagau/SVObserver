//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemInterface
//* .File Name       : $Workfile:   SVMatroxSystemInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   29 Jan 2014 10:23:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImagingLibrary.h"
#include "SVMatroxSystemInterface.h"
#include "SVMatroxEnums.h"
#include "SVMatroxSystem.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxResourceMonitor.h"

/**
@SVOperationName Allocate

@SVOperationDescription This method allocates the handle for the Matrox System.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Allocate(const SVString& SystemDescriptor, SVMatroxInt SystemNum, SVMatroxSystem& p_rSystem)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			SVMatroxIdentifier systemID;

			size_t len = SystemDescriptor.length() + 1;
			char* tmp = new char[len];
			strcpy_s(tmp, len, SystemDescriptor.c_str());

			// how to handle multiple boards?
			// if that is an issue, need to verify with multiple cards...
			MsysAlloc( tmp, M_DEV0 + SystemNum, M_COMPLETE, &systemID);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			delete [] tmp;

			if( l_Code == SVMEE_STATUS_OK )
			{
				p_rSystem.m_SystemIdentifier = systemID;
				SVMatroxResourceMonitor::InsertIdentifier( SVSystemID, p_rSystem.m_SystemIdentifier );
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Release

@SVOperationDescription This method releases the handle allocated in the Allocate method.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Release(SVMatroxSystem& SystemId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (SystemId.m_SystemIdentifier != 0)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MsysFree( SystemId.m_SystemIdentifier );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				SVMatroxResourceMonitor::EraseIdentifier( SVSystemID, SystemId.m_SystemIdentifier );
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the Matrox System Control values.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, double& value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT mValue;
		MsysInquire( SystemId.m_SystemIdentifier, InquireType, &mValue);
		value = static_cast<double>(mValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the Matrox System Control values.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, SVString& value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_TEXT_CHAR l_String[256];
		MsysInquire( SystemId.m_SystemIdentifier, InquireType, reinterpret_cast<void *>(l_String));
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			value = l_String;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the Matrox System Control values.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& InquireValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_MatroxType = 0;
		HRESULT hr = SVMatroxSystemInquire::m_convertor.ConvertEnumToMatroxType( InquireType, l_MatroxType );
		if (S_OK == hr)
		{
			MIL_INT l_Value = 0;
			MsysInquire( SystemId.m_SystemIdentifier, l_MatroxType, &l_Value );
			InquireValue = static_cast<long>( l_Value );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Set

@SVOperationDescription This method sets the Matrox System Control values.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxSystemControl::m_convertor.ConvertEnumToMatroxType(ControlType, l_MatroxType);
		if (S_OK == hr)
		{
			MsysControl( SystemId.m_SystemIdentifier, l_MatroxType, ControlValue );
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Destroy

@SVOperationDescription This method destroys the handle for the Matrox System.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::Destroy(SVMatroxSystem& p_rSystem)
{
	SVStatusCode l_Code( SVMatroxSystemInterface::Release(p_rSystem) );
	
	// Clear the handle ?
	p_rSystem.m_SystemIdentifier = 0;

	return l_Code;
}

/**
@SVOperationName SetHookFunction

@SVOperationDescription This method set the hook function for the Matrox System.

*/

SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

	// Convert Enum
	SVMatroxInt l_MatroxType = 0;
	HRESULT hr = SVMatroxSystemHook::m_convertor.ConvertEnumToMatroxType( HookType, l_MatroxType );
	if (S_OK == hr)
	{
		MsysHookFunction( SystemId.m_SystemIdentifier, l_MatroxType, HookHandlerPtr, UserDataPtr );
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
	else
	{
		// conversion error
		l_Code = hr;
	}
	return l_Code;
}

/**
@SVOperationName ReleaseHookFunction

@SVOperationDescription This method unsets the hook function for the Matrox System.

*/
SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

	// Convert Enum
	SVMatroxInt l_MatroxType = 0;
	HRESULT hr = SVMatroxSystemHook::m_convertor.ConvertEnumToMatroxType( HookType, l_MatroxType );
	if (S_OK == hr)
	{	
		MsysHookFunction( SystemId.m_SystemIdentifier, l_MatroxType | M_UNHOOK, HookHandlerPtr, UserDataPtr);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
	else
	{
		// conversion error
		l_Code = hr;
	}
	return l_Code;
}

/**
@SVOperationName GetHookInfo

@SVOperationDescription This method gets the hook info for the Matrox System.

*/
SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	
	// Convert Enum
	SVMatroxInt l_MatroxType = 0;
	HRESULT hr = SVMatroxSystemHookInfo::m_convertor.ConvertEnumToMatroxType(HookInfoType, l_MatroxType);
	if (S_OK == hr)
	{
		MIL_INT mValue;
		MsysGetHookInfo(SystemId.m_SystemIdentifier, p_EventId, l_MatroxType, &mValue);
		value = static_cast<long>(mValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
	else
	{
		// conversion error
		l_Code = hr;
	}
	return l_Code;
}

/**
@SVOperationName GetHookInfo

@SVOperationDescription This method gets the hook info for the Matrox System.

*/
SVMatroxSystemInterface::SVStatusCode SVMatroxSystemInterface::GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

	// Convert Enum
	SVMatroxInt l_MatroxType = 0;
	HRESULT hr = SVMatroxSystemHookInfo::m_convertor.ConvertEnumToMatroxType(HookInfoType, l_MatroxType);
	if (S_OK == hr)
	{
		MsysGetHookInfo(SystemId.m_SystemIdentifier, p_EventId, l_MatroxType, &value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
	else
	{
		// conversion error
		l_Code = hr;
	}
	return l_Code;
}

