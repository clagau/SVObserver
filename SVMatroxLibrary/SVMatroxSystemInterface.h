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

#ifndef SVMATROXSYSTEMINTERFACE_H
#define SVMATROXSYSTEMINTERFACE_H

#include "SVMatroxStatusInformation.h"
#include "SVMatroxSystem.h"
#include "SVMatroxSystemEnums.h"

class SVMatroxSystemInterface
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;

	static SVStatusCode Allocate(const SVMatroxString& SystemDescriptor, SVMatroxInt SystemNum, SVMatroxSystem& system);
	static SVStatusCode Release(SVMatroxSystem& SystemId);

	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& value);
	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, double& value);
	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, SVMatroxString& value);

	static SVStatusCode Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue);

	static SVStatusCode Destroy(SVMatroxSystem& p_rSystem);

	static SVStatusCode SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);
	static SVStatusCode ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);

	static SVStatusCode GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value);
	static SVStatusCode GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value);
};

#endif

