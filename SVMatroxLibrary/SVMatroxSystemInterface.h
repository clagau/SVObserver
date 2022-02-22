//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxSystemEnums.h"
#pragma endregion Includes

class SVMatroxSystem;

class SVMatroxSystemInterface
{
public:
	typedef __int64(_stdcall *SVMatroxHookFunctionPtr)(__int64, __int64, void*);

	static HRESULT Allocate(const std::string& SystemDescriptor, long long SystemNum, SVMatroxSystem& system);
	static HRESULT Release(SVMatroxSystem& SystemId);

	static HRESULT Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& value);

	static HRESULT Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue);

	static HRESULT Destroy(SVMatroxSystem& p_rSystem);

	static HRESULT SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);
	static HRESULT ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);

	static HRESULT GetHookInfo(const SVMatroxSystem& SystemId, __int64 p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value);
	static HRESULT GetHookInfo(const SVMatroxSystem& SystemId, __int64 p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value);

	static HRESULT getNonPagedMemory( long long& rMemorySize, long long& rMemoryUsed );
};

