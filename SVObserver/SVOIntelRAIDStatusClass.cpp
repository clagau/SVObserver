// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   23 Jul 2013 18:00:26  $
// ******************************************************************************

#include "stdafx.h"
#include <memory>
#include "SVOIntelRAIDStatusClass.h"
#include "ModelessMessageBox.h"
#include "SVStatusLibrary/SVEventLogClass.h"
#include "SVSVIMStateClass.h"

SVOIntelRAIDStatusClass::SVOIntelRAIDStatusClass()
	:m_hCheckEvent( NULL ), m_csRaidStatus(), m_csErrorStatus()
{
	m_hCheckEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

SVOIntelRAIDStatusClass::~SVOIntelRAIDStatusClass()
{
	if( m_hCheckEvent != NULL )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = NULL;
	}
}

HRESULT SVOIntelRAIDStatusClass::UpdateStatus()
{
	HRESULT l_svOk = S_OK;

	if( m_hCheckEvent != NULL )
	{
		switch( ::WaitForSingleObject( m_hCheckEvent, 0 ) )
		{
			case WAIT_TIMEOUT:
			{
				l_svOk = CheckStatus();

				if( ! m_csRaidStatus.IsEmpty() )
				{
					if( ::SetEvent( m_hCheckEvent ) )
					{
						CString l_csMessage;

						l_csMessage.Format( "The RAID system is not reporting a functional status.\n\n"
							"STATUS : %s\n\n"
							"Please contact your support representive for assistance.", m_csRaidStatus );

						ModelessMessageBox( l_csMessage, m_hCheckEvent );
					}
					else
					{
						l_svOk = S_FALSE;
					}
				}

				break;
			}
			case WAIT_OBJECT_0:
			{
				break;
			}
			default:
			{
				l_svOk = S_FALSE;

				break;
			}
		}
	}
	else
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

HRESULT SVOIntelRAIDStatusClass::CheckStatus()
{
	std::auto_ptr<SVEventLogClass> l_psvLog (new SVEventLogClass);

	unsigned long l_ulItem = 0;

	m_csRaidStatus.Empty();
	m_csErrorStatus.Empty();

	HRESULT l_svOk = l_psvLog->Open( "Application" );

	if( l_svOk == S_OK )
	{
		unsigned long l_ulCount = 0;

		l_svOk = l_psvLog->GetRecordCount( l_ulCount );

		if( l_svOk == S_OK )
		{
			SVEventRecordStruct l_svRecord;

			CString l_csStatus;

			l_svOk = l_psvLog->ReadLast( l_svRecord );

			if( l_svOk == S_OK )
			{
				l_ulItem = 1;

				while( l_svOk == S_OK && l_ulItem <= l_ulCount  && m_csRaidStatus.IsEmpty() )
				{
					if( CString( l_svRecord.GetSourceName() ).CompareNoCase( "IAANTMon" ) == 0 )
					{
						LPCTSTR l_szString = l_svRecord.GetFirstString();

						while( l_szString != NULL )
						{
							if( 0 < strlen( l_szString ) )
							{
								l_csStatus = l_szString;
							}

							l_szString = l_svRecord.GetNextString();
						}

						m_csRaidStatus = l_csStatus;
					}

					l_svOk = l_psvLog->ReadPrevious( l_svRecord );

					l_ulItem++;
				}
			}
		}
	}

	if( l_svOk != S_OK )
	{
		m_csErrorStatus.Format( "Error Reading Event Log (Item = %lu - ErrorCode = %lu)", l_ulItem, l_svOk );

		FILE* l_pFile = ::fopen( "C:\\SVObserver\\LastEventReadError.txt", "w" );

		if( l_pFile != NULL )
		{
			::fprintf( l_pFile,"%s\n", m_csErrorStatus );

			::fclose( l_pFile );

			l_pFile = NULL;
		}
	}

	if( m_csRaidStatus.IsEmpty() )
	{
		SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
	}
	else
	{
		if( m_csRaidStatus.CompareNoCase( "Normal" ) == 0 )
		{
			m_csRaidStatus.Empty();

			SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
		}
		else
		{
			SVSVIMStateClass::AddState( SV_STATE_RAID_FAILURE );
		}
	}

	return l_svOk;
}

const CString& SVOIntelRAIDStatusClass::GetRaidStatus()
{
	return m_csRaidStatus;
}

const CString& SVOIntelRAIDStatusClass::GetErrorStatus()
{
	return m_csErrorStatus;
}

const HANDLE SVOIntelRAIDStatusClass::GetCheckEvent()
{
	return m_hCheckEvent;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOIntelRAIDStatusClass.cpp_v  $
 * 
 *    Rev 1.1   23 Jul 2013 18:00:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for <memory>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:05:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   09 Jul 2012 14:12:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 Jun 2012 13:28:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Oct 2010 14:08:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 14:05:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Nov 2009 13:34:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unneccessary reads past the end of the event log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Sep 2009 10:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Nov 2008 16:12:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added a few extra itterations of reading the event log records from the eventlog This is done because new records may be comming in while reading. So we need to be able to read all records.  It is ok to attempt to read past the end if new records did not come in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jun 2006 08:49:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with the creation of the SVEventLogClass.  Is now being created on the heap instead of the stack.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2006 12:19:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to only check the status string to see if there was a SATA RAID error and on any other error write it to an error file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Mar 2006 15:03:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated RAID Status class to include the element when the read failure happens.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jan 2006 11:59:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added error code to the status message
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Jan 2006 09:55:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to include new RAID Failure state for SIAC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Dec 2005 15:12:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new classes to handle getting the invalid RAID status to the GUI.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
