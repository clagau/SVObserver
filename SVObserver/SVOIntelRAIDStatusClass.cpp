// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   23 Oct 2014 08:44:32  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <memory>
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
	long lType = 0;

	m_csRaidStatus.Empty();
	m_csErrorStatus.Empty();

	HRESULT l_svOk = l_psvLog->Open( _T("Application") );

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
				{	// Look for event source "IAANTMon"
					if( CString( l_svRecord.GetSourceName() ).CompareNoCase( _T("IAANTMon") ) == 0 )
					{
						lType = 1;
						LPCTSTR l_szString = l_svRecord.GetFirstString();
						CString l_strTmp = l_szString;
						int pos = 0;
						std::vector<CString> l_Strings;
						// Parse lines in text string.
						CString strTmp = l_strTmp.Tokenize(_T("\n"), pos );
						while( !strTmp.IsEmpty() )
						{	// parse all lines that contain the word status.
							if( strTmp.Find(_T("Status")) > -1 )
							{
								l_Strings.push_back(strTmp );
							}
							strTmp = l_strTmp.Tokenize( _T("\n"), pos );
						}

						if( l_Strings.size() > 0 )
						{	// first status string.
							m_csRaidStatus = l_Strings[0];
							if( l_Strings.size() > 1) // if more than 1 status string.
							{   // overwrite with second status string.
								m_csRaidStatus = l_Strings[1];
							}
						}
					}
					// Look for event source "IAStorDataMgrSvc"
					if( CString(l_svRecord.GetSourceName() ).CompareNoCase(_T("IAStorDataMgrSvc")) == 0 )
					{
						lType = 2;
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
		m_csErrorStatus.Format( _T("Error Reading Event Log (Item = %lu - ErrorCode = %lu)"), l_ulItem, l_svOk );

		FILE* l_pFile = ::fopen( _T("C:\\SVObserver\\LastEventReadError.txt"), _T("w") );

		if( l_pFile != NULL )
		{
			::fprintf( l_pFile,_T("%s\n"), m_csErrorStatus );

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
		if( lType == 1 )
		{	// If we find Normal then assume good.
			if( m_csRaidStatus.Find( _T("Normal") ) != -1 )
			{
				m_csRaidStatus.Empty();

				SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
			}
			else
			{
				SVSVIMStateClass::AddState( SV_STATE_RAID_FAILURE );
			}
		}
		if( lType == 2 )
		{	// If we do not find Degraded or rebuilding... then assume good.
			if( (m_csRaidStatus.Find( _T("Degraded") ) == -1) &&
				(m_csRaidStatus.Find( _T("Rebuilding in progress")) == -1))
			{
				m_csRaidStatus.Empty();

				SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
			}
			else
			{
				SVSVIMStateClass::AddState( SV_STATE_RAID_FAILURE );
			}
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

