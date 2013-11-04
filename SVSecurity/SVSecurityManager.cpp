//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityManager
//* .File Name       : $Workfile:   SVSecurityManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:12  $
//******************************************************************************

// SVSecurityManager.cpp: implementation of the SVSecurityManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "svsecurity.h"
#include "SVSecurityManager.h"
#include "SVAccessClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVSecurityManager::SVSecurityManager()
{
	m_pSVAccess = new SVAccessClass;
}

SVSecurityManager::~SVSecurityManager()
{
	if( m_pSVAccess )
		delete m_pSVAccess;
}


HRESULT SVSecurityManager::SVValidate( long lId )
{
	if( m_pSVAccess )
		return m_pSVAccess->Validate( lId );
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVValidate( long lId1, long lId2 )
{
	if( m_pSVAccess )
		return m_pSVAccess->Validate( lId1, lId2 );
	else
		return S_FALSE;
}



bool SVSecurityManager::SVIsDisplayable( long lId )
{
	if( m_pSVAccess )
		return m_pSVAccess->IsDisplayable(lId);
	else
		return false;
}

bool SVSecurityManager::SVIsSecured( long lId )
{
	if( m_pSVAccess )
		return m_pSVAccess->SVIsSecured( lId );
	else
		return true;
}

int SVSecurityManager::SVSetupDialog()
{
	if( m_pSVAccess )
		return m_pSVAccess->SetupDialog();
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVLogon()
{
	if( m_pSVAccess )
		return m_pSVAccess->Logon();
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVLogout()
{
	if( m_pSVAccess )
		return m_pSVAccess->Logout();
	else
		return S_FALSE;
}

// Ansi char functions....
HRESULT SVSecurityManager::SVLoad(const char* const pFileName)
{
	if( m_pSVAccess )
		return m_pSVAccess->Load( pFileName );
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVCreateProcess( const char* const strAppName, const char* const strPath, const char* const strCommand  )
{
	if( m_pSVAccess )
		return m_pSVAccess->CreateProcess( strAppName, strPath, strCommand );
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVAdd( long lID, const char* const sName, const char* const sGroup , bool bForcePrompt )
{
	if( m_pSVAccess )
		return m_pSVAccess->Add( lID, sName, sGroup, bForcePrompt);
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVAdd( long lID, const char* const sName )
{
	if( m_pSVAccess )
		return m_pSVAccess->Add( lID, sName);
	else
		return S_FALSE;
}
// Wide / Unicode functions...
HRESULT SVSecurityManager::SVLoad(const wchar_t* const pFileName)
{
	if( m_pSVAccess )
		return m_pSVAccess->Load( pFileName );
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVCreateProcess( const wchar_t* const strAppName, const wchar_t* const strPath, const wchar_t* const strCommand  )
{
	if( m_pSVAccess )
		return m_pSVAccess->CreateProcess( strAppName, strPath, strCommand );
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVAdd( long lID, const wchar_t* const sName, const wchar_t* const sGroup , bool bForcePrompt )
{
	if( m_pSVAccess )
		return m_pSVAccess->Add( lID, sName, sGroup, bForcePrompt);
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVAdd( long lID, const wchar_t* const sName )
{
	if( m_pSVAccess )
		return m_pSVAccess->Add( lID, sName);
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVProtectData( long lID )
{
	if( m_pSVAccess )
		return m_pSVAccess->ProtectData( lID );
	else
		return S_FALSE;
}

bool SVSecurityManager::SVGetUseLogon()
{
	if( m_pSVAccess )
		return m_pSVAccess->GetUseLogon();
	else
		return S_FALSE;
}


bool SVSecurityManager::SVIsLoggedOn()
{
	if( m_pSVAccess )
		return m_pSVAccess->IsLoggedOn();
	else
		return S_FALSE;
}

HRESULT SVSecurityManager::SVGetUser( BSTR& p_rbstrUser )
{
	HRESULT l_hr = S_OK;


	if( m_pSVAccess )
	{
		if( !m_pSVAccess->TimeExpired() )
		{
			CString l_cstrCurrentUser;

			l_cstrCurrentUser = m_pSVAccess->GetCurrentUser();

			if( !l_cstrCurrentUser.IsEmpty() )
			{
				p_rbstrUser = l_cstrCurrentUser.AllocSysString ();
			}
		}
	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecurityManager.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:03:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Oct 2007 15:28:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added test to prevent allocating an empty BSTR in SVGetUser.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   23 Mar 2006 08:46:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  558
 * SCR Title:  Expand Code in SVSecurity DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVGetUser.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Aug 2005 10:46:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Dual Access Point Validation(
 * Changed name of IsValid( to IsDisplayable(.
 * Added IsUserAMember(
 * IsCurrentUserValidated(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Aug 2005 11:40:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Bug with SVIsAllowed Interface
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Aug 2005 08:28:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetUser( BSTR bstName )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jul 2005 11:07:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified CreateProcess to search the current path.
 * Added IsSecured( ) to check if security is being used for this access point.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jul 2005 14:55:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Changes - Removed Auto Offline to Edit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Jul 2005 13:49:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:08:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/