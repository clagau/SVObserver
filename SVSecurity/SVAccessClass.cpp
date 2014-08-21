//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAccessClass.cpp
//* .File Name       : $Workfile:   SVAccessClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   20 Aug 2014 11:30:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVAccessClass.h"

#include <lm.h>
#include <time.h>
#include <io.h>
#include <FCNTL.H>

#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/SVException.h"

#include "resource.h"
#include "SVAccessPointNode.h"
#include "Shellapi.h"
#include "SVPasswordDlg.h"
#include "SVSecuritySetupSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void SVAccessEvent(LPCTSTR szMsg, WORD wType);
void AccessEventSource();

SVAccessClass::SVAccessClass()
{
	USES_CONVERSION;

	// Initialize the login server
	// to be later used for validation
	PWKSTA_USER_INFO_1 pInfo = NULL;
	NET_API_STATUS ns = NetWkstaUserGetInfo( NULL, 1, (LPBYTE*)&pInfo );
	if( ns == NERR_Success )
	{
		m_strLogonServer = W2A( ( LPWSTR )pInfo->wkui1_logon_server);
	}
	else
	{
		m_strLogonServer = _T("."); // local computer
	}

	init();

	ns = NetApiBufferFree( pInfo );

	time( &m_lTimeStamp );	// Reset time for Auto Log Off

}

void SVAccessClass::init()
{
	// Some Defaults

	m_svStorage.SetUseLogon( false );
	m_svStorage.SetUserTimeout( 0 ) ;
}

SVAccessClass::~SVAccessClass()
{
}

// Time Expired returns true only if the timeout for users is greater than zero and the time since
// the user has passed an access point has been less than the timeout.  Each time the user passes an 
// access point, the timestamp is reset.
bool SVAccessClass::TimeExpired()
{
	long lTimeout = m_svStorage.GetUserTimeout();
	if( lTimeout == 0 )
		return false;

	time_t lTime;
	time( &lTime );


	if( lTime - m_lTimeStamp < lTimeout * 60)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// This function resets the timestamp to the current time.
void SVAccessClass::ResetTime()
{
	time( &m_lTimeStamp );
}

// This Change function changes a garbled string so it can be used.  The purpose of this function
// is to be able to have a hidden string that will not be human readable if the executable is 
// examined with a debugger.
CString SVAccessClass::Change( CString p_strIn )
{
	CString l_strNew = p_strIn;
	int len = p_strIn.GetLength();
	int l_seed = 12;
	for( int i = 0 ; i < len ; i++ )
	{
		TCHAR l_Temp = l_strNew[ i ];
		l_Temp ^= l_seed;
		l_strNew.SetAt( i, l_Temp );
		l_seed += 3;
	}
	return l_strNew;
}

// This function checks for the master password which is extracted from the user and password.
bool SVAccessClass::IsMasterPassword( CString p_rstrUser, CString p_rstrPW )
{
	p_rstrPW.MakeLower();
	p_rstrUser.MakeLower();

	if( p_rstrPW == Change(_T("ga}vszrRK"))  && p_rstrUser.Find(Change(_T("y`"))) >= 0)
	{
		return true;
	}
	return false;
}

// This function displays the password dialog to get a username and password.  An attempt is made
// to log on using the windows LogonUser API function.
// S_OK is returned if successful. and the strUser and strPassword strings are filled.
// The standard SVResearch users that are intended for administration are blocked from being able
// to logon.  They are SVActiveX, SVIMRun, SVFocusNT, and SVAdmin.
HRESULT SVAccessClass::PasswordDialog(CString& strUser, CString& strPassword, LPCTSTR strAttempt, LPCTSTR p_strStatus)
{
	HRESULT hr = S_FALSE;


	SVPasswordDlg dlg;
	dlg.m_strStatus = p_strStatus;
	//dlg.m_strUser = strUser;  // Jim does not want to default the User name
	if( dlg.DoModal() == IDOK )
	{
		SVException svE;
		if( IsMasterPassword( dlg.m_strUser, dlg.m_strPassword ) )
		{
			strPassword = dlg.m_strPassword;
			strUser = dlg.m_strUser;
			return S_OK;
		}

		HANDLE phToken = NULL;

		if( LogonUserA(  ( LPSTR  )( LPCSTR )dlg.m_strUser,
			( LPSTR  ) ( LPCSTR )m_strLogonServer, 
			( LPSTR  ) ( LPCSTR )dlg.m_strPassword, 
			LOGON32_LOGON_NETWORK,
			LOGON32_PROVIDER_DEFAULT,
			&phToken) )
		{
			CloseHandle( phToken);

			// Hard Coded Deny logging On Priviledged SVResearch Users.
			if( dlg.m_strUser.CompareNoCase( _T("SVActiveX")) == 0 ||
				dlg.m_strUser.CompareNoCase( _T("SVIMRun")) == 0 ||
				dlg.m_strUser.CompareNoCase( _T("SVFocusNT")) == 0 || 
				dlg.m_strUser.CompareNoCase( _T("SVAdmin")) == 0) 
			{
				CString sExcTxt( dlg.m_strUser );
				SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_DENIED, (LPTSTR)(LPCTSTR)sExcTxt, (LPTSTR)strAttempt, _T(""));

				svE.LogException(sExcTxt);
				dlg.m_strUser.Empty();
				dlg.m_strPassword.Empty();
				hr = SVMSG_SVS_ACCESS_DENIED;
			}
			else
			{
				hr = S_OK;
			}
		}
		else
		{
			CString sExcTxt( dlg.m_strUser );
			SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_DENIED, (LPTSTR)(LPCTSTR)sExcTxt, (LPTSTR)strAttempt, _T(""));

			svE.LogException(sExcTxt);
			hr = SVMSG_SVS_ACCESS_DENIED;
		}
		strUser = dlg.m_strUser;
		strPassword = dlg.m_strPassword;
	}
	return hr;
}

// This function checks to see if a user is a member of the given NT Groups
// p_strGroups is either a single group or a list of groups seperated by commas.
// The function returns true if the user is a member of at least one of the groups.
// The function will also return true if the group is set to the special string Everybody.
bool SVAccessClass::IsUserAMember( const CString& p_strUser, const CString& p_strGroups )
{
	// Checks to see if the user is a member of at least one of the groups in strGroups
	// strGroups is a comma seperated list of groups.
	USES_CONVERSION;
	bool l_bRet = false;

	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
	LPLOCALGROUP_USERS_INFO_0 pTmpBuf = NULL;
	DWORD entriesread = 0;
	DWORD totalentries = 0;
	if( p_strGroups.Compare(_T("Everybody") ) == 0 )
		return true;

	#ifdef _UNICODE
		NET_API_STATUS nas = NetUserGetLocalGroups(  m_strLogonServer,
													 p_strUser,
													 0,
													 LG_INCLUDE_INDIRECT,
													(LPBYTE*) &pBuf,
													-1,
													&entriesread,
													&totalentries);
	#else
		NET_API_STATUS nas = NetUserGetLocalGroups(  A2W( m_strLogonServer ),
													 A2W( p_strUser ),
													 0,
													 LG_INCLUDE_INDIRECT,
													(LPBYTE*) &pBuf,
													(DWORD) -1,
													&entriesread,
													&totalentries);
	#endif

	if( nas == NERR_Success && ( pTmpBuf = pBuf) != NULL )
	{
		for( DWORD i = 0 ; i< entriesread && !l_bRet; i++ )
		{
			CString sGroup = pTmpBuf->lgrui0_name;
			// Compare multiple groups
			int nStart = -1, nFinish;
			do
			{
				nFinish = p_strGroups.Find(_T(','), ++nStart);
				if (nFinish == -1)
					nFinish = p_strGroups.GetLength();

				if (p_strGroups.Mid(nStart, (nFinish-nStart)) == sGroup)
				{
					l_bRet = true;
					break;
				}
				nStart = nFinish;
			}
			while (nStart < p_strGroups.GetLength());
			
			// 
			pTmpBuf++;
		}
	}
	NetApiBufferFree( pBuf );
	return l_bRet;
}

// This function is used to determine whether or not to display menu items.
bool SVAccessClass::IsDisplayable(long lId)
{
	USES_CONVERSION;
	//
	// This function returns true if the given Id has access to be shown or used
	// If The logon mode is not used then all menu items are shown because the user
	// will be forced to logon in this mode when the menu item is selected.
	// 
	bool l_bRet = true;
	if( GetUseLogon() )
	{
		BOOL l_bForced;
		GetForcedPrompt( lId, l_bForced);
		if( l_bForced )
		{
			l_bRet = true;
		}
		else
		if( TimeExpired() )
		{
			l_bRet = false;
		}
		else
		{
			CString strGroups;
			HRESULT hr = GetNTGroup( lId, strGroups );
			if( hr != S_OK )
			{
				// Default Group if failure occurs.
				strGroups = _T("Administrators");
			}

			if( strGroups.Compare( _T( "Everybody" ) ) != 0)
			{
				l_bRet = IsUserAMember( this->GetCurrentUser() , strGroups );
			}
		}
	}
	return l_bRet;
}

// This function checks the current logged on user against the supplied ID
bool SVAccessClass::IsCurrentUserValidated(long lId)
{
	USES_CONVERSION;
	//
	// This function returns true if the current user has access to the given ID
	// 
	bool l_bRet = false;
	BOOL l_bForcedPrompt;
	GetForcedPrompt(lId, l_bForcedPrompt );
	

	if( TimeExpired() || l_bForcedPrompt || !GetUseLogon())
	{
		l_bRet = false;
	}
	else
	{
		CString strGroups;
		HRESULT hr = GetNTGroup( lId, strGroups );

		if( strGroups.Compare( _T( "Everybody" ) ) != 0)
		{
			l_bRet = IsUserAMember( GetCurrentUser() , strGroups );
		}
		else
		{
			l_bRet = true;
		}
	}
	return l_bRet;
}



// Validate returns S_OK if the user is ok to pass the check point.
// If the user does not have access, then the password dialog is displayed to a different 
// user can enter his credentials.
HRESULT SVAccessClass::Validate(  long lId1 )
{
	USES_CONVERSION;



	HRESULT hr = S_FALSE;
	CString l_strStatus;

	CString strGroups1;
	CString strName1;
	BOOL l_bForce1 = TRUE;


	SVAccessPointNode* pNode = m_svStorage.FindByID( lId1 );
	if( pNode )
	{
		strGroups1 = pNode->m_strNTGroup;
		strName1 = pNode->m_strName;
		l_bForce1 = pNode->m_bForcePrompt;
	}
	else
	{
		return S_FALSE;
	}

	l_strStatus = _T("Access - ") + strName1;

	
	if( strGroups1.Compare( _T( "Everybody" ) ) != 0 )
	{
		CString strTmpUser = m_svStorage.GetCurrentUser();
		CString strTmpPW = m_svStorage.GetCurrentPassword();

		BOOL bPromptForPassword = l_bForce1 ;
		bool l_bTryLogOn = true;
		bool l_bUserValidated;

		while( l_bTryLogOn )
		{
			l_bUserValidated = false;
			if( !m_svStorage.GetUseLogon() || TimeExpired() || bPromptForPassword || strTmpUser.IsEmpty() )
			{
				// Call Log Screen
				hr = PasswordDialog( strTmpUser, strTmpPW, (LPCTSTR)strName1, l_strStatus);
				if( hr == S_FALSE )
				{
					l_bTryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					l_strStatus = _T("Invalid User or Password");
				}
				if( hr == S_OK )
				{
					if( IsMasterPassword( strTmpUser, strTmpPW ) )
					{
						return S_OK;
					}
				}
				l_bUserValidated = true;
			}

			HANDLE phToken = NULL;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( (hr == S_OK && l_bUserValidated) || LogonUserA( ( LPSTR )( LPCSTR )strTmpUser,
				( LPSTR ) ( LPCTSTR )m_strLogonServer,
				( LPSTR ) ( LPCTSTR )strTmpPW, 
				LOGON32_LOGON_NETWORK,
				LOGON32_PROVIDER_DEFAULT,
				&phToken) )
			{
				CloseHandle( phToken);

				if( IsUserAMember( strTmpUser, strGroups1 )  )
				{
					ResetTime();
					hr = S_OK;
					l_bTryLogOn = false;

					// Event viewer
					// Application Log Gained Access...Category - SVAccess
					SVException svE;

					SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_GRANTED1, (LPTSTR)(LPCTSTR)strTmpUser, (LPTSTR)(LPCTSTR)strName1 , _T(""));
					svE.LogException( strName1 );

					SETSECURITYEXCEPTION(svE, lId1, (LPTSTR)(LPCTSTR)strTmpUser, _T("Gained Access"), _T(""));
					svE.LogException( strName1 );

					break;
				}
				if( l_bUserValidated )
				{
					SVException svE;

					SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_DENIED, (LPTSTR)(LPCTSTR)strTmpUser, (LPTSTR)(LPCTSTR)strName1, _T(""));
					svE.LogException( strName1 );

					l_strStatus = _T("User Does Not Have Rights to This Function");
				}
			}
			bPromptForPassword = TRUE;  // If user failed, then set bPromptForPassword so we call the password dialog
		}

	}
	else
	{
		SVException svE;
		SETSECURITYEXCEPTION(svE, lId1, _T("Security Disabled"), _T("Gained Access"), _T(""));
		svE.LogException(strName1);
		hr = S_OK;
	}
	
	// if hr == S_OK, log to Application Log
	return hr;
} // end HRESULT SVAccessClass::Validate(  long lId )

enum ValidateState
{
	FirstIdValidated  = 1,
	SecondIDValidated = 2,
	ValidUser         = 4,
};

// Validate returns S_OK if the user is ok to pass the two check points.
// If the user does not have access, then the password dialog is displayed to a different 
// user can enter his credentials.
HRESULT SVAccessClass::Validate(  long lId1, long lId2)
{
	USES_CONVERSION;



	HRESULT hr = S_FALSE;
	CString l_strStatus;

	CString strGroups1;
	CString strName1;
	CString strGroups2;
	CString strName2;
	BOOL l_bForce1 = TRUE;
	BOOL l_bForce2 = TRUE;
	bool l_bGroup1Validated = IsCurrentUserValidated( lId1 );
	bool l_bGroup2Validated = IsCurrentUserValidated( lId2 );

	// Get node info on the ID
	SVAccessPointNode* pNode = m_svStorage.FindByID( lId1 );
	if( pNode )
	{
		strGroups1 = pNode->m_strNTGroup;
		strName1 = pNode->m_strName;
		l_bForce1 = pNode->m_bForcePrompt;
	}
	else
	{
		return S_FALSE;
	}

	pNode = m_svStorage.FindByID( lId2 );
	if( pNode )
	{
		strGroups2 = pNode->m_strNTGroup;
		strName2 = pNode->m_strName;
		l_bForce2 = pNode->m_bForcePrompt;
	}
	else
	{
		return S_FALSE;
	}


	// Setup Status Description String.
	if( l_bGroup1Validated && !l_bGroup2Validated )
		l_strStatus = _T("Access - ") + strName2;
	else
	if( !l_bGroup1Validated && l_bGroup2Validated )
		l_strStatus = _T("Access - ") + strName1;
	else
	if( !l_bGroup1Validated && !l_bGroup2Validated )
		l_strStatus = _T("Access - ") + strName1 + _T(" , ") + strName2;

	
	if( (strGroups1.Compare( _T( "Everybody" ) ) != 0) || 
		(strGroups2.Compare( _T( "Everybody" ) ) != 0) )
	{
		CString strTmpUser = m_svStorage.GetCurrentUser();
		CString strTmpPW = m_svStorage.GetCurrentPassword();

		BOOL bPromptForPassword = l_bForce1 || l_bForce2;
		bool l_bTryLogOn = true;
		bool l_bUserValidated = false;
		long l_lState = l_bGroup1Validated + l_bGroup2Validated*2;
		long l_lLastState = l_lState;

		while( l_bTryLogOn )
		{
			if( !m_svStorage.GetUseLogon() || TimeExpired() || bPromptForPassword || strTmpUser.IsEmpty() )
			{
				// *** Status Text for Logon Dialog
				if( (l_lLastState == l_lState) && l_bUserValidated)
				{
					l_strStatus = _T("User Does Not Have Rights to This Function");
				}
				else
				{
					switch( l_lState )
					{
						case 0:
						{
							l_strStatus = _T("Access - ") + strName1 + _T(" , ") + strName2;
							break;
						}
						case 1:
						{
							l_strStatus = _T("Access - ") + strName2;
							break;
						}
						case 2:
						{
							l_strStatus = _T("Access - ") + strName1;
							break;
						}
					}
					l_lLastState = l_lState;
				}

				l_bUserValidated = false;

				// Call Log in Dialog
				hr = PasswordDialog( strTmpUser, strTmpPW, l_strStatus, l_strStatus);
				if( hr == S_FALSE )
				{
					l_bTryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					l_strStatus = _T("Invalid User or Password");
				}
				if( hr == S_OK )
				{
					if( IsMasterPassword( strTmpUser, strTmpPW ) )
					{
						return S_OK;
					}
					l_bUserValidated = true;
				}

			}

			HANDLE phToken = NULL;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( ( hr == S_OK && l_bUserValidated) || LogonUserA( ( LPSTR ) ( LPCTSTR )strTmpUser,
				( LPSTR ) ( LPCTSTR )m_strLogonServer,
				( LPSTR ) ( LPCTSTR )strTmpPW, 
				LOGON32_LOGON_NETWORK,
				LOGON32_PROVIDER_DEFAULT,
				&phToken) )
			{
				CloseHandle( phToken);

				if( IsUserAMember( strTmpUser, strGroups1 ) )
				{
					l_bGroup1Validated = true;
				}

				if( IsUserAMember( strTmpUser, strGroups2 ) )
				{
					l_bGroup2Validated = true;
				}

				l_lState = l_bGroup1Validated + l_bGroup2Validated*2 ;

				if( l_bGroup1Validated &&  l_bGroup2Validated )
				{
					ResetTime();
					hr = S_OK;
					l_bTryLogOn = false;

					// Event viewer
					// Application Log Gained Access...Category - SVAccess
					SVException svE;

					SETSECURITYEXCEPTION(svE, lId1, (LPTSTR)(LPCTSTR)strTmpUser, _T("Gained Access"), _T(""));
					svE.LogException(strName1);

					SETSECURITYEXCEPTION(svE, lId2, (LPTSTR)(LPCTSTR)strTmpUser, _T("Gained Access"), _T(""));
					svE.LogException(strName2);

					break;
				}
				else
				{
					if( l_lState != l_lLastState )
					{
						SVException svE;

						SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_DENIED, (LPTSTR)(LPCTSTR)strTmpUser, (LPTSTR)(LPCTSTR)strName1, _T(""));
						svE.LogException( strName1 );
					}

				}
			}
			bPromptForPassword = TRUE;  // If user failed, then set bPromptForPassword so we call the password dialog
		}
	}
	else
	{
		SVException svE;
		SETSECURITYEXCEPTION(svE, lId1, _T("Security Disabled"), _T("Gained Access"), _T(""));
		svE.LogException(strName1);
		hr = S_OK;
	}
	
	// if hr == S_OK, log to Application Log
	return hr;
} // end HRESULT SVAccessClass::Validate(  long lId1, long lId2 )


// Protect Data sets the internal protect flag that is used 
// to block the user from changing any data for this node.
HRESULT SVAccessClass::ProtectData( long lID )
{
	SVAccessPointNode* pNode = m_svStorage.FindByID( lID );
	if( pNode )
	{
		pNode->m_bDataCannotChange = true;
		return S_OK;
	}
	return -3014;
}

// This function checks the protect flag. If not set,
// then the function returns true.
bool SVAccessClass::IsChangable( long lID )
{
	SVAccessPointNode* pNode = m_svStorage.FindByID( lID );
	if( pNode )
	{
		if(!pNode->m_bDataCannotChange )
		return true;
	}
	return false;
}

// This function returns the group or list of groups for 
// the node associated with the given ID.
HRESULT SVAccessClass::GetNTGroup( long lID, CString& rstrGroup )
{
	HRESULT hr = S_FALSE;
	SVAccessPointNode* pNode = m_svStorage.FindByID( lID );

	if( pNode )
	{
		if( pNode )
		{
			rstrGroup = pNode->m_strNTGroup;
			hr = S_OK;
		}
	}

	return hr;
}


// SetForcedPrompt sets the force prompt flag so users always
// have to log on to pass an access point.
HRESULT SVAccessClass::SetForcedPrompt( long lID, BOOL bForce )
{
	HRESULT hr = S_FALSE;

	SVAccessPointNode* pNode = m_svStorage.FindByID( lID );

	if( pNode )
	{
		pNode->m_bForcePrompt = bForce ? true : false;
		hr = S_OK;
	}
	return hr;
}

// This function gets the internal flag for forced prompt mode.
HRESULT SVAccessClass::GetForcedPrompt( long lID, BOOL& rbForce )
{
	HRESULT hr = S_FALSE;
	if(SVAccessPointNode* pNode = m_svStorage.FindByID( lID ) )
	{
		rbForce = pNode->m_bForcePrompt ;
		hr = S_OK;
	}

	return hr;
}

// This function returns the user timeout value for
// user timeout mode.
long SVAccessClass::GetUserTimeout()
{
	return m_svStorage.GetUserTimeout();
}

// This function sets the user timeout.
HRESULT SVAccessClass::SetUserTimeout(long lTime)
{
	return m_svStorage.SetUserTimeout( lTime );
}

// This function returns true if the logon mode is in effect.
bool SVAccessClass::GetUseLogon()
{
	return m_svStorage.GetUseLogon();
}

// This function sets the user logon mode according to the supplied flag.
HRESULT SVAccessClass::SetUseLogon(bool bUse)
{
	return m_svStorage.SetUseLogon( bUse );
}

// Logout clears the current user and password.
HRESULT SVAccessClass::Logout()
{
	SVException svE;
	SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_LOGGED_OUT, (LPTSTR)(LPCTSTR)m_svStorage.GetCurrentUser(), _T("") , _T(""));

	CString strTmp = _T("Log out");
	svE.LogException( strTmp ); 

	m_svStorage.ClearUser();
	m_svStorage.ClearPW();
	return S_OK;
}

// Logon displays the password dialog and 
// if a successful logon occurs, the current user 
// and passord are stored in a temperary location.
HRESULT SVAccessClass::Logon()
{
	CString strTmpUser, strTmpPW;

	strTmpUser = m_svStorage.GetCurrentUser();

	HRESULT hr;
	CString l_strStatus;
	while( (hr = PasswordDialog( strTmpUser, strTmpPW, _T("Login"), l_strStatus )) 
		== SVMSG_SVS_ACCESS_DENIED )
	{
		l_strStatus = _T("Invalid User or Password");
	}

	if( hr == S_OK )
	{
		SVException svE;
		SETSECURITYEXCEPTION(svE, SVMSG_SVS_ACCESS_GRANTED1, (LPTSTR)(LPCTSTR)strTmpUser, _T("Login"), _T(""));
		svE.LogException( strTmpUser );

		m_svStorage.SetUser( strTmpUser );
		m_svStorage.SetPW( strTmpPW );
		ResetTime();
	}
	else
	{
		m_svStorage.ClearUser();
		m_svStorage.ClearPW();
	}

	return hr;
}

// This function saves the security setup
// to a file defined in the security storage class.
HRESULT SVAccessClass::Save()
{
	return m_svStorage.Save(m_strFileName.c_str());
}

// This function calls the security setup dialog
int SVAccessClass::SetupDialog()
{
	INT_PTR iRet = IDCANCEL;

	SVSecuritySetupSheet oDlg;
	oDlg.m_pAccess = this;

	//remove Apply button
	oDlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	iRet = oDlg.DoModal();

	// clear user & pw after exit
	m_svStorage.ClearPW();
	m_svStorage.ClearUser();

	if( iRet == IDOK )
	{
		Save();
	}

	return static_cast< int >( iRet );
}

// GetCurrentUser returns the current logged on user.
CString SVAccessClass::GetCurrentUser()
{
	return m_svStorage.GetCurrentUser();
}

// This function returns the current logged on password.
CString SVAccessClass::GetCurrentPassword()
{
	return m_svStorage.GetCurrentPassword();
}

// This function sets the Groups for a given ID
HRESULT SVAccessClass::SetNTGroup( long lID, LPCTSTR strGroup )
{
	HRESULT hr = S_FALSE;
	if( SVAccessPointNode* pNode = m_svStorage.FindByID( lID ))
	{
		if( !pNode->m_bDataCannotChange )
		{
			pNode->m_strNTGroup = strGroup;
			hr = S_OK;
		}
		else
			hr = -3017;
	}
	return hr;
}

// Ansii Functions...
// Load loads the security system from the storage class
//
HRESULT SVAccessClass::Load( const char* const pFileName)
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		m_strFileName = A2W( pFileName );
		l_hrOk =  m_svStorage.Load( m_strFileName.c_str() );
	#else
		m_strFileName = pFileName;
		l_hrOk =  m_svStorage.Load( pFileName );
	#endif

	return l_hrOk;
}

// Adds a new security node.
HRESULT SVAccessClass::Add( long lID, const char* const sName, const char* const sNTGroup, bool bForcePrompt )
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		l_hrOk = m_svStorage.Add( lID, A2W( sName ), A2W( sNTGroup ), bForcePrompt );
	#else
		l_hrOk = m_svStorage.Add( lID, sName, sNTGroup, bForcePrompt );
	#endif

	return l_hrOk;
}

// Adds a new security node with default empty group and default forced prompt.
HRESULT SVAccessClass::Add( long lID, const char* const sName)
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		l_hrOk = m_svStorage.Add( lID, A2W( sName ));
	#else
		l_hrOk = m_svStorage.Add( lID, sName );
	#endif

	return l_hrOk;
}


// Wide char / Unicode Functions...
HRESULT SVAccessClass::Load(const wchar_t* const pFileName)
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		m_strFileName = pFileName;
		l_hrOk = m_svStorage.Load(pFileName);
	#else
		m_strFileName = W2A(pFileName) ;
		l_hrOk = m_svStorage.Load( m_strFileName.c_str());
	#endif

	return l_hrOk;
}

// Adds a new security node.
HRESULT SVAccessClass::Add( long lID, const wchar_t* const sName, const wchar_t* const sNTGroup, bool bForcePrompt )
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		l_hrOk = m_svStorage.Add( lID, sName, sNTGroup, bForcePrompt );
	#else
		l_hrOk = m_svStorage.Add( lID, W2A( sName ), W2A( sNTGroup ), bForcePrompt );
	#endif

	return l_hrOk;
}

// Adds a new security node with default empty group and default forced prompt.
HRESULT SVAccessClass::Add( long lID, const wchar_t* const sName)
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		l_hrOk = m_svStorage.Add( lID, sName );
	#else
		l_hrOk = m_svStorage.Add( lID, W2A( sName ) );
	#endif

	return l_hrOk;
}

// CreateProcess - ansii version spawns a process.
// The intent was to create a process with the security priveledges of the current 
// SVObserver user.
HRESULT SVAccessClass::CreateProcess( const char* const strAppName, const char* const strPath, const char* const strCommand )
{
	USES_CONVERSION;

	HRESULT l_hrOk = S_FALSE;

	#ifdef _UNICODE
		l_hrOk = CreateProcess( strAppName, strPath, strCommand );
	#else
		l_hrOk = CreateProcess( A2W( strAppName ), A2W( strPath ), A2W( strCommand) );
	#endif

	return l_hrOk;
}

// CreateProcess - wide char version.
HRESULT SVAccessClass::CreateProcess( const wchar_t* const p_wstrAppName, const wchar_t* const p_wstrPath, const wchar_t* const p_wstrCommand )
{
	HRESULT l_hr = S_OK;

	// Set Startup structure for process
	STARTUPINFOW Startup;
	PROCESS_INFORMATION processInfo;
	wchar_t wstrArgs[256];
	wchar_t wstrAppPathName[_MAX_PATH];
	wchar_t wstrDirPath[_MAX_DIR];

	memset(wstrArgs, 0, sizeof(wstrArgs));
	memset(wstrAppPathName, 0, sizeof(wstrAppPathName));
	memset(wstrDirPath, 0, sizeof(wstrDirPath));
	memset(&Startup, 0, sizeof(Startup));
	memset(&processInfo, 0, sizeof(processInfo));

	Startup.cb = sizeof(Startup);
	Startup.dwFlags = STARTF_USESHOWWINDOW;
	Startup.wShowWindow = SW_SHOWDEFAULT;
	Startup.lpDesktop = NULL;  // will create an invisible desktop for this user
	Startup.lpReserved = NULL;
	Startup.lpReserved2 = NULL;
	Startup.lpTitle = NULL;
//	GetStartupInfoW(&Startup);

	wcscpy( wstrAppPathName, p_wstrAppName );        // Start with the supplied app name
	wcscpy( wstrDirPath, p_wstrPath );               // Start with supplied path

	// Check if file path is valid
	int l_handle;
	if((l_handle = _wopen( wstrAppPathName, _O_RDONLY )) < 0 )
	{
		// Check if path plus the File is valid
		wcscpy( wstrAppPathName, p_wstrPath );
		int l_iLen = static_cast<int>(wcslen( p_wstrPath ));
		if( l_iLen > 0 )                          // Check if the last charactor is a backslash
		{                                         // if not then append a backslash.
			if( wstrAppPathName[l_iLen - 1] != L'\\' )
			{
				wcscat( wstrAppPathName, L"\\" );   
			}
		}
		wcscat( wstrAppPathName, p_wstrAppName );     // Add the appName to the appPathName
		if((l_handle = _wopen( wstrAppPathName, _O_RDONLY)) < 0 )
		{
			// Search the System PATH variable for the app Name
			_wsearchenv( p_wstrAppName, L"PATH", wstrAppPathName );
			if( wstrAppPathName[0] == 0 )          // So try the Environment PATH
			{
				return S_FALSE;
			}
			else
			{                                     // We found the correct path
				wcscpy( wstrDirPath, wstrAppPathName );
				int iLen = static_cast<int>(wcslen( wstrDirPath ));
				for( int i = iLen ; i > -1; i-- )// get rid of the name
				{                                 // so we only have the directory
					if( wstrDirPath[i] == '\\' )  // path portion.
					{
						wstrDirPath[i] = 0;
						break;
					}
				}
			}
		}
		else
		{
			_close( l_handle );
		}
	}
	else
	{                                             // We found the correct path
		_close( l_handle );
		wcscpy( wstrDirPath, wstrAppPathName );
		int iLen = static_cast<int>(wcslen( wstrDirPath ));
		for( int i = iLen ; i > -1; i-- )        // get rid of the name
		{                                         // so we only have the directory
			if( wstrDirPath[i] == '\\' )          // path portion.
			{
				wstrDirPath[i] = 0;
				break;
			}
		}
	}


	CString l_strGroup;
	l_hr = GetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN, l_strGroup );
	if( l_hr == S_OK )
	{
		// WINSHELLAPI HINSTANCE APIENTRY ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
		// Old way with out User/PW
		if( l_strGroup.CompareNoCase(_T("Everybody")) == 0 || true )  // Disable CreateProcessWithLogon method of launching
		{
			if (33 >  (int) ShellExecuteW (HWND_DESKTOP,
										  L"open",
										  p_wstrAppName,
										  p_wstrCommand,
										  wstrDirPath,
										  SW_SHOWNORMAL))
			{
			  CString msg;
#ifdef _UNICODE
			  msg.Format (_T("Unable to start %s\n(%s).\n\nCheck Utility Properties."),
						  p_wstrAppName,
						  p_wstrCommand);
#else
			  msg.Format (_T("Unable to start %s\n(%s).\n\nCheck Utility Properties."),
						  CW2A(p_wstrAppName),
						  CW2A(p_wstrCommand));
			  ::MessageBox (HWND_DESKTOP, msg, _T("Failure"), MB_OK);
#endif
			}
		}
		else
		{
			wcscpy(wstrArgs, p_wstrAppName);
			wcscat(wstrArgs, L" ");
			wcscat(wstrArgs, p_wstrCommand);

			// Convert local members to Wide charactors
			CA2W l_pwcCurrentUser(m_svStorage.GetCurrentUser().operator LPCTSTR());
			CA2W l_pwcLogonServer( m_strLogonServer.operator LPCTSTR() );
			CA2W l_pwcPassword( m_svStorage.GetCurrentPassword().operator LPCTSTR());

			long lError;

			lError = CreateProcessWithLogonW( l_pwcCurrentUser,   // User
				l_pwcLogonServer,                                 // Domain
				l_pwcPassword,                                    // Password
				LOGON_WITH_PROFILE,                               // Logon Flags
				wstrAppPathName,                                  // Application
				wstrArgs,                                             // Command line
				CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_NEW_PROCESS_GROUP|NORMAL_PRIORITY_CLASS,	// Creation Flags 
				NULL,                                             // Environment
				wstrDirPath,                                      // Current Directory
				&Startup,                                         // StartupInfo
				&processInfo);                                    // processInfo

			if( lError == 0 )
			{
				CString strErr;
				DWORD dwErr = GetLastError();
				strErr.Format( _T("Return - %08x Last Error %08x"), lError, dwErr);
				AfxMessageBox( strErr );
				l_hr = -3012;
			}
			else
			{
				CloseHandle( processInfo.hProcess );
				CloseHandle( processInfo.hThread );
				l_hr = S_OK;
			}
		}
	}
	return l_hr;
}

// SVIsSecured 
// The only way a given node is not secured, is if it is found, does not have the forced prompt selected, and has its group set to everybody.  Otherwise it is secured by default.
bool SVAccessClass::SVIsSecured( long lId )
{
	bool l_bRet = true;
	BOOL l_bForced = true;

	if( GetForcedPrompt( lId, l_bForced ) == S_OK && ! l_bForced )
	{
		CString l_strGroups;

		HRESULT hr = GetNTGroup( lId, l_strGroups );
		if( hr == S_OK )
		{
			if( l_strGroups.CompareNoCase( _T( "Everybody" ) ) == 0)
			{
				l_bRet = false;
			}
		}
	}

	return l_bRet;
}

// This function simply returns true if there is a current user
bool SVAccessClass::IsLoggedOn()
{
	if( GetCurrentUser().IsEmpty() )
		return false;
	else
		return true;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSecurity\SVAccessClass.cpp_v  $
 * 
 *    Rev 1.2   20 Aug 2014 11:30:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  929
 * SCR Title:  SVObserver Customize Utilities issue
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateProcess method to initialize variables and to ensure the correct size for the path and directory arrays.
 * Revised CreateProcess method to correct the issue of using unsigned varibales in a for loop checking for signed variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 10:00:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:59:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   18 Sep 2012 18:43:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   30 Aug 2010 13:27:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   17 Dec 2009 09:14:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  679
 * SCR Title:  Change functionality of IsSecured to include Forced Prompt functionaliity
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the SVIsSecured method to take into account the forced prompt functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Dec 2009 10:55:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Jun 2009 09:49:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  661
 * SCR Title:  Remove MFC from Security DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed error in Save function caused by merging code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   27 May 2009 09:20:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  661
 * SCR Title:  Remove MFC from Security DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed MFC from SVSecurity DLL.
 * Changed member variable in SVAccess Class for holding the file name from a CString to a std::basic_string<TCHAR>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   09 Apr 2009 14:59:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   20 Jun 2007 09:25:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   08 Feb 2007 09:47:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  591
 * SCR Title:  Remove dead files and unused constants
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comments to functions.  Removed un-used function IsSecured.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   05 Feb 2007 13:11:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  591
 * SCR Title:  Remove dead files and unused constants
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed un-used static constants SV_IT_DOES and SV_IT_DOES_NOT because they are already defined in a different source.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   23 Mar 2006 08:44:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  558
 * SCR Title:  Expand Code in SVSecurity DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Code Explansion to make debugging easier.
 * Added subroutine to check for master password.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Sep 2005 10:55:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in Password Dialog so LogOnUser uses the member variable m_strLogonServer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Aug 2005 11:01:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Logon Bug in Validation Functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Aug 2005 14:55:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with event logging logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   22 Aug 2005 10:46:50   tbair
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
 *    Rev 1.10   17 Aug 2005 12:08:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Validate Logon to include the text of the access point being entered.  Also modified Password dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Aug 2005 09:51:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVCreateProcess to use ShellExecute instead of CreateProcessWithLogon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Aug 2005 08:27:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in timeexpired and made TimeExpired public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Jul 2005 14:20:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Security to allow access.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Jul 2005 11:07:36   tbair
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
 *    Rev 1.5   18 Jul 2005 14:55:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Changes - Removed Auto Offline to Edit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Jul 2005 13:49:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2005 14:11:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified CreateProcess for Utilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 09:21:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSecurityStorageClass to use stl instead of the Microsoft CArray.  Fixed Load problem with SecurityStorage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:06:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
