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

#ifndef SVJSONCOMMANDMANAGER_H
#define SVJSONCOMMANDMANAGER_H

#include <string>

#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVJsonCommandData.h"
#include "SVJsonCommandServer.h"

template< typename SVCommandProcessor >
class SVJsonCommandManager
{
public:
	friend class SVObserverApp;

	static SVJsonCommandManager& Instance();

	virtual ~SVJsonCommandManager();

	HRESULT ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessJsonNotification( const std::string& p_rJsonNotification );

protected:
	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	SVJsonCommandManager();

	void Startup(unsigned short p_PortNumber);
	void Shutdown();

	HRESULT ProcessAsyncJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	void ThreadProcess( bool& p_WaitForEvents );

	SVJsonCommandServer m_SocketServer;

	long m_ProcessingAsyncCommand;
	SVJsonCommandDataPtr m_JsonCommandDataPtr;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;

private:
	SVJsonCommandManager( const SVJsonCommandManager& p_rObject );
	SVJsonCommandManager& operator=( const SVJsonCommandManager& p_rObject );

};

#include "SVJsonCommandManager.inl"

#endif

