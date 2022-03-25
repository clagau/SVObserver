//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncCommandHelper
//* .File Name       : $Workfile:   SVAsyncCommandHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:35:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandStatus.h"
#include "SVCommandTemplate.h"

#pragma endregion Includes

struct SVAsyncCommandHelper
{
	SVAsyncCommandHelper();
	SVAsyncCommandHelper( const SVAsyncCommandHelper& p_rObject );

	virtual ~SVAsyncCommandHelper();

	bool empty() const;

	bool GetCancel() const;
	void SetCancel( bool p_Cancel );

	void SetCommandActive();
	void SetCommandComplete();

	HRESULT NotifyRequestComplete() const;
	HRESULT WaitForRequest( DWORD p_TimeoutInMilliseconds );

	bool IsCommandEmpty() const;
	HRESULT GetCommand( SVCommandTemplatePtr& p_rCommand, DWORD p_TimeoutInMilliseconds ) const;
	HRESULT SetCommand( const SVCommandTemplatePtr& p_rCommand, DWORD p_TimeoutInMilliseconds );
	HRESULT ClearCommand( DWORD p_TimeoutInMilliseconds = INFINITE );

	SVCommandStatus GetStatus() const;
	void SetStatus( const SVCommandStatus& p_rStatus );

private:
	HRESULT CancelCommand( DWORD p_TimeoutInMilliseconds = INFINITE );
	HRESULT GetWaitHandle( HANDLE& p_rHandle ) const;

	mutable HANDLE m_WaitHandle;
	
	mutable std::timed_mutex m_TimedMutex;
	SVCommandTemplatePtr m_CommandPtr;

	bool m_Cancel;
	mutable SVCommandStatus m_Status;

	long m_CommandActive;

};
