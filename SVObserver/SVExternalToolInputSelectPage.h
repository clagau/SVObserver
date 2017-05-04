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
#include "SVUtilityLibrary/SVString.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/GuiController.h"
#pragma endregion Includes

class SVExternalToolTask;
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
	int SelectObject( SVString& rSelectedName, SVRPropertyItem* pItem );
	SVObjectClass* FindObject(SVRPropertyItem* pItem);
	HRESULT ValidateItem(SVRPropertyItem* pItem);
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
	SVString GetName(const SVGUID& guid) const;
	GUID GetToolSetGUID() const;

private:
	SVExternalToolTask*				m_pTask;
	SVRPropTree	m_Tree;
	int m_inputValueCount;

	SVGUID m_InspectionID;
	SVGUID m_ToolObjectID;
	SVGUID m_TaskObjectID;

	typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
	SvOg::GuiController<ValueCommand, ValueCommand::value_type> m_Values;
};


