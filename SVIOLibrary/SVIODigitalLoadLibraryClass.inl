// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalLoadLibraryClass
// * .File Name       : $Workfile:   SVIODigitalLoadLibraryClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:44  $
// ******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline SVIODigitalLoadLibraryClass::SVIODigitalLoadLibraryClass()
{
	m_hmHandle = nullptr;

	m_psvCreate = nullptr;
	m_psvDestroy = nullptr;
	m_psvGetInputCount = nullptr;
	m_psvGetInputValue = nullptr;
	m_psvSetInputValue = nullptr;
	m_psvGetInputPortCount = nullptr;
	m_psvGetInputPortValue = nullptr;
	m_psvSetInputPortValue = nullptr;
	m_psvGetOutputCount = nullptr;
	m_psvGetOutputValue = nullptr;
	m_psvSetOutputValue = nullptr;
	m_psvGetOutputPortCount = nullptr;
	m_psvGetOutputPortValue = nullptr;
	m_psvSetOutputPortValue = nullptr;

	// Parameter Functions.
	m_psvSetParameterValue = nullptr;
	m_psvGetParameterValue = nullptr;
	m_psvGetParameterName = nullptr;
	m_psvGetParameterCount = nullptr;
}

inline SVIODigitalLoadLibraryClass::~SVIODigitalLoadLibraryClass()
{
	Close();
}

inline HRESULT SVIODigitalLoadLibraryClass::Open(LPCTSTR p_szLibrary)
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
			m_psvGetInputCount = (SVInputGetCountPtr)::GetProcAddress( m_hmHandle, "SVInputGetCount" );
			m_psvGetInputValue = (SVInputGetValuePtr)::GetProcAddress( m_hmHandle, "SVInputGetValue" );
			m_psvSetInputValue = (SVInputSetValuePtr)::GetProcAddress( m_hmHandle, "SVInputSetValue" );
			m_psvGetInputPortCount = (SVInputGetPortCountPtr)::GetProcAddress( m_hmHandle, "SVInputGetPortCount" );
			m_psvGetInputPortValue = (SVInputGetPortValuePtr)::GetProcAddress( m_hmHandle, "SVInputGetPortValue" );
			m_psvSetInputPortValue = (SVInputSetPortValuePtr)::GetProcAddress( m_hmHandle, "SVInputSetPortValue" );
			m_psvGetOutputCount = (SVOutputGetCountPtr)::GetProcAddress( m_hmHandle, "SVOutputGetCount" );
			m_psvGetOutputValue = (SVOutputGetValuePtr)::GetProcAddress( m_hmHandle, "SVOutputGetValue" );
			m_psvSetOutputValue = (SVOutputSetValuePtr)::GetProcAddress( m_hmHandle, "SVOutputSetValue" );
			m_psvGetOutputPortCount = (SVOutputGetPortCountPtr)::GetProcAddress( m_hmHandle, "SVOutputGetPortCount" );
			m_psvGetOutputPortValue = (SVOutputGetPortValuePtr)::GetProcAddress( m_hmHandle, "SVOutputGetPortValue" );
			m_psvSetOutputPortValue = (SVOutputSetPortValuePtr)::GetProcAddress( m_hmHandle, "SVOutputSetPortValue" );

			// Proc Addresses for Parameters.  These are only implemented in the SVLptIO DLL.
			m_psvSetParameterValue = (SVSetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVSetParameterValue");
			m_psvGetParameterValue = (SVGetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVGetParameterValue" );
			m_psvGetParameterName = (SVGetParameterNamePtr)::GetProcAddress( m_hmHandle, "SVGetParameterName" );
			m_psvGetParameterCount = (SVGetParameterCountPtr)::GetProcAddress( m_hmHandle, "SVGetParameterCount" );


			if ( nullptr != m_psvCreate &&
			     nullptr != m_psvDestroy &&
			     nullptr != m_psvGetInputCount &&
			     nullptr != m_psvGetInputValue &&
			     nullptr != m_psvSetInputValue &&
			     nullptr != m_psvGetInputPortCount &&
			     nullptr != m_psvGetInputPortValue &&
			     nullptr != m_psvSetInputPortValue &&
			     nullptr != m_psvGetOutputCount &&
			     nullptr != m_psvGetOutputValue &&
			     nullptr != m_psvSetOutputValue &&
			     nullptr != m_psvGetOutputPortCount &&
			     nullptr != m_psvGetOutputPortValue &&
			     nullptr != m_psvSetOutputPortValue )
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

inline HRESULT SVIODigitalLoadLibraryClass::Close()
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
	m_psvGetInputCount = nullptr;
	m_psvGetInputValue = nullptr;
	m_psvSetInputValue = nullptr;
	m_psvGetInputPortCount = nullptr;
	m_psvGetInputPortValue = nullptr;
	m_psvSetInputPortValue = nullptr;
	m_psvGetOutputCount = nullptr;
	m_psvGetOutputValue = nullptr;
	m_psvSetOutputValue = nullptr;
	m_psvGetOutputPortCount = nullptr;
	m_psvGetOutputPortValue = nullptr;
	m_psvSetOutputPortValue = nullptr;
	m_psvSetParameterValue = nullptr;

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetInputCount )
	{
		l_hrOk = m_psvGetInputCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetInputValue )
	{
		l_hrOk = m_psvGetInputValue( p_ulChannel, p_pbValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetInputValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetInputValue )
	{
		l_hrOk = m_psvSetInputValue( p_ulChannel, p_bValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetInputPortCount )
	{
		l_hrOk = m_psvGetInputPortCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetInputPortValue )
	{
		l_hrOk = m_psvGetInputPortValue( p_ulPort, p_pulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetInputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetInputPortValue )
	{
		l_hrOk = m_psvSetInputPortValue( p_ulPort, p_ulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetOutputCount )
	{
		l_hrOk = m_psvGetOutputCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetOutputValue )
	{
		l_hrOk = m_psvGetOutputValue( p_ulChannel, p_pbValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetOutputValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetOutputValue )
	{
		l_hrOk = m_psvSetOutputValue( p_ulChannel, p_bValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetOutputPortCount )
	{
		l_hrOk = m_psvGetOutputPortCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetOutputPortValue )
	{
		l_hrOk = m_psvGetOutputPortValue( p_ulPort, p_pulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetOutputPortValue )
	{
		l_hrOk = m_psvSetOutputPortValue( p_ulPort, p_ulValue );
	}

	return l_hrOk;
}

// Parameter Functions Follow.
inline HRESULT SVIODigitalLoadLibraryClass::GetParameterCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterCount )
	{
		l_hrOk = m_psvGetParameterCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterName )
	{
		l_hrOk = m_psvGetParameterName( p_ulIndex, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvGetParameterValue )
	{
		l_hrOk = m_psvGetParameterValue( p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != m_psvSetParameterValue )
	{
		l_hrOk = m_psvSetParameterValue( p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

