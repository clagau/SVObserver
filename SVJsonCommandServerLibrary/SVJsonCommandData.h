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

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#pragma endregion Includes

struct SVJsonCommandData
{
	SVJsonCommandData();
	SVJsonCommandData( const SVJsonCommandData& rObject );

	virtual ~SVJsonCommandData();

	bool empty() const;
	void clear();

	HRESULT NotifyRequestComplete() const;
	HRESULT WaitForRequest( DWORD TimeoutInMilliseconds ) const;

	const SVString& GetJsonCommand() const { return m_JsonCommand; };
	// Note this returns the SVString object to write to the value
	SVString& GetJsonResultsObject() { return m_JsonResults; };
	HRESULT SetJsonCommand( const SVString& rCommand );

private:
	HRESULT GetWaitHandle( HANDLE& rHandle ) const;

	SVString m_JsonCommand;
	SVString m_JsonResults;

	HANDLE m_WaitHandle;

};

typedef SVSharedPtr< SVJsonCommandData > SVJsonCommandDataPtr;

