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

#pragma once

#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;

class SVExternalToolResultPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVExternalToolResultPage)

public:
	SVExternalToolResultPage(LPCTSTR Title = nullptr, SVExternalToolDetailsSheet* pParent = nullptr, int id = IDD );
	virtual ~SVExternalToolResultPage();

	//{{AFX_DATA(SVExternalToolResultPage)
	enum { IDD = IDD_EXTERNAL_TOOL_RESULTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolResultPage)
	public:
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	int SelectObject(int iIndex);
	int GetItemIndex(SVRPropertyItem* pItem);

	SVExternalToolDetailsSheet*		m_pParentDialog;
	SVExternalToolTask*				m_pTask;
	SVExternalTool*					m_pTool;
	SVString						m_sTitle;
	
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
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

