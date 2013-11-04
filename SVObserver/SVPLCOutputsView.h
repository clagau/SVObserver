//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputsView
//* .File Name       : $Workfile:   SVPLCOutputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:27:04  $
//******************************************************************************

#pragma once

// SVPLCOutputsView view
class SVIODoc;

class SVPLCOutputsView : public CListView
{
	DECLARE_DYNCREATE(SVPLCOutputsView)

protected:
	SVPLCOutputsView();           // protected constructor used by dynamic creation
	virtual ~SVPLCOutputsView();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	HRESULT PLCNameAtItem( CString& p_rstrPPQName, int p_iItem );
	bool EditOutput(int p_iWhere);
	bool AddOutput(int p_iWhere);

	CImageList ImageList;
	CImageList StateImageList;

public:
	typedef std::vector<GUID> PPQGuidList;
	PPQGuidList m_aPPQGUIDs;
	CMenu m_ContextMenuProp;
	CMenu m_ContextMenuItem;
	CMenu m_ContextMenuItemNoDelete;
	CString m_strCurrentPLCId;
	int m_CurrentItem;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	SVIODoc* m_pDocument;
	SVIODoc* GetDocument();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPlcProperties();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPlcAdd();
	afx_msg void OnPlcDelete();
	afx_msg void OnPlcEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddTransferBlock();
};



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCOutputsView.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:27:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2009 16:12:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified PLC GUI to match Functional Requirement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

