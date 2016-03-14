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

