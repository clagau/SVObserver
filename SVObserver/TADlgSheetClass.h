//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgSheetClass.cpp
/// The property sheet for the tool adjustment dialog for the external tool
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/ResizablePropertySheet.h"
#include "SVOGuiUtility/FormulaController.h"
#include "SVOGuiUtility/ExternalToolTaskController.h"
#pragma endregion Includes

namespace SvOi
{
	class IObjectClass;
}

namespace SvOg
{
	class TADlgExternalToolImageSelectPage;
}

namespace SvOp
{
	struct InputImageInformationStruct;
}

class SVIPDoc;

class TADlgSheetClass : public SvMc::ResizablePropertySheet
{
public:
	virtual BOOL OnInitDialog() override;

protected:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	TADlgSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~TADlgSheetClass();

	SVIPDoc* GetIPDoc() const;
	void markDocumentAsDirty(bool runOnce = false); // Marks the document as 'dirty' so user will be prompted to save this configuration on program exit.

	SvOi::IObjectClass* GetTaskObject() const;
	SvOi::IObjectClass* GetTopTool() const;
	uint32_t GetInspectionID() const { return m_InspectionID; }
	uint32_t GetTaskObjectID() const { return m_TaskObjectID; }

	void AddAdditionalPagesForExternalTool();
	void init();

protected:
	virtual void addPages();
	bool ValidateAllSheets();
	bool ResetTools();

	CPropertyPage* createToolAdjustmentDialogCommentPage();

private:
	void addConditionalDialog();

	SVIPDoc* m_pIPDoc;

	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;

	std::vector<uint32_t> m_dependentTools;

	typedef std::shared_ptr<SvOgu::FormulaController> ControllerPtr;
	ControllerPtr m_conditionalController;

	CScrollBar m_resizeGripper;
};

