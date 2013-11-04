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
	m_hmHandle = NULL;

	m_psvCreate = NULL;
	m_psvDestroy = NULL;
	m_psvGetCount = NULL;
	m_psvGetHandle = NULL;
	m_psvGetName = NULL;
	m_psvRegister = NULL;
	m_psvUnregister = NULL;
	m_psvUnregisterAll = NULL;
	m_psvStart = NULL;
	m_psvStop = NULL;
	m_psvGetParameterCount = NULL;
	m_psvGetParameterName = NULL;
	m_psvGetParameterValue = NULL;
	m_psvSetParameterValue = NULL;
}

inline SVIOTriggerLoadLibraryClass::~SVIOTriggerLoadLibraryClass()
{
	Close();
}

inline HRESULT SVIOTriggerLoadLibraryClass::Open(LPCTSTR p_szLibrary)
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

			if ( m_psvCreate != NULL &&
			     m_psvDestroy != NULL &&
			     m_psvGetCount != NULL &&
			     m_psvGetHandle != NULL &&
			     m_psvGetName != NULL &&
			     m_psvRegister != NULL &&
			     m_psvUnregister != NULL &&
			     m_psvUnregisterAll != NULL &&
			     m_psvStart != NULL &&
			     m_psvStop != NULL )
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

inline HRESULT SVIOTriggerLoadLibraryClass::Close()
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
	m_psvGetCount = NULL;
	m_psvGetHandle = NULL;
	m_psvGetName = NULL;
	m_psvRegister = NULL;
	m_psvUnregister = NULL;
	m_psvUnregisterAll = NULL;
	m_psvStart = NULL;
	m_psvStop = NULL;
	m_psvGetParameterCount = NULL;
	m_psvGetParameterName = NULL;
	m_psvGetParameterValue = NULL;
	m_psvSetParameterValue = NULL;

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetCount != NULL )
	{
		l_hrOk = m_psvGetCount( p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetHandle != NULL )
	{
		l_hrOk = m_psvGetHandle( p_pulHandle, p_ulIndex );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetName != NULL )
	{
		l_hrOk = m_psvGetName( p_ulHandle, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::Register( unsigned long p_ulHandle, SVCallbackStruct &p_rCallback )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvRegister != NULL )
	{
		l_hrOk = m_psvRegister( p_ulHandle, p_rCallback.m_pCallback, p_rCallback.m_pOwner, p_rCallback.m_pData );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::Unregister( unsigned long p_ulHandle, SVCallbackStruct &p_rCallback )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvUnregister != NULL )
	{
		l_hrOk = m_psvUnregister( p_ulHandle, p_rCallback.m_pCallback, p_rCallback.m_pOwner, p_rCallback.m_pData );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::UnregisterAll( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvUnregisterAll != NULL )
	{
		l_hrOk = m_psvUnregisterAll( p_ulHandle );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::Start( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvStart != NULL )
	{
		l_hrOk = m_psvStart( p_ulHandle );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::Stop( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvStop != NULL )
	{
		l_hrOk = m_psvStop( p_ulHandle );
	}

	return l_hrOk;
}
	
inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterCount != NULL )
	{
		l_hrOk = m_psvGetParameterCount( p_ulHandle, p_pulCount );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterName != NULL )
	{
		l_hrOk = m_psvGetParameterName( p_ulHandle, p_ulIndex, p_pbstrName );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::GetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvGetParameterValue != NULL )
	{
		l_hrOk = m_psvGetParameterValue( p_ulHandle, p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

inline HRESULT SVIOTriggerLoadLibraryClass::SetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( m_psvSetParameterValue != NULL )
	{
		l_hrOk = m_psvSetParameterValue( p_ulHandle, p_ulIndex, p_pvarValue );
	}

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOTriggerLoadLibraryClass.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:48:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   29 Oct 2007 16:01:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Sleep after Load and Free Library to help system resources.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   16 Jan 2004 07:19:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added functions to allow configuring IO to work with special SVIM hardware
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   17 Oct 2003 12:57:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated exposed function prototypes to change parameters from a class to individual items.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/