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

#pragma region Includes
//Moved to precompiled header: #include <string>

#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

class SVOutputStreamManager
{
public:
	static SVOutputStreamManager& Instance();

	virtual ~SVOutputStreamManager();

	HRESULT InsertOutputController( const SVGUID& rObjectId );
	HRESULT EraseOutputController();

	HRESULT InsertOutputStream( const std::string& rName, const SVGUID& rObjectId );
	HRESULT EraseOutputStream();

	HRESULT ProcessJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );
	HRESULT Rename( LPCTSTR OldName, LPCTSTR NewName);

	// These two (2) methods, Startup, Shutdown are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, as described via this comment
	void Startup(unsigned short PortNumber); // This method is only meant to be called by the main application class
	void Shutdown();						 // This method is only meant to be called by the main application class

protected:
	typedef std::pair< std::string, SVGUID > SVOutputStreamPair;

	class SVOutputSocketObserver : 
		public SVObserverNotificationFunctor,
		public SVObserverTemplate< std::string >
	{
	public:
		typedef boost::function< HRESULT ( const std::string& ) > SVObserverFunction;
		SVOutputSocketObserver( SVObserverFunction Function ) : m_Function( Function ) {}
		virtual ~SVOutputSocketObserver() {}

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

	HRESULT SendCommandToOutputStream( const std::string& rCmdName, const std::string& rName, const std::string& rJsonCommand, std::string& rJsonResults );

	HRESULT ProcessStreamManagerJsonCommand( const std::string& rJsonCommand, std::string& rJsonResults );

	SVJsonCommandServer m_SocketServer;
	DWORD m_OutputSocketCookie;
	SVObserverNotificationFunctorPtr m_SocketNotifyPtr;
	SVGUID m_OutputControllerId;
	SVOutputStreamPair m_OutputStream;
};

