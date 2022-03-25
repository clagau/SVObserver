//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityManager
//* .File Name       : $Workfile:   SVSecurityManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:26  $
//******************************************************************************

#pragma once

class SVAccessClass;

class __declspec(dllexport) SVSecurityManager
{
public:
	bool    SVIsLoggedOn();
	bool	SVIsSecured( long lId );
	SVSecurityManager();
	virtual ~SVSecurityManager();
	HRESULT SVValidate( long lId );
	HRESULT SVValidate( long lId1, long lId2 );
	bool SVIsDisplayable( long lId );
	int     SVSetupDialog();
	HRESULT SVLogon();
	HRESULT SVLogout();
	HRESULT SVGetUser( BSTR& p_rbstrUser );

	HRESULT SVLoad( LPCTSTR pFileName);
	HRESULT SVCreateProcess( LPCTSTR AppName, LPCTSTR Path, LPCTSTR Command  );
	HRESULT SVAdd( long lID, LPCTSTR Name, LPCTSTR NTGroup = nullptr, bool ForcePrompt = false );

	HRESULT SVProtectData( long lID );
	bool    SVGetUseLogon();
private:
	SVAccessClass* m_pSVAccess;
};

__declspec(dllexport) SVSecurityManager& TheSecurityManager();

