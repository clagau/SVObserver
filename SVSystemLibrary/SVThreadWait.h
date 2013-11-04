//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadWait
//* .File Name       : $Workfile:   SVThreadWait.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:03:30  $
//******************************************************************************

#pragma once

#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVEventThread.h"

class SVThreadWait
{
public:
	SVThreadWait();
	virtual ~SVThreadWait();

	HRESULT Create();
	void Destroy();

	void Sleep();

protected:
	typedef boost::function<void ( bool& )> SVThreadSignalHandler;
	typedef SVEventThread<SVThreadSignalHandler> SVWaitEventThread;

	void Dispatch( bool& p_WaitForEvents );

	SVWaitEventThread m_Thread;
	HANDLE m_WaitEvent;

};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVThreadWait.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:03:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

