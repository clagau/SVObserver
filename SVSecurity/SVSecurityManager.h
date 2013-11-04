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

#if !defined(AFX_SVSECURITYMANAGER_H__F22A92EB_D27F_4CF1_B099_1B11A3C99888__INCLUDED_)
#define AFX_SVSECURITYMANAGER_H__F22A92EB_D27F_4CF1_B099_1B11A3C99888__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_SVSECURITYMANAGER_H__F22A92EB_D27F_4CF1_B099_1B11A3C99888__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecurityManager.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:03:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
 *    Rev 1.7   22 Aug 2005 10:46:48   tbair
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
 *    Rev 1.4   25 Jul 2005 11:07:34   tbair
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
 *    Rev 1.2   06 Jul 2005 13:49:26   tbair
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