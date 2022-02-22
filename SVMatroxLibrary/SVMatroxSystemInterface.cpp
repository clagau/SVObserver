//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemInterface
//* .File Name       : $Workfile:   SVMatroxSystemInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   29 Jan 2014 10:23:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxSystemInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxEnumConvertor.h"
#include "SVMatroxSystem.h"
#include "SVMatroxResourceMonitor.h"
#pragma endregion Includes

/**
@SVOperationName Allocate

@SVOperationDescription This method allocates the handle for the Matrox System.

*/

HRESULT SVMatroxSystemInterface::Allocate(const std::string& SystemDescriptor, long long SystemNum, SVMatroxSystem& p_rSystem)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		

	
			__int64 systemID;

			size_t len = SystemDescriptor.length() + 1;
			char* tmp = new char[len];
			strcpy_s(tmp, len, SystemDescriptor.c_str());

			// how to handle multiple boards?
			// if that is an issue, need to verify with multiple cards...
			MsysAlloc( tmp, M_DEV0 + SystemNum, M_COMPLETE, &systemID);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			delete [] tmp;

			if( l_Code == S_OK )
			{
				p_rSystem.m_SystemIdentifier = systemID;
				SVMatroxResourceMonitor::InsertIdentifier( SVSystemID, p_rSystem.m_SystemIdentifier );
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

HRESULT SVMatroxSystemInterface::Release(SVMatroxSystem& SystemId)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (SystemId.m_SystemIdentifier != 0)
		{
			

			
				MsysFree( SystemId.m_SystemIdentifier );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				SVMatroxResourceMonitor::EraseIdentifier( SVSystemID, SystemId.m_SystemIdentifier );
			
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

///**
//@SVOperationName Get
//
//@SVOperationDescription This method gets the Matrox System Control values.
//
//*/
//
HRESULT SVMatroxSystemInterface::Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& InquireValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long long l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemInquire::m_convertor, InquireType, l_MatroxType );
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

HRESULT SVMatroxSystemInterface::Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		__int64 l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemControl::m_convertor, ControlType, l_MatroxType);
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

HRESULT SVMatroxSystemInterface::Destroy(SVMatroxSystem& p_rSystem)
{
	HRESULT l_Code( SVMatroxSystemInterface::Release(p_rSystem) );
	
	// Clear the handle ?
	p_rSystem.m_SystemIdentifier = 0;

	return l_Code;
}

/**
@SVOperationName SetHookFunction

@SVOperationDescription This method set the hook function for the Matrox System.

*/

HRESULT SVMatroxSystemInterface::SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr)
{
	HRESULT l_Code( S_OK );

	// Convert Enum
	long long l_MatroxType = 0;
	HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemHook::m_convertor, HookType, l_MatroxType );
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
HRESULT SVMatroxSystemInterface::ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr)
{
	HRESULT l_Code( S_OK );

	// Convert Enum
	long long l_MatroxType = 0;
	HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemHook::m_convertor, HookType, l_MatroxType );
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
HRESULT SVMatroxSystemInterface::GetHookInfo(const SVMatroxSystem& SystemId, __int64 p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value)
{
	HRESULT l_Code( S_OK );
	
	// Convert Enum
	long long l_MatroxType = 0;
	HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemHookInfo::m_convertor, HookInfoType, l_MatroxType);
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
HRESULT SVMatroxSystemInterface::GetHookInfo(const SVMatroxSystem& SystemId, __int64 p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value)
{
	HRESULT l_Code( S_OK );

	// Convert Enum
	long long l_MatroxType = 0;
	HRESULT hr = ConvertEnumToMatroxType(SVMatroxSystemHookInfo::m_convertor, HookInfoType, l_MatroxType);
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

/*static*/ HRESULT SVMatroxSystemInterface::getNonPagedMemory( long long& rMemorySize, long long& rMemoryUsed )
{
	MIL_INT MilMemSize;
	MIL_INT MilMemUsed;

	MappInquire( M_NON_PAGED_MEMORY_SIZE, &MilMemSize );
	MappInquire( M_NON_PAGED_MEMORY_USED, &MilMemUsed );

	rMemorySize = MilMemSize;
	rMemoryUsed = MilMemUsed;

	return S_OK;
}