//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketThread
//* .File Name       : $Workfile:   SVSocketThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:17:56  $
//******************************************************************************

#pragma once

#include <comdef.h>
#include <string>

/*
ThreadFunc must have the following prototype:

     void ThreadFunc();

*/

template <typename ThreadFunc>
class SVSocketThread
{
private:
	bool m_bQuit;
	HANDLE m_hThread;
	unsigned long m_ulThreadID;
	_bstr_t m_tag;
	ThreadFunc m_threadFunc;

	static DWORD WINAPI ThreadProc( LPVOID lpParam );

public:
	SVSocketThread();
	~SVSocketThread();

	HRESULT Create(const ThreadFunc& threadFunc, const _bstr_t& p_rTag);
	void Destroy();

	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;
};

#include "SVSocketThread.inl"

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSocketLibrary\SVSocketThread.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:17:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 11:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to include error messages and changes thread name from standard string to basic string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/