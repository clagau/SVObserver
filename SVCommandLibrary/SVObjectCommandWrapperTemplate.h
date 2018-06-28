//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandWrapperTemplate
//* .File Name       : $Workfile:   SVObjectCommandWrapperTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:47:14  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

template<typename CommandPtr>
class SVObjectCommandWrapperTemplate
{
public:
	typedef std::shared_ptr<SVObjectCommandWrapperTemplate> SVObjectCommandWrapperPtr;

	SVObjectCommandWrapperTemplate(const CommandPtr& rpCommand);

	virtual ~SVObjectCommandWrapperTemplate();

	HRESULT Execute();

	HRESULT NotifyRequestComplete() const;
	HRESULT IsRequestComplete() const;
	HRESULT WaitForRequest(DWORD p_TimeoutInMilliseconds) const;

	HRESULT GetStatus() const;

private:
	CommandPtr m_pCommand;

	HANDLE m_WaitHandle;
	HRESULT m_Status;

};

#include "SVObjectCommandWrapperTemplate.inl"

