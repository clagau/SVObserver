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
//Moved to precompiled header: #include <memory>
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

	const std::string& GetJsonCommand() const { return m_JsonCommand; };
	// Note this returns the std::string object to write to the value
	std::string& GetJsonResultsObject() { return m_JsonResults; };
	HRESULT SetJsonCommand( const std::string& rCommand );

private:
	HRESULT GetWaitHandle( HANDLE& rHandle ) const;

	std::string m_JsonCommand;
	std::string m_JsonResults;

	HANDLE m_WaitHandle;

};

typedef std::shared_ptr< SVJsonCommandData > SVJsonCommandDataPtr;

