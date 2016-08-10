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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <lm.h>
//Moved to precompiled header: #include <time.h>
//Moved to precompiled header: #include <io.h>
//Moved to precompiled header: #include <FCNTL.H>

#include "SVAccessClass.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageTextGenerator.h"

#include "resource.h"
#include "SVAccessPointNode.h"
#include "Shellapi.h"
#include "SVPasswordDlg.h"
#include "SVSecuritySetupSheet.h"
#pragma endregion Includes

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
	PWKSTA_USER_INFO_1 pInfo = nullptr;
	NET_API_STATUS ns = NetWkstaUserGetInfo( nullptr, 1, (LPBYTE*)&pInfo );
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
HRESULT SVAccessClass::PasswordDialog(CString& strUser, CString& strPassword, LPCTSTR Attempt, LPCTSTR p_strStatus)
{
	HRESULT Result = S_FALSE;


	SVPasswordDlg dlg;
	dlg.m_strStatus = p_strStatus;
	//dlg.m_strUser = strUser;  // Jim does not want to default the User name
	if( dlg.DoModal() == IDOK )
	{
		if( IsMasterPassword( dlg.m_strUser, dlg.m_strPassword ) )
		{
			Result =  S_OK;
		}
		else
		{
			HANDLE phToken = nullptr;

			if( LogonUserA(  static_cast<LPCSTR> (dlg.m_strUser),
				static_cast<LPCSTR> (m_strLogonServer), 
				static_cast<LPCSTR> (dlg.m_strPassword), 
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
					SVStringArray msgList;
					msgList.push_back( SVString(dlg.m_strUser) );
					msgList.push_back( SVString(Attempt) );
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvOi::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

					dlg.m_strUser.Empty();
					dlg.m_strPassword.Empty();
					Result = SVMSG_SVS_ACCESS_DENIED;
				}
				else
				{
					Result = S_OK;
				}
			}
			else
			{
				SVStringArray msgList;
				msgList.push_back( SVString(dlg.m_strUser) );
				msgList.push_back( SVString(Attempt) );
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvOi::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

				Result = SVMSG_SVS_ACCESS_DENIED;
			}
		}
		strUser = dlg.m_strUser;
		strPassword = dlg.m_strPassword;
	}
	return Result;
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

	LPLOCALGROUP_USERS_INFO_0 pBuf = nullptr;
	LPLOCALGROUP_USERS_INFO_0 pTmpBuf = nullptr;
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

	if( nas == NERR_Success && nullptr != ( pTmpBuf = pBuf) )
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
			if( S_OK != hr )
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
				hr = PasswordDialog( strTmpUser, strTmpPW, strName1, l_strStatus);
				if( hr == S_FALSE )
				{
					l_bTryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					l_strStatus = _T("Invalid User or Password");
				}
				if( S_OK == hr )
				{
					if( IsMasterPassword( strTmpUser, strTmpPW ) )
					{
						return S_OK;
					}
				}
				l_bUserValidated = true;
			}

			HANDLE phToken = nullptr;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( (S_OK == hr && l_bUserValidated) || LogonUserA( ( LPSTR )( LPCSTR )strTmpUser,
				( LPSTR ) ( LPCTSTR )m_strLogonServer,
				( LPSTR ) ( LPCTSTR )strTmpPW, 
				LOGON32_LOGON_NETWORK,
				LOGON32_PROVIDER_DEFAULT,
				&phToken) )
			{
				CloseHandle( phToken);

				SVStringArray msgList;
				msgList.push_back( SVString( strTmpUser ) );
				msgList.push_back( SVString( strName1 ) );
				
				if( IsUserAMember( strTmpUser, strGroups1 )  )
				{
					ResetTime();
					hr = S_OK;
					l_bTryLogOn = false;

					// Event viewer
					// Application Log Gained Access...Category - SVAccess
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVS_ACCESS_GRANTED, SvOi::Tid_Security_Access_Granted, msgList, SvStl::SourceFileParams(StdMessageParams) );

					msgList.clear();
					msgList.push_back( SVString( strTmpUser ) );
					Exception.setMessage( lId1, SvOi::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
					break;
				}
				if( l_bUserValidated )
				{
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvOi::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

					l_strStatus = _T("User Does Not Have Rights to This Function");
				}
			}
			bPromptForPassword = TRUE;  // If user failed, then set bPromptForPassword so we call the password dialog
		}

	}
	else
	{
		SVStringArray msgList;
		msgList.push_back( SvStl::MessageData::convertId2AddtionalText( SvOi::Tid_Security_Disabled) );
		
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( lId1, SvOi::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
		hr = S_OK;
	}
	
	// if S_OK == hr, log to Application Log
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
	SvOi::MessageTextEnum msgId = SvOi::Tid_Empty;
	SVStringArray msgList;

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
	{
		l_strStatus = _T("Access - ") + strName2;
	}
	else if( !l_bGroup1Validated && l_bGroup2Validated )
	{
		l_strStatus = _T("Access - ") + strName1;
	}
	else if( !l_bGroup1Validated && !l_bGroup2Validated )
	{
		l_strStatus = _T("Access - ") + strName1 + _T(" , ") + strName2;
	}

	
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
					msgId = SvOi::Tid_Security_UserNoRights;
					msgList.clear();
				}
				else
				{
					switch( l_lState )
					{
						case 0:
						{
							CString tmpString = strName1 + _T(" , ") + strName2;
							l_strStatus = _T("Access - ") + tmpString;
							msgId = SvOi::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(SVString(tmpString));
							break;
						}
						case 1:
						{
							l_strStatus = _T("Access - ") + strName2;
							msgId = SvOi::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(SVString(strName2));
							break;
						}
						case 2:
						{
							l_strStatus = _T("Access - ") + strName1;
							msgId = SvOi::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(SVString(strName1));
							break;
						}
					}
					l_lLastState = l_lState;
				}

				l_bUserValidated = false;

				SVString Attempt = SvStl::MessageTextGenerator::Instance().getText(msgId, msgList);
				// Call Log in Dialog
				hr = PasswordDialog( strTmpUser, strTmpPW, Attempt.c_str(), l_strStatus);
				if( hr == S_FALSE )
				{
					l_bTryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					l_strStatus = _T("Invalid User or Password");
				}
				if( S_OK == hr )
				{
					if( IsMasterPassword( strTmpUser, strTmpPW ) )
					{
						return S_OK;
					}
					l_bUserValidated = true;
				}

			}

			HANDLE phToken = nullptr;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( ( S_OK == hr && l_bUserValidated) || LogonUserA( ( LPSTR ) ( LPCTSTR )strTmpUser,
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

					msgList.clear();
					msgList.push_back( SVString(strTmpUser) );
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( lId1, SvOi::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );

					Exception.setMessage( lId2, SvOi::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );

					break;
				}
				else
				{
					if( l_lState != l_lLastState )
					{
						msgList.clear();
						msgList.push_back( SVString(strTmpUser) );
						msgList.push_back( SVString(strName1) );
						SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
						Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvOi::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );
					}

				}
			}
			bPromptForPassword = TRUE;  // If user failed, then set bPromptForPassword so we call the password dialog
		}
	}
	else
	{
		msgList.clear();
		msgList.push_back( SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Security_Disabled) );
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( lId1, SvOi::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
		hr = S_OK;
	}
	
	// if S_OK == hr, log to Application Log
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
	SVStringArray msgList;
	msgList.push_back( SVString(m_svStorage.GetCurrentUser()) );
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVS_ACCESS_LOGGED_OUT, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );

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
	SVString Attempt = SvStl::MessageTextGenerator::Instance().getText(SvOi::Tid_Security_Login);
	while( (hr = PasswordDialog( strTmpUser, strTmpPW, Attempt.c_str(), l_strStatus )) 
		== SVMSG_SVS_ACCESS_DENIED )
	{
		l_strStatus = _T("Invalid User or Password");
	}

	if( S_OK == hr )
	{
		SVStringArray msgList;
		msgList.push_back( SVString( strTmpUser ) );
		msgList.push_back( Attempt );

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVS_ACCESS_GRANTED, SvOi::Tid_Security_Access_Granted, msgList, SvStl::SourceFileParams(StdMessageParams) );

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
	Startup.lpDesktop = nullptr;  // will create an invisible desktop for this user
	Startup.lpReserved = nullptr;
	Startup.lpReserved2 = nullptr;
	Startup.lpTitle = nullptr;
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
	if( S_OK == l_hr )
	{
		// WINSHELLAPI HINSTANCE APIENTRY ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
		// Old way with out User/PW
		if( l_strGroup.CompareNoCase(_T("Everybody")) == 0 || true )  // Disable CreateProcessWithLogon method of launching
		{
			if (33 >  reinterpret_cast<LONGLONG> (ShellExecuteW (HWND_DESKTOP,
										  L"open",
										  p_wstrAppName,
										  p_wstrCommand,
										  wstrDirPath,
										  SW_SHOWNORMAL)))
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
				wstrArgs,                                         // Command line
				CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_NEW_PROCESS_GROUP|NORMAL_PRIORITY_CLASS,	// Creation Flags 
				nullptr,                                          // Environment
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

	if( S_OK == GetForcedPrompt( lId, l_bForced ) && ! l_bForced )
	{
		CString l_strGroups;

		HRESULT hr = GetNTGroup( lId, l_strGroups );
		if( S_OK == hr )
		{
			if( 0 == l_strGroups.CompareNoCase( _T( "Everybody" ) ) )
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

