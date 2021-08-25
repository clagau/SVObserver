// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTriggerLoadLibraryClass
// * .File Name       : $Workfile:   SVIOTriggerLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:48:18  $
// ******************************************************************************

#pragma once

#include "SVTriggerInfoStruct.h"

typedef HRESULT (WINAPI *SVCreatePtr)();
typedef HRESULT (WINAPI *SVDestroyPtr)();
typedef unsigned long (WINAPI *SVTriggerGetCountPtr)();
typedef unsigned long (WINAPI *SVTriggerGetHandlePtr)(unsigned long);
typedef _variant_t (WINAPI *SVTriggerGetNamePtr)(unsigned long);
typedef HRESULT (WINAPI *SVTriggerRegisterPtr)(unsigned long, SvTrig::TriggerCallBack pTriggerCallback);
typedef HRESULT (WINAPI *SVTriggerUnregisterPtr)(unsigned long);
typedef HRESULT (WINAPI *SVTriggerStartPtr)(unsigned long);
typedef HRESULT (WINAPI *SVTriggerStopPtr)(unsigned long);
typedef unsigned long (WINAPI *SVTriggerGetParameterCountPtr)(unsigned long);
typedef _variant_t(WINAPI *SVTriggerGetParameterNamePtr)(unsigned long, unsigned long);
typedef _variant_t (WINAPI *SVTriggerGetParameterValuePtr)(unsigned long, unsigned long);
typedef HRESULT (WINAPI *SVTriggerSetParameterValuePtr)(unsigned long, unsigned long, const _variant_t&);

namespace SvTrig
{
class SVIOTriggerLoadLibraryClass
{
public:
	SVIOTriggerLoadLibraryClass() = default;
	~SVIOTriggerLoadLibraryClass();

	HRESULT Open(LPCTSTR libraryPath);
	HRESULT Close();

	unsigned long GetCount() const;
	unsigned long GetHandle(unsigned long index) const;
	_variant_t GetName(unsigned long triggerIndex) const;
	HRESULT Register(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback);
	HRESULT Unregister(unsigned long triggerIndex);
	HRESULT Start(unsigned long triggerIndex);
	HRESULT Stop(unsigned long triggerIndex);
	unsigned long GetParameterCount(unsigned long triggerIndex) const;
	_variant_t GetParameterName(unsigned long triggerIndex, unsigned long index) const;
	_variant_t GetParameterValue(unsigned long triggerIndex, unsigned long index) const;
	HRESULT SetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue);

private:
	HMODULE m_Handle {nullptr};

	SVCreatePtr m_pCreate {nullptr};
	SVDestroyPtr m_pDestroy {nullptr};
	SVTriggerGetCountPtr m_pGetCount {nullptr};
	SVTriggerGetHandlePtr m_pGetHandle {nullptr};
	SVTriggerGetNamePtr m_pGetName {nullptr};
	SVTriggerRegisterPtr m_pRegister {nullptr};
	SVTriggerUnregisterPtr m_pUnregister {nullptr};
	SVTriggerStartPtr m_pStart {nullptr};
	SVTriggerStopPtr m_pStop {nullptr};
	SVTriggerGetParameterCountPtr m_pGetParameterCount {nullptr};
	SVTriggerGetParameterNamePtr m_pGetParameterName {nullptr};
	SVTriggerGetParameterValuePtr m_pGetParameterValue {nullptr};
	SVTriggerSetParameterValuePtr m_pSetParameterValue {nullptr};
};
}