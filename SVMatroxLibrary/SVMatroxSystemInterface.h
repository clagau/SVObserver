//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemInterface
//* .File Name       : $Workfile:   SVMatroxSystemInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxStatusInformation.h"
#include "SVMatroxSystem.h"
#include "SVMatroxSystemEnums.h"
#pragma endregion Includes

class SVMatroxSystemInterface
{
public:

	static HRESULT Allocate(const SVString& SystemDescriptor, SVMatroxInt SystemNum, SVMatroxSystem& system);
	static HRESULT Release(SVMatroxSystem& SystemId);

	static HRESULT Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& value);
	static HRESULT Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, double& value);
	static HRESULT Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, SVString& value);

	static HRESULT Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue);

	static HRESULT Destroy(SVMatroxSystem& p_rSystem);

	static HRESULT SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);
	static HRESULT ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);

	static HRESULT GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value);
	static HRESULT GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value);

	static HRESULT getNonPagedMemory( long long& rMemorySize, long long& rMemoryUsed );
};

