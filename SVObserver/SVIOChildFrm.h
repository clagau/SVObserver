//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOSplitterFrame
//* .File Name       : $Workfile:   SVIOChildFrm.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:08:40  $
//******************************************************************************

#ifndef CHILDFRM_H
#define CHILDFRM_H

//#if !defined(AFX_CHILDFRM_H__A02486BD_8B4D_11D1_B749_97CA47472392__INCLUDED_)
//#define AFX_CHILDFRM_H__A02486BD_8B4D_11D1_B749_97CA47472392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVIOSplitterFrame
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVIOSplitterFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE( SVIOSplitterFrame )

public:
	// Generated message map functions
	//{{AFX_MSG(SVIOSplitterFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	SVIOSplitterFrame();   // protected constructor used by dynamic creation

public:
	virtual ~SVIOSplitterFrame();


public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	CSplitterWnd m_wndSplitter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif	// CHILDFRM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOChildFrm.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:08:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 10:31:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:48:18   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Oct 26 1999 15:17:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add new version headers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/