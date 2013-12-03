//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncProcedure
//* .File Name       : $Workfile:   SVAsyncProcedure.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:12:40  $
//******************************************************************************

#include "SVStatusLibrary/SVException.h"
#include "SVStatusLibrary/SVStatusCodes.h"

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::SVAsyncProcedure()
{
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::~SVAsyncProcedure()
{
	Destroy();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
HRESULT SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Create(const SVAPCSignalHandler& apcHandler, const SVThreadSignalHandler& threadHandler, LPCTSTR tag)
{
	m_tag = tag;
	m_apcHandler = apcHandler;

	HRESULT hr = m_thread.Create(threadHandler, m_tag.c_str());
	return hr;
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
HRESULT SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Signal(void* pData)
{
	SVException l_svLog;

	HRESULT l_Status = S_OK;

	if( ! m_thread.IsActive() )
	{
		l_svLog.SetException(SV_FATAL_SVSYSTEMLIBRARY_0002, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 671, 0 );
		l_svLog.LogException();

		l_Status = m_thread.Restart();
	}

	if( l_Status == S_OK )
	{
		if( ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), ( ULONG_PTR )pData ) == 0 )
		{
			l_svLog.SetException(SV_FATAL_SVSYSTEMLIBRARY_0001, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 672, 0 );
			l_svLog.LogException();

			l_Status = m_thread.Restart();

			if( l_Status == S_OK )
			{
				if( ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), ( ULONG_PTR )pData ) == 0 )
				{
					l_Status = SV_FATAL_SVSYSTEMLIBRARY_0001;

					l_svLog.SetException(l_Status, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 674, 0 );
					l_svLog.LogException();
				}
			}
			else
			{
				l_svLog.SetException(l_Status, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 675, 0 );
				l_svLog.LogException();
			}
		}
	}
	else
	{
		l_svLog.SetException(l_Status, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 675, 0 );
		l_svLog.LogException();
	}

	return l_Status;
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
void SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Destroy()
{
	m_thread.Destroy();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
unsigned long SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::GetThreadID() const
{
	return m_thread.GetThreadID();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
int SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::GetPriority() const
{
	return m_thread.GetPriority();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
void SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::SetPriority(int priority)
{
	m_thread.SetPriority(priority);
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
bool SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::IsActive() const
{
	return m_thread.IsActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\SVAsyncProcedure.inl_v  $
 * 
 *    Rev 1.1   02 Oct 2013 10:12:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:48:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Sep 2011 15:14:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to allow access to the thread identifier.  Added a new Event notified thread object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 13:34:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.  Updated Thread creation functionality and re-create functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:45:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to this project to allow for multple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Aug 2009 16:48:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new files for the new threading model.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jul 2009 09:40:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Oct 2008 10:29:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed logging to event log due to excessive entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Oct 2008 10:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Get/Set Priority methods to get/set the thread priority
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2008 15:50:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to write to event log in case of signalling failure
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/