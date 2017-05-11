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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <list>
#include "SVOIntelRAIDStatusClass.h"
#include "SVMFCControls/ModelessMessageBox.h"
#include "SVStatusLibrary/SVEventLogClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

SVOIntelRAIDStatusClass::SVOIntelRAIDStatusClass()
	:m_hCheckEvent( nullptr ), m_RaidStatus(), m_ErrorStatus()
{
	m_hCheckEvent = ::CreateEvent( nullptr, true, false, nullptr );
}

SVOIntelRAIDStatusClass::~SVOIntelRAIDStatusClass()
{
	if( nullptr != m_hCheckEvent )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = nullptr;
	}
}

HRESULT SVOIntelRAIDStatusClass::UpdateStatus()
{
	HRESULT l_svOk = S_OK;

	if( nullptr != m_hCheckEvent )
	{
		switch( ::WaitForSingleObject( m_hCheckEvent, 0 ) )
		{
			case WAIT_TIMEOUT:
			{
				l_svOk = CheckStatus();

				if( !m_RaidStatus.empty() )
				{
					if( ::SetEvent( m_hCheckEvent ) )
					{
						SVString Message = SvUl_SF::Format( _T("The RAID system is not reporting a functional status.\n\n"
							"STATUS : %s\n\n"
							"Please contact your support representative for assistance."), m_RaidStatus );

						CallModelessMessageBox( Message, m_hCheckEvent );
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

	m_RaidStatus.clear();
	m_ErrorStatus.clear();

	HRESULT l_svOk = l_psvLog->Open( _T("Application") );

	if( S_OK == l_svOk )
	{
		unsigned long l_ulCount = 0;

		l_svOk = l_psvLog->GetRecordCount( l_ulCount );

		if( S_OK == l_svOk )
		{
			SVEventRecordStruct l_svRecord;

			CString l_csStatus;

			l_svOk = l_psvLog->ReadLast( l_svRecord );

			if( S_OK == l_svOk )
			{
				l_ulItem = 1;

				while( S_OK == l_svOk && l_ulItem <= l_ulCount  && m_RaidStatus.empty() )
				{	// Look for event source "IAANTMon"
					if( 0 == SvUl_SF::CompareNoCase( SVString(l_svRecord.GetSourceName()), SVString(_T("IAANTMon")) ) )
					{
						lType = 1;
						SVStringVector StatusStrings;
						CString StatusString = l_svRecord.GetFirstString();
						int pos = 0;
						// Parse lines in text string.
						CString strTmp = StatusString.Tokenize(_T("\n"), pos );
						while( !strTmp.IsEmpty() )
						{	// parse all lines that contain the word status.
							if( strTmp.Find(_T("Status")) > -1 )
							{
								StatusStrings.push_back( SVString(strTmp) );
							}
							strTmp = strTmp.Tokenize( _T("\n"), pos );
						}

						if( StatusStrings.size() > 0 )
						{	// first status string.
							m_RaidStatus = StatusStrings[0];
							if( StatusStrings.size() > 1) // if more than 1 status string.
							{   // overwrite with second status string.
								m_RaidStatus = StatusStrings[1];
							}
						}
					}
					// Look for event source "IAStorDataMgrSvc"
					if( CString(l_svRecord.GetSourceName() ).CompareNoCase(_T("IAStorDataMgrSvc")) == 0 )
					{
						lType = 2;
						LPCTSTR l_szString = l_svRecord.GetFirstString();

						while( nullptr != l_szString )
						{
							if( 0 < strlen( l_szString ) )
							{
								l_csStatus = l_szString;
							}

							l_szString = l_svRecord.GetNextString();
						}

						m_RaidStatus = l_csStatus;
					}

					l_svOk = l_psvLog->ReadPrevious( l_svRecord );

					l_ulItem++;
				}
			}
		}
	}

	if( S_OK != l_svOk )
	{
		m_ErrorStatus = SvUl_SF::Format( _T("Error Reading Event Log (Item = %lu - ErrorCode = %lu)"), l_ulItem, l_svOk );

		FILE* l_pFile = ::fopen(  SvStl::GlobalPath::Inst().GetObserverPath(_T("LastEventReadError.txt")).c_str() , _T("w") );

		if( nullptr != l_pFile )
		{
			::fprintf( l_pFile,_T("%s\n"), m_ErrorStatus.c_str() );

			::fclose( l_pFile );

			l_pFile = nullptr;
		}
	}

	if( m_RaidStatus.empty() )
	{
		SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
	}
	else
	{
		if( lType == 1 )
		{	// If we find Normal then assume good.
			if( SVString::npos != m_RaidStatus.find( _T("Normal") ) )
			{
				m_RaidStatus.clear();

				SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
			}
			else
			{
				SVSVIMStateClass::AddState( SV_STATE_RAID_FAILURE );
			}
		}
		if( lType == 2 )
		{	// If we do not find Degraded or rebuilding... then assume good.
			if( SVString::npos == m_RaidStatus.find( _T("Degraded") ) &&
				SVString::npos == m_RaidStatus.find( _T("Rebuilding in progress")) )
			{
				m_RaidStatus.clear();

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

const SVString& SVOIntelRAIDStatusClass::GetRaidStatus()
{
	return m_RaidStatus;
}

const SVString& SVOIntelRAIDStatusClass::GetErrorStatus()
{
	return m_ErrorStatus;
}

const HANDLE SVOIntelRAIDStatusClass::GetCheckEvent()
{
	return m_hCheckEvent;
}

