//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolResultPage
//* .File Name       : $Workfile:   SVExternalToolResultPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:56  $
//******************************************************************************

#ifndef SVEXTERNALTOOLRESULTPAGE_H
#define SVEXTERNALTOOLRESULTPAGE_H

class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;

#include "PropertyTree/PropTree.h"

class SVExternalToolResultPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVExternalToolResultPage)

public:
	SVExternalToolResultPage(const CString& sTitle = _T(""), SVExternalToolDetailsSheet* pParent = NULL, int id = IDD );
	~SVExternalToolResultPage();

	//{{AFX_DATA(SVExternalToolResultPage)
	enum { IDD = IDD_EXTERNAL_TOOL_RESULTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolResultPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	int SelectObject(int iIndex);
	int GetItemIndex(SVRPropertyItem* pItem);

	SVExternalToolDetailsSheet*		m_pParentDialog;
	SVExternalToolTask*				m_pTask;
	SVExternalTool*					m_pTool;
	CString							m_sTitle;
	
	enum
	{
		ID_BASE = 100,
	};

	SVRPropTree	m_Tree;
	
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);

	// Generated message map functions
	//{{AFX_MSG(SVExternalToolResultPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

