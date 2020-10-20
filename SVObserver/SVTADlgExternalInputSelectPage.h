//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalInputSelectPage
//* .File Name       : $Workfile:   SVTADlgExternalInputSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/DataController.h"
#include "SVOGui/ExternalToolTaskController.h"
#pragma endregion Includes


class SVTADlgExternalInputSelectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(SVTADlgExternalInputSelectPage)

// Construction
public:
	SVTADlgExternalInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, int id = IDD);
	virtual ~SVTADlgExternalInputSelectPage() = default;

	// Dialog Data
	//{{AFX_DATA(SVTADlgExternalInputSelectPage)
	enum { IDD = IDD_TA_EXTERNAL_INPUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgExternalInputSelectPage)
	public:
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int SelectObject( std::string& rSelectedName, SVRPropertyItem* pItem );
	HRESULT ValidateItem(SVRPropertyItem* pItem);

	std::string getCompleteName(SVRPropertyItem* pItem) const;

	int GetItemIndex(SVRPropertyItem* pItem);
	const std::unique_ptr<SvPb::InputValueDefinition> GetInputDefinitionPtr(SVRPropertyItem* pItem);

	enum
	{
		ID_BASE = 100,
	};

	// Generated message map functions
	//{{AFX_MSG(SVTADlgExternalInputSelectPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnUndoChanges();
	
	DECLARE_MESSAGE_MAP()

private:
	std::string GetName(uint32_t id) const;

	SVRPropertyItem* AddGroupToTree(const std::string& groupName, std::map<std::string, SVRPropertyItem*>& rMapGroupItems, SVRPropertyItem* pRoot);
	void AddItemToTree(const SvPb::InputValueDefinition& rDefinition, SVRPropertyItem* pGroupItem, int iID);
private:
	SVRPropTree	m_Tree;
	int m_inputValueCount;

	const uint32_t m_InspectionID;
	const uint32_t m_ToolObjectID;

	ExternalToolTaskController m_externalToolTaskController;
	const uint32_t m_TaskObjectID;
	SvOg::ValueController m_InputValues;
};

