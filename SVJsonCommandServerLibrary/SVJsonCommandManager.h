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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonCommandManager.h_v  $
 * 
 *    Rev 1.2   01 Oct 2013 09:26:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jun 2013 17:15:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 May 2013 07:40:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updted source code to add the new process notification methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:14:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
