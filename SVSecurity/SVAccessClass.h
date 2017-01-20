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

#pragma region Includes 
#include "SVAccessPointNode.h"
#include "SVSecurityStorage.h"
#include "ObjectInterfaces\MessageTextEnum.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

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
	HRESULT Load(LPCTSTR FileName);
	HRESULT CreateProcess( LPCTSTR AppName, LPCTSTR Path, LPCTSTR Command  );
	HRESULT Add( long lID, LPCTSTR Name, LPCTSTR NTGroup = nullptr , bool bForcePrompt = false );

	HRESULT ProtectData( long lID );
	bool GetUseLogon();
	bool TimeExpired();
protected:
	// Use these functions to setup access
	HRESULT Save();
	HRESULT SetNTGroup( long lID, LPCTSTR NTGroup );
	HRESULT SetForcedPrompt( long lID, BOOL Force );
	HRESULT GetNTGroup( long lID, SVString& rNTGroup );
	HRESULT GetForcedPrompt( long lID, BOOL& rForce );
	bool IsChangable( long lID );
	long GetUserTimeout();
	HRESULT SetUseLogon(bool bUse);
	HRESULT SetUserTimeout(long lTime);
	HRESULT PasswordDialog(SVString& rUser, SVString& strPassword, LPCTSTR Attempt, LPCTSTR p_strStatus);
	bool IsUserAMember( const SVString& rUser, const SVString& p_strGroups );
	bool IsCurrentUserValidated(long lId);

	void init();
	void ResetTime();


public:
	bool IsLoggedOn();
	const SVString& GetCurrentUser();
	const SVString& GetCurrentPassword();

	// **** Construction Destruction
	SVAccessClass();
	virtual ~SVAccessClass();


protected:
	SVString Change( const SVString& rSource );
	bool IsMasterPassword( LPCTSTR User, LPCTSTR PW );
	// Global settings

	// Time keeping
	time_t m_lTimeStamp;

	// Auto Log
	SVString m_LogonServer;
	SVString m_FileName;

	// Keep track of current data
	SVSecurityStorage m_svStorage;
};

