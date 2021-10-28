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
#include "SVSecurityManager.h"
#include "SVAccessClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


SVSecurityManager g_theSecurityManager;

SVSecurityManager& TheSecurityManager()
{
	return g_theSecurityManager;
}	// Security

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

HRESULT SVSecurityManager::SVLoad( LPCTSTR FileName )
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->Load( FileName );
	}
	
	return E_FAIL;
}

HRESULT SVSecurityManager::SVCreateProcess( LPCTSTR AppName, LPCTSTR Path, LPCTSTR Command  )
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->CreateProcess( AppName, Path, Command );
	}

	return E_FAIL;
}

HRESULT SVSecurityManager::SVAdd( long lID, LPCTSTR Name, LPCTSTR NTGroup /*= nullptr*/, bool ForcePrompt /*= false*/ )
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->Add( lID, Name, NTGroup, ForcePrompt);
	}

	return E_FAIL;
}

HRESULT SVSecurityManager::SVProtectData( long lID )
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->ProtectData( lID );
	}

	return E_FAIL;
}

bool SVSecurityManager::SVGetUseLogon()
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->GetUseLogon();
	}

	return false;
}


bool SVSecurityManager::SVIsLoggedOn()
{
	if( nullptr != m_pSVAccess )
	{
		return m_pSVAccess->IsLoggedOn();
	}

	return false;
}

HRESULT SVSecurityManager::SVGetUser( BSTR& p_rbstrUser )
{
	HRESULT l_hr = S_OK;


	if( nullptr != m_pSVAccess )
	{
		if( !m_pSVAccess->TimeExpired() )
		{
			if( !m_pSVAccess->GetCurrentUser().empty() )
			{
				p_rbstrUser = _bstr_t( m_pSVAccess->GetCurrentUser().c_str() ) ;
			}
		}
	}
	return l_hr;
}

