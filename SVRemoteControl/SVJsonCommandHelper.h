//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandHelper
//* .File Name       : $Workfile:   SVJsonCommandHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Oct 2014 10:10:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#include "JsonLib/include/json.h"
#include "SVCriticalSection.h"
#pragma endregion Includes

struct SVJsonCommandHelper
{
	SVJsonCommandHelper();
	SVJsonCommandHelper( const std::string& p_rJsonCommand );
	SVJsonCommandHelper( const SVJsonCommandHelper& p_rObject );

	virtual ~SVJsonCommandHelper();

	bool empty() const;
	void clear();

	HRESULT NotifyRequestComplete() const;
	HRESULT WaitForRequest( DWORD p_TimeoutInMilliseconds ) const;

	bool IsJsonCommandEmpty() const;
	HRESULT GetJsonCommand( std::string& p_rJsonCommand, DWORD p_TimeoutInMilliseconds ) const;
	HRESULT SetJsonCommand( const std::string& p_rJsonCommand, DWORD p_TimeoutInMilliseconds );
	HRESULT ClearJsonCommand( DWORD p_TimeoutInMilliseconds = INFINITE );

	const Json::Value& GetJsonResults() const;
	HRESULT SetJsonResults( const Json::Value& p_rJsonResults );
	HRESULT ExtractStatus(const std::string& tag) const;

private:
	HRESULT GetWaitHandle( HANDLE& p_rHandle ) const;

	mutable HANDLE m_WaitHandle;

	mutable SVCriticalSection m_CriticalSection;
	std::string m_JsonCommand;

	Json::Value m_JsonResults;
};
