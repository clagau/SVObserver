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

#ifndef SVOBJECTCOMMANDDATAJSON_H
#define SVOBJECTCOMMANDDATAJSON_H

#include <string>
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectCommandDataJson.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:40:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:29:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated main application and create the command data holder for the JSON commands used with the socket and the remote input tool.  Updated source code to initialize, start and stop the scoket for sending and receiving data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
