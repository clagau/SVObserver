//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityStorage.h
//* .File Name       : $Workfile:   SVSecurityStorage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:04:52  $
//******************************************************************************

#ifndef SVSECURITYSTORAGE_H
#define SVSECURITYSTORAGE_H

#include <vector>

#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVMaterialsLibrary/SVMaterialsTreeAdapter.h"

#include "SVAccessPointNode.h"

class SVSecurityStorage
{
public:
	friend class SVSecuritySetupPage;

	SVSecurityStorage();
	virtual ~SVSecurityStorage();

	HRESULT Load(LPCTSTR pFileName);
	HRESULT Add( long lID, LPCTSTR sName, LPCTSTR sGroup , bool bForcePrompt = false );
	HRESULT Add( long lID, LPCTSTR sName );
	HRESULT ProtectData( long lID );

	bool GetUseLogon() const;
	bool GetAutoEdit() const;
	void SetUser( LPCTSTR strUser );
	void SetPW( LPCTSTR strPW );
	CString GetCurrentUser() const;
	CString GetCurrentPassword() const;

	// Use these functions to setup access
	HRESULT Save(LPCTSTR pFileName);
	HRESULT SetNTGroup( long lID, LPCTSTR strGroup );
	HRESULT SetForcedPrompt( long lID, BOOL bForce );
	HRESULT SetUseAutoEdit( bool p_bUse);
	long GetUserTimeout() const;
	HRESULT SetUseLogon( bool bUse );
	HRESULT SetUserTimeout(long lTime);

	void ClearUser( );
	void ClearPW( );


	SVAccessPointNode* FindByID( long lID );


protected:
	typedef std::vector < SVAccessPointNode > SVAccessPointNodeVectorArray;

	HRESULT GetMaterialsTree( SVMaterialsTree& p_rMaterialsTree ) const;
	HRESULT GetChildMaterialsTree( SVMaterialsTreeAdapter& p_rMaterialsTree, SVAccessPointNodeVectorArray::const_iterator& p_rNodeIter ) const;

	HRESULT ProcessMaterialsTree( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree& p_rMaterialsTree );
	HRESULT ProcessChild( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTreeAdapter& p_rMaterialsTree );

	HRESULT ProcessRoot( const SVMaterialsTree& p_rMaterialsTree );


	// Manage Data to tree...
	long m_lCurrentData;
	long m_lCurrentIndex;

	// Settings
	long m_lTimeout;
	bool m_bUseLogon;
	bool m_bAutoEdit;

	// The Tree Stored as an array
	SVAccessPointNodeVectorArray m_aNodes;

	// Auto Log
//	CString m_strLogonServer;
	CString m_strCurrentUser;
	CString m_strCurrentPW;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecurityStorage.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:04:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Sep 2012 18:42:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2009 10:54:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jun 2007 09:25:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Mar 2006 08:50:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  558
 * SCR Title:  Expand Code in SVSecurity DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ClearPW and ClearUser.
 * Added static_cast<long>(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Jul 2005 13:49:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 09:21:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSecurityStorageClass to use stl instead of the Microsoft CArray.  Fixed Load problem with SecurityStorage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:10:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
