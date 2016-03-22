//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputStreamManager
//* .File Name       : $Workfile:   SVOutputStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:16:48  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <string>

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVUtilityLibrary/SVGUID.h"

class SVOutputStreamManager
{
public:
	friend class SVObserverApp;

	static SVOutputStreamManager& Instance();

	virtual ~SVOutputStreamManager();

	HRESULT InsertOutputController( const SVGUID& rObjectId );
	HRESULT EraseOutputController();

	HRESULT InsertOutputStream( const std::string& rName, const SVGUID& rObjectId );
	HRESULT EraseOutputStream();

	HRESULT ProcessJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );
	HRESULT Rename( LPCTSTR OldName, LPCTSTR NewName);

protected:
	typedef std::pair< std::string, SVGUID > SVOutputStreamPair;

	class SVOutputSocketObserver : 
		public SVObserverNotificationFunctor,
		public SVObserverTemplate< std::string >
	{
	public:
		typedef boost::function< HRESULT ( const std::string& ) > SVObserverFunction;
		SVOutputSocketObserver( SVObserverFunction Function ) : m_Function( Function ) {}

		virtual HRESULT ObserverUpdate( const std::string& rData ) override
		{
			HRESULT l_Status = S_OK;

			if( !( m_Function.empty() ) )
			{
				l_Status = m_Function( rData );
			}
			else
			{
				l_Status = E_FAIL;
			}
			return l_Status;
		}

	private:
		SVOutputSocketObserver() : m_Function() {}
		SVObserverFunction m_Function;
	};

	SVOutputStreamManager();

	void Startup(unsigned short PortNumber);
	void Shutdown();

	HRESULT SendCommandToOutputStream( const std::string& rCmdName, const std::string& rName, const std::string& rJsonCommand, std::string& rJsonResults );

	HRESULT ProcessStreamManagerJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );

	SVJsonCommandServer m_SocketServer;
	DWORD m_OutputSocketCookie;
	SVObserverNotificationFunctorPtr m_SocketNotifyPtr;
	SVGUID m_OutputControllerId;
	SVOutputStreamPair m_OutputStream;

};

