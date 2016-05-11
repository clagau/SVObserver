//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityManager
//* .File Name       : $Workfile:   SVSecurityManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:26  $
//******************************************************************************

#pragma once

#ifdef _EXPORTING
   #define CLASS_DECLSPEC    __declspec(dllexport)
#else
   #define CLASS_DECLSPEC    __declspec(dllimport)
#endif

class SVAccessClass;

class CLASS_DECLSPEC SVSecurityManager  
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

	// Ansi charactor functions
	HRESULT SVLoad(const char* const pFileName);
	HRESULT SVCreateProcess( const char* const strAppName, const char* const strPath, const char* const strCommand  );
	HRESULT SVAdd( long lID, const char* const sName, const char* const sGroup , bool bForcePrompt = false );
	HRESULT SVAdd( long lID, const char* const sName );

	// Wide charactor / Unicode support
	HRESULT SVLoad( const wchar_t* const pFileName);
	HRESULT SVCreateProcess( const wchar_t* const strAppName, const wchar_t* const strPath, const wchar_t* const strCommand  );
	HRESULT SVAdd( long lID, const wchar_t* const sName, const wchar_t* const sGroup , bool bForcePrompt = false );
	HRESULT SVAdd( long lID, const wchar_t* const sName );

	HRESULT SVProtectData( long lID );
	bool    SVGetUseLogon();
//	bool    SVGetAutoEdit();
private:
	SVAccessClass* m_pSVAccess;

};
