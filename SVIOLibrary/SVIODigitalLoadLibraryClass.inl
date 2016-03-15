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
	m_hmHandle = NULL;

	m_psvCreate = NULL;
	m_psvDestroy = NULL;
	m_psvGetInputCount = NULL;
	m_psvGetInputValue = NULL;
	m_psvSetInputValue = NULL;
	m_psvGetInputPortCount = NULL;
	m_psvGetInputPortValue = NULL;
	m_psvSetInputPortValue = NULL;
	m_psvGetOutputCount = NULL;
	m_psvGetOutputValue = NULL;
	m_psvSetOutputValue = NULL;
	m_psvGetOutputPortCount = NULL;
	m_psvGetOutputPortValue = NULL;
	m_psvSetOutputPortValue = NULL;

	// Parameter Functions.
	m_psvSetParameterValue = NULL;
	m_psvGetParameterValue = NULL;
	m_psvGetParameterName = NULL;
	m_psvGetParameterCount = NULL;
}

inline SVIODigitalLoadLibraryClass::~SVIODigitalLoadLibraryClass()
{
	Close();
}

inline HRESULT SVIODigitalLoadLibraryClass::Open(LPCTSTR p_szLibrary)
{
	HRESULT l_hrOk = S_OK;

	if ( m_hmHandle == NULL )
	{
		m_hmHandle = ::LoadLibrary( p_szLibrary );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( m_hmHandle != NULL )
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

			// Proc Addresses for Parameters.  These are only implemented in TVicLpt DLLs.
			m_psvSetParameterValue = (SVSetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVSetParameterValue");
			m_psvGetParameterValue = (SVGetParameterValuePtr)::GetProcAddress( m_hmHandle, "SVGetParameterValue" );
			m_psvGetParameterName = (SVGetParameterNamePtr)::GetProcAddress( m_hmHandle, "SVGetParameterName" );
			m_psvGetParameterCount = (SVGetParameterCountPtr)::GetProcAddress( m_hmHandle, "SVGetParameterCount" );


			if ( m_psvCreate != NULL &&
			     m_psvDestroy != NULL &&
			     m_psvGetInputCount != NULL &&
			     m_psvGetInputValue != NULL &&
			     m_psvSetInputValue != NULL &&
			     m_psvGetInputPortCount != NULL &&
			     m_psvGetInputPortValue != NULL &&
			     m_psvSetInputPortValue != NULL &&
			     m_psvGetOutputCount != NULL &&
			     m_psvGetOutputValue != NULL &&
			     m_psvSetOutputValue != NULL &&
			     m_psvGetOutputPortCount != NULL &&
			     m_psvGetOutputPortValue != NULL &&
			     m_psvSetOutputPortValue != NULL )
			{
				l_hrOk = m_psvCreate();
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

inline HRESULT SVIODigitalLoadLibraryClass::Close()
{
	HRESULT l_hrOk = S_OK;

	if ( m_hmHandle != NULL )
	{
		if ( m_psvDestroy != NULL )
		{
			l_hrOk = m_psvDestroy();
		}

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

	m_psvCreate = NULL;
	m_psvDestroy = NULL;
	m_psvGetInputCount = NULL;
	m_psvGetInputValue = NULL;
	m_psvSetInputValue = NULL;
	m_psvGetInputPortCount = NULL;
	m_psvGetInputPortValue = NULL;
	m_psvSetInputPortValue = NULL;
	m_psvGetOutputCount = NULL;
	m_psvGetOutputValue = NULL;
	m_psvSetOutputValue = NULL;
	m_psvGetOutputPortCount = NULL;
	m_psvGetOutputPortValue = NULL;
	m_psvSetOutputPortValue = NULL;
	m_psvSetParameterValue = NULL;

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetInputCount != NULL )
	{
		l_hrOk = m_psvGetInputCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetInputValue != NULL )
	{
		l_hrOk = m_psvGetInputValue( p_ulChannel, p_pbValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetInputValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetInputValue != NULL )
	{
		l_hrOk = m_psvSetInputValue( p_ulChannel, p_bValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetInputPortCount != NULL )
	{
		l_hrOk = m_psvGetInputPortCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetInputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetInputPortValue != NULL )
	{
		l_hrOk = m_psvGetInputPortValue( p_ulPort, p_pulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetInputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetInputPortValue != NULL )
	{
		l_hrOk = m_psvSetInputPortValue( p_ulPort, p_ulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetOutputCount != NULL )
	{
		l_hrOk = m_psvGetOutputCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetOutputValue != NULL )
	{
		l_hrOk = m_psvGetOutputValue( p_ulChannel, p_pbValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetOutputValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetOutputValue != NULL )
	{
		l_hrOk = m_psvSetOutputValue( p_ulChannel, p_bValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetOutputPortCount != NULL )
	{
		l_hrOk = m_psvGetOutputPortCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetOutputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetOutputPortValue != NULL )
	{
		l_hrOk = m_psvGetOutputPortValue( p_ulPort, p_pulValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetOutputPortValue != NULL )
	{
		l_hrOk = m_psvSetOutputPortValue( p_ulPort, p_ulValue );
	}

	return l_hrOk;
}

// Parameter Functions Follow.
inline HRESULT SVIODigitalLoadLibraryClass::GetParameterCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterCount != NULL )
	{
		l_hrOk = m_psvGetParameterCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterName != NULL )
	{
		l_hrOk = m_psvGetParameterName( p_ulIndex, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterValue != NULL )
	{
		l_hrOk = m_psvGetParameterValue( p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

inline HRESULT SVIODigitalLoadLibraryClass::SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetParameterValue != NULL )
	{
		l_hrOk = m_psvSetParameterValue( p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

