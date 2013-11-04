//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQBar
//* .File Name       : $Workfile:   SVPPQBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:30:48  $
//******************************************************************************

#ifndef SVPPQBAR_H
#define SVPPQBAR_H

#include "SVWindowBar.h"

class SVPPQObject;

class SVButtonList : public CArray< CButton*, CButton* >
{
public:
	virtual ~SVButtonList()
	{
		RemoveAll();
		FreeExtra();
	};

};



class SVPPQWindowClass : public CWnd
{
protected:
	//{{AFX_MSG(SVPPQWindowClass)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVPPQWindowClass();

	virtual ~SVPPQWindowClass();

	BOOL BuildButtons( SVPPQObject* PSelectedPPQ );

	BOOL DestroyButtons();


	//{{AFX_VIRTUAL(SVPPQWindowClass)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL


protected:
	SVButtonList buttonList;
	SVPPQObject *m_pPPQ;

};



class SVPPQBarClass : public SVWindowBarClass
{
	DECLARE_DYNCREATE( SVPPQBarClass )

protected:
	//{{AFX_MSG(SVPPQBarClass)
	afx_msg void OnSelChangePQTab( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVPPQBarClass();

	virtual ~SVPPQBarClass();

	BOOL Create( CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE, UINT nID = AFX_IDW_CONTROLBAR_FIRST + 16 );
	BOOL BuildButtons();

	BOOL DestroyButtons();
	
	CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );

	//{{AFX_VIRTUAL(SVPPQBarClass)
	//}}AFX_VIRTUAL

//#ifdef _DEBUG
//	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
//#endif


protected:
	SVPPQWindowClass PPQWindow;

	CTabCtrl	pqTabCtrl;

};







//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVPPQBAR_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPPQBar.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:30:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   09 Nov 2010 16:32:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 13:51:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   20 Nov 2002 10:46:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed reference of SVPPQObjectClass to SVPPQObject
 * Changed reference of SVDigitizerObjectClass to SVPPQObject
 *  Removed COM support
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:26:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 23 1999 11:03:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised for new PPQ architechture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
