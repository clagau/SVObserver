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

#include "SVUtilityLibrary/SVSharedPtr.h"

template< typename SVCommandPtr >
class SVObjectCommandWrapperTemplate
{
public:
	typedef SVSharedPtr< SVObjectCommandWrapperTemplate > SVObjectCommandWrapperPtr;

	SVObjectCommandWrapperTemplate( const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectCommandWrapperTemplate();

	HRESULT Execute();

	HRESULT NotifyRequestComplete() const;
	HRESULT IsRequestComplete() const;
	HRESULT WaitForRequest(DWORD p_TimeoutInMilliseconds) const;

	HRESULT GetStatus() const;

private:
	SVCommandPtr m_CommandPtr;

	HANDLE m_WaitHandle;
	HRESULT m_Status;

};

#include "SVObjectCommandWrapperTemplate.inl"

