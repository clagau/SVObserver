//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputsView
//* .File Name       : $Workfile:   SVRemoteOutputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2013 18:17:46  $
//******************************************************************************

#pragma once

#include <map>
#include <vector>

// SVRemoteOutputsView view
class SVIODoc;

class SVRemoteOutputsView : public CListView
{
	DECLARE_DYNCREATE(SVRemoteOutputsView)

protected:
	typedef std::map< CString, bool > SVGroupStateMap;

	SVRemoteOutputsView();           // protected constructor used by dynamic creation
	virtual ~SVRemoteOutputsView();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	HRESULT RemoteOutputGroupNameAtItem( CString& p_rstrGroupName, int p_iItem );
	bool EditOutput(int p_iWhere);
	bool AddOutput(int p_iWhere);

	CImageList ImageList;
	CImageList StateImageList;
	SVGroupStateMap m_GroupStates;

public:
	typedef std::vector<GUID> PPQGuidList;
	PPQGuidList m_aPPQGUIDs;
	CMenu m_ContextMenuProp;
	CMenu m_ContextMenuItem;
	CMenu m_ContextMenuItemNoDelete;
	CString m_strCurrentPLCId;
	int m_CurrentItem;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	SVIODoc* m_pDocument;
	SVIODoc* GetDocument();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRemoteOutputProperties();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRemoteOutputAdd();
	afx_msg void OnRemoteOutputDelete();
	afx_msg void OnRemoteOutputEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddTransferBlock();

};



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputsView.h_v  $
 * 
 *    Rev 1.1   23 Jul 2013 18:17:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std map
 * Added include for std vector
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2013 18:09:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std map
 * Added include for std vector
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:55:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jan 2012 11:54:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
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

