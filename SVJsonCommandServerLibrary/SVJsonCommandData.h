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

#ifndef SVJSONCOMMANDDATA_H
#define SVJSONCOMMANDDATA_H

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonCommandData.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:14:26   bWalter
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
