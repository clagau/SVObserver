// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODDELoadLibraryClass
// * .File Name       : $Workfile:   SVIODDELoadLibraryClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:30  $
// ******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline SVIODDELoadLibraryClass::SVIODDELoadLibraryClass()
{
	m_hmHandle = NULL;

	m_psvSetServer = NULL;
	m_psvSendData = NULL;
	m_psvReceiveData = NULL;
}

inline SVIODDELoadLibraryClass::~SVIODDELoadLibraryClass()
{
	Close();
}

inline HRESULT SVIODDELoadLibraryClass::Open(LPCTSTR p_szLibrary)
{
	HRESULT l_hrOk = S_OK;

	if ( m_hmHandle == NULL )
	{
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( m_hmHandle != NULL )
		{
			m_psvSetServer = (SVDDESetServerPtr)::GetProcAddress( m_hmHandle, "SVDDESetServer" );
			m_psvSendData = (SVDDESendDataPtr)::GetProcAddress( m_hmHandle, "SVDDESendData" );
			m_psvReceiveData = (SVDDEReceiveDataPtr)::GetProcAddress( m_hmHandle, "SVDDEReceiveData" );

			if ( m_psvSetServer != NULL &&
			     m_psvSendData != NULL &&
			     m_psvReceiveData != NULL )
			{
				l_hrOk = S_OK;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( l_hrOk != S_OK )
	{
		Close();
	}

	return l_hrOk;
}

inline HRESULT SVIODDELoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_OK;

	if ( m_hmHandle != NULL )
	{
		if ( ::FreeLibrary( m_hmHandle ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			m_hmHandle = NULL;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	m_psvSetServer = NULL;
	m_psvSendData = NULL;
	m_psvReceiveData = NULL;

	return l_hrOk;
}




inline HRESULT SVIODDELoadLibraryClass::SetDDEServer( HWND HWnd, BSTR TStrServer, BSTR TStrTopic )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetServer != NULL )
	{
		if(m_psvSetServer( HWnd, TStrServer, TStrTopic ) )
			l_hrOk = S_OK;
	}

	return l_hrOk;
}

inline HRESULT SVIODDELoadLibraryClass::SendDDEData( SVDDEItemStruct *p_pItem )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSendData != NULL )
	{
		if(m_psvSendData( p_pItem ) )
			l_hrOk = S_OK;

	}

	return l_hrOk;
}

inline HRESULT SVIODDELoadLibraryClass::ReceiveDDEData( SVDDEItemStruct *p_pItem )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvReceiveData != NULL )
	{
		if(m_psvReceiveData( p_pItem ))
			l_hrOk = S_OK;

	}

	return l_hrOk;
}

