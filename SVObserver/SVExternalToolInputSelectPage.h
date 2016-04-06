//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolInputSelectPage
//* .File Name       : $Workfile:   SVExternalToolInputSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "PropertyTree/PropTree.h"
#pragma endregion Includes

class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;
class SVObjectClass;

class SVExternalToolInputSelectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(SVExternalToolInputSelectPage)

// Construction
public:
	SVExternalToolInputSelectPage(const CString& sTitle, SVExternalToolDetailsSheet* pParent = NULL, int id = IDD );
	virtual ~SVExternalToolInputSelectPage();

	CString							m_sGroupName;
	bool							m_bTabbed;

// Dialog Data
	//{{AFX_DATA(SVExternalToolInputSelectPage)
	enum { IDD = IDD_EXTERNAL_TOOL_INPUTS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolInputSelectPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int SelectObject(CString& rsSelectedName, SVRPropertyItem* pItem);
	SVObjectClass* FindObject(SVRPropertyItem* pItem);
	HRESULT ValidateItem(SVRPropertyItem* pItem);
	int GetItemIndex(SVRPropertyItem* pItem);

	SVExternalToolDetailsSheet*		m_pParentDialog;
	SVExternalToolTask*				m_pTask;
	SVExternalTool*					m_pTool;

	enum
	{
		ID_BASE = 100,
	};

	SVRPropTree	m_Tree;

	// Generated message map functions
	//{{AFX_MSG(SVExternalToolInputSelectPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	
	DECLARE_MESSAGE_MAP()
};


