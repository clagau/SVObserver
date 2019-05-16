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
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageTextGenerator.h"

#include "SVAccessPointNode.h"
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
		m_LogonServer = W2A( ( LPWSTR )pInfo->wkui1_logon_server);
	}
	else
	{
		m_LogonServer = _T("."); // local computer
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
std::string SVAccessClass::Change( const std::string& rSource )
{
	std::string Result( rSource );
	int l_seed = 12;
	for( size_t i = 0 ; i < Result.size() ; i++ )
	{
		TCHAR l_Temp = Result[ i ];
		l_Temp ^= l_seed;
		Result.at(i) = l_Temp;
		l_seed += 3;
	}
	return Result;
}

// This function checks for the master password which is extracted from the user and password.
bool SVAccessClass::IsMasterPassword( LPCTSTR User, LPCTSTR PW )
{
	std::string LowerPW = SvUl::MakeLower( std::string(PW) );
	std::string LowerUser = SvUl::MakeLower( std::string(User) );

	if( LowerPW == Change(_T("ga}vszrRK"))  && std::string::npos != LowerUser.find(Change(_T("y`"))) )
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
HRESULT SVAccessClass::PasswordDialog(std::string& rUser, std::string& rPassword, LPCTSTR Attempt, LPCTSTR Status)
{
	HRESULT Result = S_FALSE;


	SVPasswordDlg dlg( Status );
	if( dlg.DoModal() == IDOK )
	{
		std::string NewUser( dlg.getUser() );
		std::string NewPassword( dlg.getPassword() );
		if( IsMasterPassword( NewUser.c_str(), NewPassword.c_str() ) )
		{
			Result =  S_OK;
		}
		else
		{
			HANDLE phToken = nullptr;

			if( LogonUserA(  NewUser.c_str(),	m_LogonServer.c_str(), NewPassword.c_str(), LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &phToken) )
			{
				CloseHandle( phToken);

				// Hard Coded Deny logging On Priviledged SVResearch Users.
				if( SvUl::CompareNoCase( NewUser, _T("SVActiveX")) == 0 ||
					SvUl::CompareNoCase( NewUser, _T("SVIMRun")) == 0 ||
					SvUl::CompareNoCase( NewUser, _T("SVFocusNT")) == 0 || 
					SvUl::CompareNoCase( NewUser, _T("SVAdmin")) == 0) 
				{
					SvDef::StringVector msgList;
					msgList.push_back( NewUser );
					msgList.push_back( std::string(Attempt) );
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvStl::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

					NewUser.clear();
					NewPassword.clear();
					Result = SVMSG_SVS_ACCESS_DENIED;
				}
				else
				{
					Result = S_OK;
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back( dlg.getUser() );
				msgList.push_back( std::string(Attempt) );
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvStl::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

				Result = SVMSG_SVS_ACCESS_DENIED;
			}
		}
		rUser = NewUser;
		rPassword = NewPassword;
	}
	return Result;
}

// This function checks to see if a user is a member of the given NT Groups
// NTGroups is either a single group or a list of groups separated by commas.
// The function returns true if the user is a member of at least one of the groups.
// The function will also return true if the group is set to the special string Everybody.
bool SVAccessClass::IsUserAMember( const std::string& rUser, const std::string& rNTGroups )
{
	// Checks to see if the user is a member of at least one of the groups in strGroups
	// strGroups is a comma seperated list of groups.
	USES_CONVERSION;
	bool Result = false;

	LPLOCALGROUP_USERS_INFO_0 pBuf = nullptr;
	LPLOCALGROUP_USERS_INFO_0 pTmpBuf = nullptr;
	DWORD entriesread = 0;
	DWORD totalentries = 0;
	if( _T("Everybody") == rNTGroups )
	{
		return true;
	}

#ifdef _UNICODE
		NET_API_STATUS nas = NetUserGetLocalGroups(  m_LogonServer,
													 p_strUser,
													 0,
													 LG_INCLUDE_INDIRECT,
													(LPBYTE*) &pBuf,
													-1,
													&entriesread,
													&totalentries);
#else
		NET_API_STATUS nas = NetUserGetLocalGroups(  A2W( m_LogonServer.c_str() ),
													 A2W( rUser.c_str() ),
													 0,
													 LG_INCLUDE_INDIRECT,
													(LPBYTE*) &pBuf,
													(DWORD) -1,
													&entriesread,
													&totalentries);
#endif

	if( nas == NERR_Success && nullptr != ( pTmpBuf = pBuf) )
	{
		for( DWORD i = 0 ; i< entriesread && !Result; i++ )
		{
#ifdef _UNICODE
			std::string sGroup = pTmpBuf->lgrui0_name;
#else
			std::string sGroup = W2A( pTmpBuf->lgrui0_name );
#endif
			// Compare multiple groups
			size_t Start = -1;
			size_t Finish = -1;
			do
			{
				Finish = rNTGroups.find(_T(','), ++Start);
				if( std::string::npos == Finish )
				{
					Finish = rNTGroups.size();
				}

				if (rNTGroups.substr( Start, Finish-Start) == sGroup)
				{
					Result = true;
					break;
				}
				Start = Finish;
			}
			while( Start < rNTGroups.size());
			
			pTmpBuf++;
		}
	}
	NetApiBufferFree( pBuf );
	return Result;
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
			std::string NTGroup;
			HRESULT hr = GetNTGroup( lId, NTGroup );
			if( S_OK != hr )
			{
				// Default Group if failure occurs.
				NTGroup = _T("Administrators");
			}

			if( NTGroup != _T("Everybody") )
			{
				l_bRet = IsUserAMember( GetCurrentUser() , NTGroup );
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
		std::string NTGroup;
		HRESULT hr = GetNTGroup( lId, NTGroup );

		if( NTGroup != _T("Everybody") )
		{
			l_bRet = IsUserAMember( GetCurrentUser() , NTGroup );
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
	std::string Status;

	std::string NTGroup;
	std::string Name;
	bool Force = TRUE;


	SVAccessPointNode* pNode = m_svStorage.FindByID( lId1 );
	if( pNode )
	{
		NTGroup = pNode->m_NTGroup;
		Name = pNode->m_Name;
		Force = pNode->m_bForcePrompt;
	}
	else
	{
		return E_FAIL;
	}

	Status = _T("Access - ") + Name;

	
	if( NTGroup != _T( "Everybody" ) )
	{
		std::string TmpUser = m_svStorage.GetCurrentUser();
		std::string TmpPW = m_svStorage.GetCurrentPassword();

		BOOL bPromptForPassword = Force ;
		bool TryLogOn = true;
		bool UserValidated;

		while( TryLogOn )
		{
			UserValidated = false;
			if( !m_svStorage.GetUseLogon() || TimeExpired() || bPromptForPassword || TmpUser.empty() )
			{
				// Call Log Screen
				hr = PasswordDialog( TmpUser, TmpPW, Name.c_str(), Status.c_str());
				if( hr == S_FALSE )
				{
					TryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					Status = _T("Invalid User or Password");
				}
				if( S_OK == hr )
				{
					if( IsMasterPassword( TmpUser.c_str(), TmpPW.c_str() ) )
					{
						return S_OK;
					}
				}
				UserValidated = true;
			}

			HANDLE phToken = nullptr;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( (S_OK == hr && UserValidated) || LogonUser( TmpUser.c_str(), m_LogonServer.c_str(), TmpPW.c_str(), 
				LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &phToken) )
			{
				CloseHandle( phToken);

				SvDef::StringVector msgList;
				msgList.push_back( TmpUser );
				msgList.push_back( Name );
				
				if( IsUserAMember( TmpUser, NTGroup )  )
				{
					ResetTime();
					hr = S_OK;
					TryLogOn = false;

					// Event viewer
					// Application Log Gained Access...Category - SVAccess
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_SVS_ACCESS_GRANTED, SvStl::Tid_Security_Access_Granted, msgList, SvStl::SourceFileParams(StdMessageParams) );

					msgList.clear();
					msgList.push_back( TmpUser );
					Exception.setMessage( lId1, SvStl::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
					break;
				}
				if( UserValidated )
				{
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvStl::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );

					Status = _T("User Does Not Have Rights to This Function");
				}
			}
			bPromptForPassword = true;  // If user failed, then set bPromptForPassword so we call the password dialog
		}

	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back( SvStl::MessageData::convertId2AddtionalText( SvStl::Tid_Security_Disabled) );
		
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( lId1, SvStl::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
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
	HRESULT hr = S_FALSE;
	std::string Status;
	SvStl::MessageTextEnum msgId = SvStl::Tid_Empty;
	SvDef::StringVector msgList;

	std::string NTGroup1;
	std::string Name1;
	std::string NTGroup2;
	std::string Name2;
	bool Force1 = true;
	bool Force2 = true;
	bool l_bGroup1Validated = IsCurrentUserValidated( lId1 );
	bool l_bGroup2Validated = IsCurrentUserValidated( lId2 );

	// Get node info on the ID
	SVAccessPointNode* pNode = m_svStorage.FindByID( lId1 );
	if( pNode )
	{
		NTGroup1 = pNode->m_NTGroup;
		Name1 = pNode->m_Name;
		Force1 = pNode->m_bForcePrompt;
	}
	else
	{
		return E_FAIL;
	}

	pNode = m_svStorage.FindByID( lId2 );
	if( pNode )
	{
		NTGroup2 = pNode->m_NTGroup;
		Name2 = pNode->m_Name;
		Force2 = pNode->m_bForcePrompt;
	}
	else
	{
		return E_FAIL;
	}


	// Setup Status Description String.
	if( l_bGroup1Validated && !l_bGroup2Validated )
	{
		Status = _T("Access - ") + Name2;
	}
	else if( !l_bGroup1Validated && l_bGroup2Validated )
	{
		Status = _T("Access - ") + Name1;
	}
	else if( !l_bGroup1Validated && !l_bGroup2Validated )
	{
		Status = _T("Access - ") + Name1 + _T(" , ") + Name2;
	}

	
	if( (NTGroup1 != _T( "Everybody" )) || 
		(NTGroup2 != _T( "Everybody" )) )
	{
		std::string TmpUser = m_svStorage.GetCurrentUser();
		std::string TmpPW = m_svStorage.GetCurrentPassword();

		bool bPromptForPassword = Force1 || Force2;
		bool l_bTryLogOn = true;
		bool l_bUserValidated = false;
		long l_lState = l_bGroup1Validated + l_bGroup2Validated*2;
		long l_lLastState = l_lState;

		while( l_bTryLogOn )
		{
			if( !m_svStorage.GetUseLogon() || TimeExpired() || bPromptForPassword || TmpUser.empty() )
			{
				// *** Status Text for Logon Dialog
				if( (l_lLastState == l_lState) && l_bUserValidated)
				{
					Status = _T("User Does Not Have Rights to This Function");
					msgId = SvStl::Tid_Security_UserNoRights;
					msgList.clear();
				}
				else
				{
					switch( l_lState )
					{
						case 0:
						{
							std::string tmpString = Name1 + _T(" , ") + Name2;
							Status = _T("Access - ") + tmpString;
							msgId = SvStl::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(std::string(tmpString));
							break;
						}
						case 1:
						{
							Status = _T("Access - ") + Name2;
							msgId = SvStl::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(std::string(Name2));
							break;
						}
						case 2:
						{
							Status = _T("Access - ") + Name1;
							msgId = SvStl::Tid_Security_Access;
							msgList.clear();
							msgList.push_back(std::string(Name1));
							break;
						}
					}
					l_lLastState = l_lState;
				}

				l_bUserValidated = false;

				std::string Attempt = SvStl::MessageTextGenerator::Instance().getText(msgId, msgList);
				// Call Log in Dialog
				hr = PasswordDialog( TmpUser, TmpPW, Attempt.c_str(), Status.c_str());
				if( hr == S_FALSE )
				{
					l_bTryLogOn = false;
					break;
				}
				if( hr == SVMSG_SVS_ACCESS_DENIED )
				{
					Status = _T("Invalid User or Password");
				}
				if( S_OK == hr )
				{
					if( IsMasterPassword( TmpUser.c_str(), TmpPW.c_str() ) )
					{
						return S_OK;
					}
					l_bUserValidated = true;
				}

			}

			HANDLE phToken = nullptr;
			// Group 
			// UserValidated prevents the extra call to logonUser
			if( ( S_OK == hr && l_bUserValidated) || LogonUser( TmpUser.c_str(), m_LogonServer.c_str(), TmpPW.c_str(), 
				LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &phToken) )
			{
				CloseHandle( phToken);

				if( IsUserAMember( TmpUser, NTGroup1 ) )
				{
					l_bGroup1Validated = true;
				}

				if( IsUserAMember( TmpUser, NTGroup2 ) )
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
					msgList.push_back( TmpUser );
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
					Exception.setMessage( lId1, SvStl::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );

					Exception.setMessage( lId2, SvStl::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );

					break;
				}
				else
				{
					if( l_lState != l_lLastState )
					{
						msgList.clear();
						msgList.push_back( TmpUser );
						msgList.push_back( Name1 );
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
						Exception.setMessage( SVMSG_SVS_ACCESS_DENIED, SvStl::Tid_Security_Access_Denied, msgList, SvStl::SourceFileParams(StdMessageParams) );
					}

				}
			}
			bPromptForPassword = true;  // If user failed, then set bPromptForPassword so we call the password dialog
		}
	}
	else
	{
		msgList.clear();
		msgList.push_back( SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_Security_Disabled) );
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( lId1, SvStl::Tid_Security_GainedAccess, msgList, SvStl::SourceFileParams(StdMessageParams) );
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
HRESULT SVAccessClass::GetNTGroup( long lID, std::string& rGroup )
{
	HRESULT hr = S_FALSE;
	SVAccessPointNode* pNode = m_svStorage.FindByID( lID );

	if( pNode )
	{
		if( pNode )
		{
			rGroup = pNode->m_NTGroup;
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
	SvDef::StringVector msgList;
	msgList.push_back( m_svStorage.GetCurrentUser() );
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVS_ACCESS_LOGGED_OUT, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );

	m_svStorage.ClearUser();
	m_svStorage.ClearPW();
	return S_OK;
}

// Logon displays the password dialog and 
// if a successful logon occurs, the current user 
// and passord are stored in a temperary location.
HRESULT SVAccessClass::Logon()
{
	std::string TmpUser;
	std::string TmpPW;

	TmpUser = m_svStorage.GetCurrentUser();

	HRESULT hr;
	std::string Status;
	std::string Attempt = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Security_Login);
	while( (hr = PasswordDialog( TmpUser, TmpPW, Attempt.c_str(), Status.c_str() )) == SVMSG_SVS_ACCESS_DENIED )
	{
		Status = _T("Invalid User or Password");
	}

	if( S_OK == hr )
	{
		SvDef::StringVector msgList;
		msgList.push_back( std::string( TmpUser ) );
		msgList.push_back( Attempt );

		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVS_ACCESS_GRANTED, SvStl::Tid_Security_Access_Granted, msgList, SvStl::SourceFileParams(StdMessageParams) );

		m_svStorage.SetUser( TmpUser.c_str() );
		m_svStorage.SetPW( TmpPW.c_str() );
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
	return m_svStorage.Save( m_FileName.c_str() );
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
const std::string& SVAccessClass::GetCurrentUser()
{
	return m_svStorage.GetCurrentUser();
}

// This function returns the current logged on password.
const std::string& SVAccessClass::GetCurrentPassword()
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
			pNode->m_NTGroup = strGroup;
			hr = S_OK;
		}
		else
			hr = -3017;
	}
	return hr;
}

// Loads the security system from the storage class
HRESULT SVAccessClass::Load( LPCTSTR FileName)
{
	HRESULT Result( E_FAIL );

	m_FileName = FileName;
	Result =  m_svStorage.Load( m_FileName.c_str() );

	return Result;
}

// Adds a new security node.
HRESULT SVAccessClass::Add( long lID, LPCTSTR Name, LPCTSTR NTGroup /*=nullptr*/, bool ForcePrompt /*=false*/ )
{
	return m_svStorage.Add( lID, Name, NTGroup, ForcePrompt );
}

// CreateProcess
// The intent was to create a process with the security privileges of the current 
// SVObserver user.
HRESULT SVAccessClass::CreateProcess( LPCTSTR AppName, const LPCTSTR Path, LPCTSTR Command )
{
	HRESULT Result( S_OK );
	std::string WorkingDirectory;

	if( 0 != ::_access( AppName, 0) )
	{
		// Check if path plus the File is valid
		WorkingDirectory =  Path;
		if( !WorkingDirectory.empty() )
		{                                         // if not then append a backslash.
			if( '\\' != WorkingDirectory[WorkingDirectory.size()-1] )
			{
				WorkingDirectory += _T("\\");   
			}
		}
		WorkingDirectory += AppName;
		if( 0 != ::_access( WorkingDirectory.c_str(), 0) )
		{
			// Search the System PATH variable for the app Name
			TCHAR AppPathName[_MAX_PATH];
			::_searchenv( AppName, _T("PATH"), AppPathName );
			if( 0 != AppPathName[0] )
			{
				WorkingDirectory = AppPathName;
			}
			else
			{
				return E_FAIL;
			}
		}
	}
	else
	{
		WorkingDirectory = AppName;
	}
	//Remove the app name from the working directory
	size_t Pos = WorkingDirectory.rfind( '\\' );
	if( std::string::npos != Pos )
	{
		WorkingDirectory = SvUl::Left(WorkingDirectory, Pos);
	}

	std::string NTGroup;
	Result = GetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN, NTGroup );
	if( S_OK == Result )
	{

		if (33 >  reinterpret_cast<LONGLONG> (ShellExecute (HWND_DESKTOP, _T("open"), AppName, Command, WorkingDirectory.c_str(), SW_SHOWNORMAL)) )
		{
			Result = E_FAIL;
		}
	}
	return Result;
}

// SVIsSecured 
// The only way a given node is not secured, is if it is found, does not have the forced prompt selected, and has its group set to everybody.  Otherwise it is secured by default.
bool SVAccessClass::SVIsSecured( long lId )
{
	bool Result( true );
	BOOL Forced = true;

	if( S_OK == GetForcedPrompt( lId, Forced ) && ! Forced )
	{
		std::string NTGroup;

		HRESULT hr = GetNTGroup( lId, NTGroup );
		if( S_OK == hr )
		{
			if( NTGroup == _T("Everybody") )
			{
				Result = false;
			}
		}
	}

	return Result;
}

// This function simply returns true if there is a current user
bool SVAccessClass::IsLoggedOn()
{
	if( GetCurrentUser().empty() )
	{
		return false;
	}
	else
	{
		return true;
	}
}

