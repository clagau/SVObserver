// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOINIClass
// * .File Name       : $Workfile:   SVOINIClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:08:38  $
// ******************************************************************************


inline SVOINIClass::SVOINIClass()
{
	m_tszFile = nullptr;
}

inline SVOINIClass::~SVOINIClass()
{
	if ( nullptr != m_tszFile )
	{
		delete[] m_tszFile;
		m_tszFile = nullptr;
	}
}

inline HRESULT SVOINIClass::SetFile( LPCTSTR p_szFile )
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr != m_tszFile )
	{
		delete[] m_tszFile;
		m_tszFile = nullptr;
	}

	if ( nullptr != p_szFile )
	{
		m_tszFile = new TCHAR[ _tcslen( p_szFile ) + 1 ];
		_tcscpy( m_tszFile, p_szFile );

		if ( _taccess( p_szFile, 4 ) != 0 )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

inline HRESULT SVOINIClass::GetValue( LPCTSTR p_szSection,
                                      LPCTSTR p_szKey,
                                      LPCTSTR p_szDefault,
                                      BSTR*   p_pbstrValue,
									  LPCTSTR p_szFileName )
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == p_szFileName )
	{
		 p_szFileName = m_tszFile;
	}

	TCHAR l_szResult[ SHRT_MAX ];

	unsigned long l_ulSize = ::GetPrivateProfileString( p_szSection, p_szKey, 
	                                                    p_szDefault,
	                                                    l_szResult, sizeof l_szResult,
	                                                    p_szFileName );

	
	if ( nullptr != *p_pbstrValue )	// TRB coding error was if( p_pbstrValue
	{
		::SysFreeString( *p_pbstrValue );

		*p_pbstrValue = nullptr;
	}

	if ( ( sizeof l_szResult ) <= l_ulSize + 2 )
	{
		l_hrOk = S_FALSE;
	}
	else
	{
		CString l_csResult( l_szResult );

		*p_pbstrValue = l_csResult.AllocSysString();
	}

	return l_hrOk;
}

inline std::string SVOINIClass::GetValueString( LPCTSTR p_szSection,
                                      LPCTSTR p_szKey,
                                      LPCTSTR p_szDefault,
									  LPCTSTR p_szFileName )
{
	std::string sValue;

	HRESULT l_hrOk = S_OK;

	if ( nullptr == p_szFileName )
	{
		 p_szFileName = m_tszFile;
	}

	TCHAR l_szResult[ SHRT_MAX ];

	unsigned long l_ulSize = ::GetPrivateProfileString( p_szSection, p_szKey, 
	                                                    p_szDefault,
	                                                    l_szResult, sizeof l_szResult,
	                                                    p_szFileName );

	
	sValue = l_szResult;

	return sValue;
}

inline int SVOINIClass::GetValueInt( LPCTSTR p_szSection,
	                                  LPCTSTR p_szKey,
	                                  int     p_iDefault,
						              LPCTSTR p_szFileName )
{
	int iValue;
	GetValue(p_szSection, p_szKey, p_iDefault, &iValue, p_szFileName);
	return iValue;
}

inline HRESULT SVOINIClass::GetValue( LPCTSTR p_szSection,
	                                  LPCTSTR p_szKey,
	                                  int     p_iDefault,
	                                  int*    p_piValue,
						              LPCTSTR p_szFileName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr == p_szFileName )
	{
		p_szFileName = m_tszFile;
	}

	if ( nullptr != p_piValue )
	{
		INT iVal = ::GetPrivateProfileInt( p_szSection, p_szKey, p_iDefault, p_szFileName );

		*p_piValue = (int) iVal;

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

inline HRESULT SVOINIClass::SetValue( LPCTSTR p_szSection,
                                      LPCTSTR p_szKey,
                                      LPCTSTR p_szValue,
									  LPCTSTR p_szFileName)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr == p_szFileName )
	{
		p_szFileName = m_tszFile;
	}

	if ( ::WritePrivateProfileString( p_szSection, p_szKey, 
	                                  p_szValue, p_szFileName ) )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

inline HRESULT SVOINIClass::SetValue( LPCTSTR p_szSection,
                                      LPCTSTR p_szKey,
                                      int     p_iValue,
									  LPCTSTR p_szFileName)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr == p_szFileName )
	{
		p_szFileName = m_tszFile;
	}

	TCHAR tszValue[MAX_PATH];
	_itot(p_iValue, tszValue, 10);

	l_hrOk = SetValue( p_szSection, p_szKey, tszValue, p_szFileName );

	return l_hrOk;
}

