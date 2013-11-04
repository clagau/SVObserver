// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SecuredObject
// * .File Name       : $Workfile:   SecuredObject.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:30:36  $
// ******************************************************************************

#ifndef SECUREDOBJECT_H
#define SECUREDOBJECT_H

class SVSecuredObject : public CObject
{
	DECLARE_SERIAL(SVSecuredObject)

public:
	SVSecuredObject();
	virtual ~SVSecuredObject();

public:
	CString Lookup(LPCTSTR szApp, int nParent, int nChild, BOOL bCreate=FALSE, LPCTSTR groups=NULL);
	void  Load(LPCTSTR szApp=NULL, int nParent=0);
	void  Save();
	BOOL  IsSecured(DWORD nParent, DWORD nChild=0);
	BOOL  Validate(CString strAccount, CString strPassword, DWORD nParent, DWORD nChild=0);

private:
	friend BOOL CALLBACK EnumChildKeys(LPCTSTR p_szKey, void* ptr);
	friend BOOL CALLBACK EnumParentKeys(LPCTSTR p_szKey, void* ptr);
	friend BOOL CALLBACK EnumApplicationKeys(LPCTSTR p_szKey, void* ptr);

private:
	CMapStringToString	m_map;
	CString				m_key;
	BOOL				m_bModified;
	CString				m_szAppKey;
	CString				m_szParentKey;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SecuredObject.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:30:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 08:28:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Jul 2000 17:17:20   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated Model Ids
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jul 2000 10:07:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  11
 * SCR Title:  Stage 2: Security
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Secured Object class using resource Id's and the registry for access to program functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
