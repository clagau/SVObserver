//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventLogClass
//* .File Name       : $Workfile:   SVEventLogClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:42:16  $
//******************************************************************************

#include "stdafx.h"
#include <exception>
#include "SVEventLogClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"


SVEventLogClass::SVEventLogClass()
:m_pucBuffer(NULL),m_ulBufferSize(0)
{
	LocalInitialize();
}

SVEventLogClass::~SVEventLogClass()
{
	Close();
	LocalDestroy();
}

HRESULT SVEventLogClass::Open( LPCTSTR p_pszName )
{
	HRESULT l_svOk = Close();

	if( l_svOk == S_OK )
	{
		m_hHandle = ::OpenEventLog( NULL, p_pszName );

		if( m_hHandle != NULL )
		{
			m_csName = p_pszName;
		}
		else
		{
			l_svOk = ::GetLastError();
		}
	}

	return l_svOk;
}

HRESULT SVEventLogClass::Close()
{
	HRESULT l_svOk = S_OK;

	if( m_hHandle != NULL )
	{
		if( ! ::CloseEventLog( m_hHandle ) )
		{
			l_svOk = ::GetLastError();
		}
	}


	return l_svOk;
}

HRESULT SVEventLogClass::GetRecordCount( unsigned long& p_rulCount )
{
	HRESULT l_svOk = S_FALSE;

	p_rulCount = 0;

	if( m_hHandle != NULL )
	{
		if( ::GetNumberOfEventLogRecords( m_hHandle, &p_rulCount ) )
		{
			l_svOk = S_OK;
		}
		else
		{
			l_svOk = ::GetLastError();
		}
	}

	return l_svOk;
}

HRESULT SVEventLogClass::ReadLast( SVEventRecordStruct& p_rsvRecord )
{
	HRESULT l_svOk = S_FALSE;

	p_rsvRecord.SetBuffer( NULL, SV_EVENT_RECORD_STRUCT_RECORD_SIZE );

	if( m_hHandle != NULL )
	{
		unsigned long l_ulRecordNumber = 1;

		if( ::GetOldestEventLogRecord( m_hHandle, &l_ulRecordNumber ) )
		{
			unsigned long l_ulRecordCount = 1;

			if( ::GetNumberOfEventLogRecords( m_hHandle, &l_ulRecordCount ) )
			{
				unsigned long l_ulBytesRead = 0;
				unsigned long l_ulBytesNeeded = 0;

				l_ulRecordNumber += l_ulRecordCount - 1;

				int l_iTries = 0;
				do
				{
					l_iTries++;
					if( ::ReadEventLog( m_hHandle, EVENTLOG_SEEK_READ | EVENTLOG_BACKWARDS_READ, l_ulRecordNumber, 
						m_pucBuffer, m_ulBufferSize, &l_ulBytesRead, &l_ulBytesNeeded ) )
					{
						EVENTLOGRECORD* l_poRecord = NULL;

						m_ulBytesRead = l_ulBytesRead;
						m_pucCurrent = m_pucBuffer;

						l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

						m_pucPrevious = m_pucCurrent + l_poRecord->Length;

						if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
						{
							m_pucPrevious = NULL;
						}

						p_rsvRecord.SetBuffer( m_pucCurrent, l_poRecord->Length );

						l_svOk = S_OK;
					}
					else
					{
						l_svOk = ::GetLastError();

						if ( l_svOk == ERROR_INSUFFICIENT_BUFFER )
						{
							delete[] m_pucBuffer;
							m_pucBuffer = NULL;

							try
							{
								m_pucBuffer = new unsigned char[l_ulBytesNeeded];
								m_ulBufferSize = l_ulBytesNeeded;
							}
							catch( std::bad_alloc* memExc )
							{
								m_pucBuffer = NULL;
								m_ulBufferSize = 0;

								delete memExc;
							}
						}
					}
				} 
				while ( (l_svOk != ERROR_INSUFFICIENT_BUFFER) && (l_svOk != S_OK) && (m_pucBuffer != NULL)  && (l_iTries <= 2) ); //end do while
			}
			else
			{
				l_svOk = ::GetLastError();
			}
		}
		else
		{
			l_svOk = ::GetLastError();
		}
	}

	 // End of File Error - Not an error - Change status to OK
	if ( l_svOk == (HRESULT)(38) )
	{
		l_svOk = S_OK;
	}

	return l_svOk;
}

HRESULT SVEventLogClass::ReadPrevious( SVEventRecordStruct& p_rsvRecord )
{
	HRESULT l_svOk = S_FALSE;

	p_rsvRecord.SetBuffer( NULL, SV_EVENT_RECORD_STRUCT_RECORD_SIZE );

	if( m_hHandle != NULL )
	{
		if( m_pucPrevious != NULL )
		{
			EVENTLOGRECORD* l_poRecord = NULL;

			m_pucCurrent = m_pucPrevious;

			l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

			m_pucPrevious = m_pucCurrent + l_poRecord->Length;

			if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
			{
				m_pucPrevious = NULL;
			}

			p_rsvRecord.SetBuffer( m_pucCurrent, l_poRecord->Length );

			l_svOk = S_OK;
		}
		else
		{
			unsigned long l_ulBytesRead = 0;
			unsigned long l_ulBytesNeeded = 0;

			int l_iTries = 0;
			do
			{
				l_iTries++;
				if( ::ReadEventLog( m_hHandle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ, 1, 
					m_pucBuffer, m_ulBufferSize, &l_ulBytesRead, &l_ulBytesNeeded ) )
				{
					EVENTLOGRECORD* l_poRecord = NULL;

					m_ulBytesRead = l_ulBytesRead;
					m_pucCurrent = m_pucBuffer;

					l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

					m_pucPrevious = m_pucCurrent + l_poRecord->Length;

					if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
					{
						m_pucPrevious = NULL;
					}

					p_rsvRecord.SetBuffer( m_pucCurrent, l_poRecord->Length );

					l_svOk = S_OK;
				}
				else
				{
					l_svOk = ::GetLastError();
					if ( l_svOk == ERROR_INSUFFICIENT_BUFFER )
					{
						delete[] m_pucBuffer;
						m_pucBuffer = NULL;

						try
						{
							m_pucBuffer = new unsigned char[l_ulBytesNeeded];
							m_ulBufferSize = l_ulBytesNeeded;
						}
						catch( std::bad_alloc *memExc )
						{
							m_pucBuffer = NULL;
							m_ulBufferSize = 0;

							delete memExc;
						}
					}
					else
					{
						if ( l_svOk == (HRESULT)(38) )
						{
							l_svOk = S_OK;
						}

					}
				}
			}
			while ( (l_svOk != ERROR_INSUFFICIENT_BUFFER)&& (l_svOk != S_OK) && (m_pucBuffer != NULL) && (l_iTries <= 2) );
		}
	}

	 // End of File Error - Not an error - Change status to OK
	if ( l_svOk == (HRESULT)(38) )
	{
		l_svOk = S_OK;
	}

	return l_svOk;
}


HRESULT SVEventLogClass::LocalInitialize()
{
	HRESULT l_svOk = S_OK;

	m_pucBuffer = new unsigned char[SV_EVENT_RECORD_STRUCT_RECORD_SIZE];
	m_ulBufferSize = SV_EVENT_RECORD_STRUCT_RECORD_SIZE;

	// Should be one of the following: "Application", "Security", or "System"
	m_csName.clear();

	m_hHandle = NULL;

	m_ulBytesRead = 0;

	m_pucPrevious = NULL;
	m_pucCurrent = NULL;

	return l_svOk;
}

HRESULT SVEventLogClass::LocalDestroy()
{
	HRESULT l_svOk = S_OK;

	if( m_pucBuffer != NULL )
	{
		delete[] m_pucBuffer;

		m_pucBuffer = NULL;
	}

	m_ulBufferSize = 0;

	// Should be one of the following: "Application", "Security", or "System"
	m_csName.clear();

	m_hHandle = NULL;

	m_ulBytesRead = 0;

	m_pucPrevious = NULL;
	m_pucCurrent = NULL;

	return l_svOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\SVEventLogClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:42:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Nov 2009 13:29:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed unreliable read from event log.  Added functions to aid in debugging.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2009 16:11:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed retry logic in both ReadLast and ReadPrevious.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Nov 2008 15:49:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Wrong Size was being sent to ReadEventLog in ReadPrevious.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jun 2008 13:38:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   fixed problem with reading from the event viewer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2007 15:12:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jul 2006 14:40:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem if the EventView Record is longer then what is allocated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Apr 2006 15:11:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  560
 * SCR Title:  Remove un-necessary event log read errors for SATA RAID status metholodolgy
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated read methods to ignore end-of-file contition and updated the size of the read buffer to 512K.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Dec 2005 15:08:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new objects to the library to read the Windows Event Log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
