//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SheetForExternalToolAdjustment.cpp
/// The property sheet for the tool adjustment dialog for the external tool
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SvOGui/FormulaController.h"
#include "SVOGui/ExternalToolTaskController.h"
#pragma endregion Includes

namespace SvOi
{
	class IObjectClass;
}


namespace SvOg
{
	class SVExternalToolImageSelectPage;
}

namespace SvOp
{
	struct InputImageInformationStruct;
}


class SVIPDoc;

class SVToolAdjustmentDialogSheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SVToolAdjustmentDialogSheetClass)
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogSheetClass)
public:
	virtual BOOL OnInitDialog() override;
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
	SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVToolAdjustmentDialogSheetClass();

	SVIPDoc* GetIPDoc() const;
	void markDocumentAsDirty(bool runOnce = false); // Marks the document as 'dirty' so user will be prompted to save this configuration on program exit.

	SvOi::IObjectClass* GetTaskObject() const;
	uint32_t GetInspectionID() const { return m_InspectionID; }
	uint32_t GetTaskObjectID() const { return m_TaskObjectID; }

	void AddAdditionalPagesForExternalTool();
	void init();

protected:
	virtual void addPages();
	void ValidateAllSheets();
	bool ResetTools(SvOi::IObjectClass* pObject);

	CPropertyPage* createToolAdjustmentDialogCommentPage();

private:
	void addConditionalDialog();

	SVIPDoc* m_pIPDoc;

	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;

	typedef std::shared_ptr<SvOg::FormulaController> ControllerPtr;
	ControllerPtr m_conditionalController;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

