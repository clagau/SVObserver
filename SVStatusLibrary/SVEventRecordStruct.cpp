// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEventRecordStruct
// * .File Name       : $Workfile:   SVEventRecordStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 17:42:44  $
// ******************************************************************************

#include "stdafx.h"
#include "SVEventRecordStruct.h"

SVEventRecordStruct::SVEventRecordStruct()
: m_ulBufferSize(0L), m_pucBuffer(NULL)
{
	LocalInitialize(SV_EVENT_RECORD_STRUCT_RECORD_SIZE);
}

SVEventRecordStruct::~SVEventRecordStruct()
{
	LocalDestroy();
}

void SVEventRecordStruct::SetBuffer( unsigned char* p_pucBuffer, unsigned long p_unLength )
{
	if( p_unLength > m_ulBufferSize )
	{
		LocalInitialize( p_unLength );
	}

	if( p_pucBuffer != NULL )
	{
		EVENTLOGRECORD* l_poRecord = NULL;

		l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( p_pucBuffer );

		memcpy( m_pucBuffer, p_pucBuffer, l_poRecord->Length );

		m_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucBuffer );

		m_szSourceName = reinterpret_cast<LPCTSTR>( m_pucBuffer + sizeof( EVENTLOGRECORD ) );
		m_szComputerName = m_szSourceName + strlen( m_szSourceName ) + 1;
	}
}

EVENTLOGRECORD* SVEventRecordStruct::GetEventLogRecord()
{
	return m_poRecord;
}

LPCTSTR SVEventRecordStruct::GetSourceName()
{
	return m_szSourceName;
}

LPCTSTR SVEventRecordStruct::GetComputerName()
{
	return m_szComputerName;
}

LPCTSTR SVEventRecordStruct::GetFirstString()
{
	m_szCurrentString = NULL;

	if( m_poRecord != NULL && 0 < m_poRecord->NumStrings )
	{
		m_szCurrentString = reinterpret_cast<LPCTSTR>( m_pucBuffer + m_poRecord->StringOffset );

		m_ulStringIndex = 0;
	}

	return m_szCurrentString;
}

LPCTSTR SVEventRecordStruct::GetNextString()
{
	if( m_poRecord != NULL && m_ulStringIndex + 1 < m_poRecord->NumStrings )
	{
		unsigned long l_ulSize = strlen( m_szCurrentString ) + 1;
		
		m_szCurrentString += l_ulSize;

		m_ulStringIndex++;
	}
	else
	{
		m_szCurrentString = NULL;
	}

	return m_szCurrentString;
}

HRESULT SVEventRecordStruct::LocalInitialize(unsigned long p_ulSize)
{
	LocalDestroy();

	if( 0 < p_ulSize )
	{
		m_pucBuffer = new unsigned char[p_ulSize];

		m_ulBufferSize = p_ulSize;
	}

	return S_OK;
}

HRESULT SVEventRecordStruct::LocalDestroy()
{
	m_poRecord = NULL;

	if ( m_pucBuffer != NULL )
	{
		delete[] m_pucBuffer;

		m_pucBuffer = NULL;
	}

	m_ulBufferSize = 0;

	m_szSourceName = NULL;
	m_szComputerName = NULL;

	m_szCurrentString = NULL;
	m_ulStringIndex = 0;

	return S_OK;
}

LPCTSTR SVEventRecordStruct::GetType()
{
	LPCTSTR l_pString = NULL;
	if( m_poRecord != NULL )
	{
		switch( m_poRecord->EventType )
		{
			case EVENTLOG_ERROR_TYPE:
			{
				l_pString = _T("Error");
				break;
			}
			case EVENTLOG_AUDIT_FAILURE:
			{
				l_pString = _T("Audit Failure");
				break;
			}
			case EVENTLOG_AUDIT_SUCCESS:
			{
				l_pString = _T("Success");
				break;
			}
			case EVENTLOG_INFORMATION_TYPE:
			{
				l_pString = _T("Information");
				break;
			}
			case EVENTLOG_WARNING_TYPE:
			{
				l_pString = _T("Warning");
				break;
			}
			default:
			{
				l_pString = _T("Information");
				break;
			}
		}
	}
	return l_pString;
}

std::string SVEventRecordStruct::GetDateTime()
{
	CTime l_time((__time64_t)m_poRecord->TimeWritten);
	return static_cast<LPCTSTR>( l_time.Format(_T("%m/%d/%Y  %I:%M:%S %p")));
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\SVEventRecordStruct.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:42:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2010 08:16:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 2009 13:29:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed unreliable read from event log.  Added functions to aid in debugging.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
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
 *    Rev 1.2   26 Jul 2006 12:54:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed the buffer size to be an unsigned long
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2006 14:40:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem if the EventView Record is longer then what is allocated.
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
