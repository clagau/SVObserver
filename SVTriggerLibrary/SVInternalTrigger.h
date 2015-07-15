//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInternalTrigger
//* .File Name       : $Workfile:   SVInternalTrigger.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 14:15:24  $
//******************************************************************************
#ifndef SVINTERNALTRIGGER_H
#define SVINTERNALTRIGGER_H

#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVTriggerHandler.h"
#include "SVSystemLibrary/SVThread.h"

class SVInternalTrigger
{
	typedef boost::function<void ( bool& )> ThreadSignalHandler;

public:
	SVInternalTrigger();
	~SVInternalTrigger();

	void Create(LPCTSTR p_strOwner, SVThreadAttribute eAttr);
	void Destroy();

	HRESULT Subscribe(unsigned long p_ulIndex, SVTriggerHandler& handler);
	HRESULT Unsubscribe(unsigned long p_ulIndex);
	bool HasSubscribers(unsigned long p_ulIndex) const;

	HRESULT Fire(unsigned long p_ulIndex);

private:
	SVThread<ThreadSignalHandler> m_thread;
	SVTriggerHandlerList m_triggerHandlerList;

	void Dispatch( bool& p_WaitForEvents );
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTriggerLibrary\SVInternalTrigger.h_v  $
 * 
 *    Rev 1.1   01 Dec 2014 14:15:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attributer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:55:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Feb 2010 10:03:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2010 15:47:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Sep 2009 09:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 09:59:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
