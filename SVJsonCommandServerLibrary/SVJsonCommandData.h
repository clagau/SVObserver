//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandData
//* .File Name       : $Workfile:   SVJsonCommandData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:14:26  $
//******************************************************************************

#pragma once

#include <string>
#include "SVUtilityLibrary/SVSharedPtr.h"

struct SVJsonCommandData
{
	SVJsonCommandData();
	SVJsonCommandData( const SVJsonCommandData& p_rObject );

	virtual ~SVJsonCommandData();

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

typedef SVSharedPtr< SVJsonCommandData > SVJsonCommandDataPtr;

