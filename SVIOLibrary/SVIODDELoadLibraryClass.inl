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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIODDELoadLibraryClass.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:46:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   07 Aug 2012 10:35:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Set DDE Server method to return the correct value.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   29 Oct 2007 16:01:06   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Sleep after Load and Free Library to help system resources.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   16 Mar 2004 09:21:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed SendDDEData and ReceiveDDEData return codes to S_OK rather than a BOOL true when the function succeeded.
   
   /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Mar 2004 08:57:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendDDEData and ReceiveDDEData to return S_OK rather than a BOOL true.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
