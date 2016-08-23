// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTriggerLoadLibraryClass
// * .File Name       : $Workfile:   SVIOTriggerLoadLibraryClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:48:20  $
// ******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline SVIOTriggerLoadLibraryClass::SVIOTriggerLoadLibraryClass()
{
	m_hmHandle = nullptr;

	m_psvCreate = nullptr;
	m_psvDestroy = nullptr;
	m_psvGetCount = nullptr;
	m_psvGetHandle = nullptr;
	m_psvGetName = nullptr;
	m_psvRegister = nullptr;
	m_psvUnregister = nullptr;
	m_psvUnregisterAll = nullptr;
	m_psvStart = nullptr;
	m_psvStop = nullptr;
	m_psvGetParameterCount = nullptr;
	m_psvGetParameterName = nullptr;
	m_psvGetParameterValue = nullptr;
	m_psvSetParameterValue = nullptr;
}

inline SVIOTriggerLoadLibraryClass::~SVIOTriggerLoadLibraryClass()
{
	Close();
}

inline HRESULT SVIOTriggerLoadLibraryClass::Open(LPCTSTR p_szLibrary)
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == m_hmHandle )
	{
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( nullptr != m_hmHandle )
		{
			m_psvCreate = (SVCreatePtr)::GetProcAddress( m_hmHandle, "SVCreate" );
			m_psvDestroy = (SVDestroyPtr)::GetProcAddress( m_hmHandle, "SVDestroy" );
			m_psvGetCount = (SVTriggerGetCountPtr)::GetProcAddress( m_hmHandle, "SVTriggerGetCount" );
			m_psvGetHandle = (SVTriggerGetHandlePtr)::GetProcAddress( m_hmHandle, "SVTriggerGetHandle" );
			m_psvGetName = (SVTriggerGetNamePtr)::GetProcAddress( m_hmHandle, "SVTriggerGetName" );
			m_psvRegister = (SVTriggerRegisterPtr)::GetProcAddress( m_hmHandle, "SVTriggerRegister" );
			m_psvUnregister = (SVTriggerUnregisterPtr)::GetProcAddress( m_hmHandle, "SVTriggerUnregister" );
			m_psvUnregisterAll = (SVTriggerUnregisterAllPtr)::GetProcAddress( m_hmHandle, "SVTriggerUnregisterAll" );
			m_psvStart = (SVTriggerStartPtr)::GetProcAddress( m_hmHandle, "SVTriggerStart" );
			m_psvStop = (SVTriggerStopPtr)::GetProcAddress( m_hmHandle, "SVTriggerStop" );
			m_psvGetParameterCount = (SVTriggerGetParameterCountPtr)::GetProcAddress( m_hmHandle, "SVTriggerGetParameterCount" );
			m_psvGetParameterName = (SVTriggerGetParameterNamePtr)::GetProcAddress( m_hmHandle, "SVTriggerGetParameterName" );
			m_psvGetParameterValue = (SVTriggerGetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVTriggerGetParameterValue" );
			m_psvSetParameterValue = (SVTriggerSetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVTriggerSetParameterValue" );

			if ( nullptr != m_psvCreate &&
			     nullptr != m_psvDestroy &&
			     nullptr != m_psvGetCount &&
			     nullptr != m_psvGetHandle &&
			     nullptr != m_psvGetName &&
			     nullptr != m_psvRegister &&
			     nullptr != m_psvUnregister &&
			     nullptr != m_psvUnregisterAll &&
			     nullptr != m_psvStart &&
			     nullptr != m_psvStop )
			{
				l_hrOk = m_psvCreate();
			}
			else
			{
				l_hrOk = S_FALSE;
			}

		}
	}

	if ( S_OK != l_hrOk )
	{
		Close();
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr != m_hmHandle )
	{
		if ( nullptr != m_psvDestroy )
		{
			l_hrOk = m_psvDestroy();
		}

		if ( ::FreeLibrary( m_hmHandle ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			m_hmHandle = nullptr;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	m_psvCreate = nullptr;
	m_psvDestroy = nullptr;
	m_psvGetCount = nullptr;
	m_psvGetHandle = nullptr;
	m_psvGetName = nullptr;
	m_psvRegister = nullptr;
	m_psvUnregister = nullptr;
	m_psvUnregisterAll = nullptr;
	m_psvStart = nullptr;
	m_psvStop = nullptr;
	m_psvGetParameterCount = nullptr;
	m_psvGetParameterName = nullptr;
	m_psvGetParameterValue = nullptr;
	m_psvSetParameterValue = nullptr;

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetCount )
	{
		l_hrOk = m_psvGetCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetHandle )
	{
		l_hrOk = m_psvGetHandle( pTriggerchannel, p_ulIndex );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetName )
	{
		l_hrOk = m_psvGetName( triggerchannel, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::Register( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvRegister )
	{
		l_hrOk = m_psvRegister( triggerchannel, rDispatcher);
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::Unregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvUnregister )
	{
		l_hrOk = m_psvUnregister( triggerchannel, rDispatcher);
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::UnregisterAll( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvUnregisterAll )
	{
		l_hrOk = m_psvUnregisterAll( triggerchannel );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::Start( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvStart )
	{
		l_hrOk = m_psvStart( triggerchannel );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::Stop( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvStop )
	{
		l_hrOk = m_psvStop( triggerchannel );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterCount )
	{
		l_hrOk = m_psvGetParameterCount( triggerchannel, p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterName )
	{
		l_hrOk = m_psvGetParameterName( triggerchannel, p_ulIndex, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterValue )
	{
		l_hrOk = m_psvGetParameterValue( triggerchannel, p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::SetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetParameterValue )
	{
		l_hrOk = m_psvSetParameterValue( triggerchannel, p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

