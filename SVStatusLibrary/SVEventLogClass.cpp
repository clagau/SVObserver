//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventLogClass
//* .File Name       : $Workfile:   SVEventLogClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   20 Aug 2014 12:25:38  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <exception>
#include "SVEventLogClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"


namespace SvStl
{
	SVEventLogClass::SVEventLogClass()
	:m_pucBuffer(nullptr), m_ulBufferSize(0)
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

		if( S_OK == l_svOk )
		{
			m_hHandle = ::OpenEventLog( nullptr, p_pszName );

			if( nullptr != m_hHandle )
			{
				m_csName = (nullptr != p_pszName) ? p_pszName : SVString();
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

		if( nullptr != m_hHandle )
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

		if( nullptr != m_hHandle )
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

		p_rsvRecord.SetBuffer( nullptr, SV_EVENT_RECORD_STRUCT_RECORD_SIZE );

		if( nullptr != m_hHandle )
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
							EVENTLOGRECORD* l_poRecord = nullptr;

							m_ulBytesRead = l_ulBytesRead;
							m_pucCurrent = m_pucBuffer;

							l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

							m_pucPrevious = m_pucCurrent + l_poRecord->Length;

							if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
							{
								m_pucPrevious = nullptr;
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
								m_pucBuffer = nullptr;

								try
								{
									m_pucBuffer = new unsigned char[l_ulBytesNeeded];
									m_ulBufferSize = l_ulBytesNeeded;
								}
								catch( std::bad_alloc* memExc )
								{
									m_pucBuffer = nullptr;
									m_ulBufferSize = 0;

									delete memExc;
								}
							}
						}
					} 
					while ( (ERROR_INSUFFICIENT_BUFFER != l_svOk ) && (S_OK != l_svOk) && (nullptr != m_pucBuffer)  && (l_iTries <= 2) ); //end do while
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

		p_rsvRecord.SetBuffer( nullptr, SV_EVENT_RECORD_STRUCT_RECORD_SIZE );

		if( nullptr != m_hHandle )
		{
			if( nullptr != m_pucPrevious )
			{
				EVENTLOGRECORD* l_poRecord = nullptr;

				m_pucCurrent = m_pucPrevious;

				l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

				m_pucPrevious = m_pucCurrent + l_poRecord->Length;

				if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
				{
					m_pucPrevious = nullptr;
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
						EVENTLOGRECORD* l_poRecord = nullptr;

						m_ulBytesRead = l_ulBytesRead;
						m_pucCurrent = m_pucBuffer;

						l_poRecord = reinterpret_cast<EVENTLOGRECORD*>( m_pucCurrent );

						m_pucPrevious = m_pucCurrent + l_poRecord->Length;

						if( m_pucBuffer + m_ulBytesRead <= m_pucPrevious )
						{
							m_pucPrevious = nullptr;
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
							m_pucBuffer = nullptr;

							try
							{
								m_pucBuffer = new unsigned char[l_ulBytesNeeded];
								m_ulBufferSize = l_ulBytesNeeded;
							}
							catch( std::bad_alloc *memExc )
							{
								m_pucBuffer = nullptr;
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
				while ( (ERROR_INSUFFICIENT_BUFFER == l_svOk) && (nullptr != m_pucBuffer) && (l_iTries <= 2) );
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

		m_hHandle = nullptr;

		m_ulBytesRead = 0;

		m_pucPrevious = nullptr;
		m_pucCurrent = nullptr;

		return l_svOk;
	}

	HRESULT SVEventLogClass::LocalDestroy()
	{
		HRESULT l_svOk = S_OK;

		if( nullptr != m_pucBuffer )
		{
			delete[] m_pucBuffer;

			m_pucBuffer = nullptr;
		}

		m_ulBufferSize = 0;

		// Should be one of the following: "Application", "Security", or "System"
		m_csName.clear();

		m_hHandle = nullptr;

		m_ulBytesRead = 0;

		m_pucPrevious = nullptr;
		m_pucCurrent = nullptr;

		return l_svOk;
	}
} //namespace SvStl
