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
#pragma endregion Includes

namespace SvOp
{
class SVExternalToolTask;
class InputValueDefinition;
}
class SVObjectClass;

class SVExternalToolInputSelectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(SVExternalToolInputSelectPage)

// Construction
public:
	SVExternalToolInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, uint32_t taskObjectID, int id = IDD);
	virtual ~SVExternalToolInputSelectPage();

// Dialog Data
	//{{AFX_DATA(SVExternalToolInputSelectPage)
	enum { IDD = IDD_TA_EXTERNAL_INPUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolInputSelectPage)
	public:
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int SelectObject( std::string& rSelectedName, SVRPropertyItem* pItem );
	SVObjectClass* FindObject(SVRPropertyItem* pItem);
	HRESULT ValidateItem(SVRPropertyItem* pItem);

	bool ValidateValueObject(SVObjectClass* pObject, const SvOp::InputValueDefinition& rInputedef);
	int GetItemIndex(SVRPropertyItem* pItem);
	const SvOp::InputValueDefinition*  GetInputValueDefinitionPtr(SVRPropertyItem* pItem);

	enum
	{
		ID_BASE = 100,
	};

	// Generated message map functions
	//{{AFX_MSG(SVExternalToolInputSelectPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnUndoChanges();

	
	DECLARE_MESSAGE_MAP()

private:
	std::string GetName(uint32_t id) const;

private:
	SvOp::SVExternalToolTask* m_pTask;
	SVRPropTree	m_Tree;
	int m_inputValueCount;

	const uint32_t m_InspectionID;
	const uint32_t m_ToolObjectID;
	const uint32_t m_TaskObjectID;

	SvOg::ValueController m_Values;
};


