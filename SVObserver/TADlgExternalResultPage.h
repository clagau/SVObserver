//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgExternalResultPage
//* .File Name       : $Workfile:   TADlgExternalResultPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOgu
{
class ExternalToolTaskController;
}

class TADlgExternalResultPage : public CPropertyPage
{
public:
	TADlgExternalResultPage(LPCTSTR Title, uint32_t inspectionId, uint32_t taskObjectId, SvOgu::ExternalToolTaskController& rExternalToolTaskController);
	virtual ~TADlgExternalResultPage();

	//{{AFX_DATA(TADlgExternalResultPage)
	enum { IDD = IDD_TA_EXTERNAL_RESULTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgExternalResultPage)
	public:
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnSetActive() override;
	//}}AFX_VIRTUAL

public:
	void DisplayResults();

protected:
	int SelectObject(int iIndex);
	int GetItemIndex(SVRPropertyItem* pItem);
	void rebuildPropertyTree();
	std::pair<std::map<std::string, SVRPropertyItem*>, int> AssembleTree(SVRPropertyItem* pRoot);
	void HandleTables(std::map<std::string, SVRPropertyItem*>& rMapGroupItems, int NumResults, SVRPropertyItem* pRoot);

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	std::string	m_sTitle;
	SvOgu::ExternalToolTaskController& m_rExternalToolTaskController;
	SvOgu::ValueController m_ValueController;

	enum
	{
		ID_BASE = 100,
	};

	SVRPropTree	m_Tree;
	
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);

	// Generated message map functions
	//{{AFX_MSG(TADlgExternalResultPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

