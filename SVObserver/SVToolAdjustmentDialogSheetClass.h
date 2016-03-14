//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogSheetClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogSheetClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SvOGui/FormulaController.h"
#pragma endregion Includes

class SVIPDoc;
class SVToolClass;

class SVToolAdjustmentDialogSheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SVToolAdjustmentDialogSheetClass)

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogSheetClass)
public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVToolAdjustmentDialogSheetClass)
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVToolAdjustmentDialogSheetClass( SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVToolAdjustmentDialogSheetClass( SVIPDoc* p_pIPDoc, const GUID& rInspectionID, const GUID& rTaskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~SVToolAdjustmentDialogSheetClass();

	SVIPDoc* GetIPDoc() const;
	SVToolClass* GetTool() const;
	const GUID& GetInspectionID() const;
	const GUID& GetToolID() const;
	
	template< typename SVToolType >
	HRESULT GetToolByType( SVToolType*& rpTool ) const
	{
		HRESULT l_Status = S_OK;

		rpTool = dynamic_cast< SVToolType* >( GetTool() );

		if( nullptr == rpTool )
		{
			l_Status = E_FAIL;
		}
		return l_Status;
	}
	
protected:
	void init();
	void addPages();

private:
	SVIPDoc* m_pIPDoc;
	GUID m_InspectionID;
	GUID m_TaskObjectID;

	typedef SVSharedPtr<SvOg::FormulaController> ControllerPtr;
	ControllerPtr m_conditionalController;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

