//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetAdjustmentDialogSheet
//* .File Name       : $Workfile:   SVToolSetAdjustmentDialogSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SvOGui/SVFormulaEditorPage.h"
#include "SVOCore/SVConditional.h"
#pragma endregion Includes

class SVToolSetAdjustmentDialogSheetClass : public CPropertySheet
{
//******************************************************************************
// MFC Declarations:
//******************************************************************************
	DECLARE_DYNAMIC(SVToolSetAdjustmentDialogSheetClass)

//******************************************************************************
// Class Wizard Generated Message Map Entries:
//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolSetAdjustmentDialogSheetClass)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	virtual ~SVToolSetAdjustmentDialogSheetClass();

protected:
	void init( SVConditionalClass& rCondition );
	void addPages();

    afx_msg void OnOK();
    afx_msg void OnCancel();

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	//{{AFX_VIRTUAL(SVToolSetAdjustmentDialogSheetClass)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

//******************************************************************************
// Class Wizard Generated Dialog Data:
//******************************************************************************
	//{{AFX_DATA(<ClassName>)
	//}}AFX_DATA

private:
	GUID m_InspectionID;
	GUID m_TaskObjectID;

	typedef SVSharedPtr<SvOg::SVFormulaEditorPageClass> FormulaEditorPagePtr;
	FormulaEditorPagePtr m_formulaPage;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

