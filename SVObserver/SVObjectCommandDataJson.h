//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandDataJson
//* .File Name       : $Workfile:   SVObjectCommandDataJson.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:26  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <string>
#include "SVUtilityLibrary/SVSharedPtr.h"

struct SVObjectCommandDataJson
{
	SVObjectCommandDataJson();
	SVObjectCommandDataJson( const SVObjectCommandDataJson& p_rObject );

	virtual ~SVObjectCommandDataJson();

	bool empty() const;
	void clear();

	HRESULT NotifyRequestComplete() const;
	HRESULT WaitForRequest( DWORD p_TimeoutInMilliseconds ) const;

	const std::string& GetJsonCommand() const;
	HRESULT SetJsonCommand( const std::string& p_rCommand );

	std::string m_JsonResults;

private:
	HRESULT GetWaitHandle( HANDLE& p_rHandle ) const;

	std::string m_JsonCommand;

	HANDLE m_WaitHandle;

};

typedef SVSharedPtr< SVObjectCommandDataJson > SVObjectCommandDataJsonPtr;
