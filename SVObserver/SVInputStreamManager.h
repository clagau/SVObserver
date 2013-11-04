//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, INc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputStreamManager
//* .File Name       : $Workfile:   SVInputStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:01:40  $
//******************************************************************************

#ifndef SVINPUTSTREAMMANAGER_H
#define SVINPUTSTREAMMANAGER_H

#include <map>
#include <string>

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"

namespace Json
{
	class Value;
};

class SVInputStreamManager
{
public:
	friend class SVObserverApp;

	static SVInputStreamManager& Instance();

	virtual ~SVInputStreamManager();

	HRESULT QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	HRESULT QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	HRESULT ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVGUID FindInputStream( const std::string& p_rName );

	HRESULT InsertInputStream( const std::string& p_rName, const SVGUID& p_rObjectId );
	HRESULT EraseInputStream( const SVGUID& p_rObjectId );

	HRESULT ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

protected:
	typedef std::map< std::string, SVGUID > SVInputStreamMap;

	SVInputStreamManager();

	void Startup(unsigned short p_PortNumber);
	void Shutdown();

	HRESULT SendCommandToInputStream( const std::string& p_rName, const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessStreamManagerJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const;
	HRESULT ProcessJsonValuesToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const;

	HRESULT ConvertJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, const std::string& p_rJson ) const;

	SVJsonCommandServer m_SocketServer;
	SVCriticalSection m_Lock;
	SVInputStreamMap m_InputStreams;

private:
	SVInputStreamManager( const SVInputStreamManager& p_rObject );
	const SVInputStreamManager& operator=( const SVInputStreamManager& p_rObject );

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInputStreamManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:01:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:39:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new stream manager to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
