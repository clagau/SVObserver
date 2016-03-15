// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEventRecordStruct
// * .File Name       : $Workfile:   SVEventRecordStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 10:08:10  $
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
		size_t l_ulSize = strlen( m_szCurrentString ) + 1;
		
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

