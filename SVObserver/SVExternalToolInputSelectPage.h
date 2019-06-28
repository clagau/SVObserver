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
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

namespace SvOp
{
class SVExternalToolTask;
}
class SVObjectClass;

class SVExternalToolInputSelectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(SVExternalToolInputSelectPage)

// Construction
public:
	SVExternalToolInputSelectPage( LPCTSTR Title, const SVGUID& rInspectionID, const SVGUID& rToolObjectID, const SVGUID& rTaskObjectID, int id = IDD );
	virtual ~SVExternalToolInputSelectPage();

// Dialog Data
	//{{AFX_DATA(SVExternalToolInputSelectPage)
	enum { IDD = IDD_EXTERNAL_TOOL_INPUTS };
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
	bool ValidateValueObject(SVObjectClass* pObject, int iIndex);
	int GetItemIndex(SVRPropertyItem* pItem);

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
	
	DECLARE_MESSAGE_MAP()

private:
	std::string GetName(const SVGUID& guid) const;

private:
	SvOp::SVExternalToolTask* m_pTask;
	SVRPropTree	m_Tree;
	int m_inputValueCount;

	const SVGUID m_InspectionID;
	const SVGUID m_ToolObjectID;
	const SVGUID m_TaskObjectID;

	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;
};


