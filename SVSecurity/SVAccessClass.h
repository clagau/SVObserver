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

#ifndef SVACCESSCLASS_H
#define SVACCESSCLASS_H

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVAccessClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:00:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Sep 2012 18:43:00   jspila
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
 *    Rev 1.16   18 Feb 2011 09:27:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   27 Oct 2010 08:58:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   15 Dec 2009 10:55:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   27 May 2009 09:20:38   tbair
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
 *    Rev 1.12   20 Jun 2007 09:25:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Feb 2007 09:47:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  591
 * SCR Title:  Remove dead files and unused constants
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed un-used function IsSecured.  This was replaced with SVIsSecured.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   23 Mar 2006 08:44:58   tbair
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
 *    Rev 1.9   22 Aug 2005 10:46:48   tbair
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
 *    Rev 1.8   03 Aug 2005 09:52:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Aug 2005 09:52:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Aug 2005 08:27:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in timeexpired and made TimeExpired public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Jul 2005 14:20:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Security to allow access.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Jul 2005 11:07:36   tbair
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
 *    Rev 1.4   18 Jul 2005 15:11:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI - Removed Auto Offline to Edit 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Jul 2005 13:49:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jun 2005 14:11:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified CreateProcess for Utilities
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
