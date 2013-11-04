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

#include <io.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline SVOINIClass::SVOINIClass()
{
	m_tszFile = NULL;
}

inline SVOINIClass::~SVOINIClass()
{
	if ( m_tszFile != NULL )
	{
		delete[] m_tszFile;
		m_tszFile = NULL;
	}
}

inline HRESULT SVOINIClass::SetFile( LPCTSTR p_szFile )
{
	HRESULT l_hrOk = S_OK;

	if ( m_tszFile != NULL )
	{
		delete[] m_tszFile;
		m_tszFile = NULL;
	}

	if ( p_szFile != NULL )
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

	if ( p_szFileName == NULL )
	{
		 p_szFileName = m_tszFile;
	}

	TCHAR l_szResult[ SHRT_MAX ];

	unsigned long l_ulSize = ::GetPrivateProfileString( p_szSection, p_szKey, 
	                                                    p_szDefault,
	                                                    l_szResult, sizeof l_szResult,
	                                                    p_szFileName );

	
	if ( *p_pbstrValue != NULL )	// TRB coding error was if( p_pbstrValue
	{
		::SysFreeString( *p_pbstrValue );

		*p_pbstrValue = NULL;
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

	if ( p_szFileName == NULL )
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

	if ( p_szFileName == NULL )
	{
		p_szFileName = m_tszFile;
	}

	if ( p_piValue != NULL )
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

	if ( p_szFileName == NULL )
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

	if ( p_szFileName == NULL )
	{
		p_szFileName = m_tszFile;
	}

	TCHAR tszValue[MAX_PATH];
	_itot(p_iValue, tszValue, 10);

	l_hrOk = SetValue( p_szSection, p_szKey, tszValue, p_szFileName );

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOINIClass.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:08:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Oct 2007 16:21:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed pointer error in GetValue - programmer forgot to de-reference BSTR pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Jul 2005 14:11:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetValueString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Jul 2004 11:27:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetValueInt
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Jul 2003 08:30:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for int type
 * added SetFile
 * made file name optional for gets and sets ( to work with SetFile)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 15:04:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed PVCS Log Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2003 14:58:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed PVCS Log Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *   Rev 1.2   08 Jul 2003 14:56:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *  Fixed PVCS Log Data.
 *
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *   Rev 1.1   08 Jul 2003 14:50:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *  Updated PVCS log information.
 *
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *   Rev 1.0   08 Jul 2003 13:13:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *  New files needed to handle callback functionality and parameter functionality.
 *
 * /////////////////////////////////////////////////////////////////////////////////////
*/