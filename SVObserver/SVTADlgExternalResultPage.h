//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalResultPage
//* .File Name       : $Workfile:   SVTADlgExternalResultPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/ExternalToolTaskController.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes


class SVTADlgExternalResultPage : public CPropertyPage
{
public:
	SVTADlgExternalResultPage(LPCTSTR Title, uint32_t inspectionId, uint32_t taskObjectId, int id = IDD );
	virtual ~SVTADlgExternalResultPage();

	//{{AFX_DATA(SVTADlgExternalResultPage)
	enum { IDD = IDD_TA_EXTERNAL_RESULTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgExternalResultPage)
	public:
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	int SelectObject(int iIndex);
	int GetItemIndex(SVRPropertyItem* pItem);

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	std::string						m_sTitle;
	ExternalToolTaskController m_externalToolTaskController;
	SvOg::ValueController m_ValueController;

	enum
	{
		ID_BASE = 100,
	};

	SVRPropTree	m_Tree;
	
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);

	// Generated message map functions
	//{{AFX_MSG(SVTADlgExternalResultPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

