//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandManager
//* .File Name       : $Workfile:   SVJsonCommandManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 09:26:18  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVJsonCommandData.h"
#include "SVJsonCommandServer.h"
#pragma endregion Includes

template< typename SVCommandProcessor >
class SVJsonCommandManager
{
public:
	static SVJsonCommandManager& Instance();

	virtual ~SVJsonCommandManager();

	HRESULT ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessJsonNotification( const std::string& p_rJsonNotification );

	// These two (2) methods, Startup, Shutdown are only meant to be called by the main application class and no other
	// They used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call these methods anywhere else, as described via this comment
	void Startup(unsigned short PortNumber); // This method is only meant to be called by the main application class
	void Shutdown();						 // This method is only meant to be called by the main application class

protected:
	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	HRESULT ProcessAsyncJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	void ThreadProcess( bool& p_WaitForEvents );

	SVJsonCommandServer m_SocketServer;

	long m_ProcessingAsyncCommand;
	SVJsonCommandDataPtr m_JsonCommandDataPtr;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;

private:
	SVJsonCommandManager();
	SVJsonCommandManager( const SVJsonCommandManager& p_rObject );
	SVJsonCommandManager& operator=( const SVJsonCommandManager& p_rObject );
};

#include "SVJsonCommandManager.inl"
