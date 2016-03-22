//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAccessClass.h
//* .File Name       : $Workfile:   SVAccessClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:00:10  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <string>
#include "SVAccessPointNode.h"
#include "SVSecurityStorage.h"

class SVAccessClass
{

friend class SVSecuritySetupSheet;
friend class SVSecuritySetupPage;
friend class SVSecurityGeneralPage;

public:
	// Use these functions to check access
	HRESULT Validate( long lId );
	HRESULT Validate( long lId1, long lId2 );
	bool IsDisplayable(long lId);
	bool SVIsSecured( long lId );
	int SetupDialog();
	HRESULT Logon();
	HRESULT Logout();
	// Ansii functions
	HRESULT Load(const char* const pFileName);
	HRESULT CreateProcess( const char* const strAppName, const char* const strPath, const char* const strCommand  );
	HRESULT Add( long lID, const char* const sName, const char* const sGroup , bool bForcePrompt = false );
	HRESULT Add( long lID, const char* const sName );
	// Wide char / Unicode functions
	HRESULT Load(const wchar_t* const pFileName);
	HRESULT CreateProcess( const wchar_t* const strAppName, const wchar_t* const strPath, const wchar_t* const strCommand  );
	HRESULT Add( long lID, const wchar_t* const sName, const wchar_t* const sGroup , bool bForcePrompt = false );
	HRESULT Add( long lID, const wchar_t* const sName );

	HRESULT ProtectData( long lID );
	bool GetUseLogon();
	bool TimeExpired();
protected:
	// Use these functions to setup access
	HRESULT Save();
	HRESULT SetNTGroup( long lID, LPCTSTR strGroup );
	HRESULT SetForcedPrompt( long lID, BOOL bForce );
	HRESULT GetNTGroup( long lID, CString& rstrGroup );
	HRESULT GetForcedPrompt( long lID, BOOL& rbForce );
	bool IsChangable( long lID );
	long GetUserTimeout();
	HRESULT SetUseLogon(bool bUse);
	HRESULT SetUserTimeout(long lTime);
	HRESULT PasswordDialog(CString& strUser, CString& strPassword, LPCTSTR strAtempt, LPCTSTR p_strStatus);
	bool IsUserAMember( const CString& p_strUser, const CString& p_strGroups );
	bool IsCurrentUserValidated(long lId);

	std::string m_strFileName;

	void init();
	void ResetTime();


public:
	bool IsLoggedOn();
	CString GetCurrentUser();
	CString GetCurrentPassword();

	// **** Construction Destruction
	SVAccessClass();
	virtual ~SVAccessClass();


protected:
	CString Change( CString p_strIn );
	bool IsMasterPassword( CString p_rstrUser, CString p_rstrPW );
	// Global settings

	// Time keeping
	time_t m_lTimeStamp;

	// Auto Log
	CString m_strLogonServer;

	// Keep track of current data
	SVSecurityStorage m_svStorage;

};

