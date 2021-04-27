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
	SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	SVToolAdjustmentDialogSheetClass(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVToolAdjustmentDialogSheetClass();

	SVIPDoc* GetIPDoc() const;
	void markDocumentAsDirty(bool runOnce=false); // Marks the document as 'dirty' so user will be prompted to save this configuration on program exit.

	SvOi::IObjectClass* GetTaskObject() const;
	uint32_t GetInspectionID() const {return m_InspectionID; }
	uint32_t GetTaskObjectID() const {return m_TaskObjectID; }

	void AddAdditionalPagesForExternalTool(bool addToolAdjustmentDialogCommentPage);

protected:
	void init();
	void addPages();

private:
	CPropertyPage* createToolAdjustmentDialogCommentPage();
	void addConditionalDialog();

	SVIPDoc* m_pIPDoc;
	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;

	typedef std::shared_ptr<SvOg::FormulaController> ControllerPtr;
	ControllerPtr m_conditionalController;
	ExternalToolTaskController m_externalToolTaskController;

	LRESULT AddPagesForTestedExternalTool(WPARAM, LPARAM);
	LRESULT RemovePagesForTestedExternalTool(WPARAM, LPARAM);
};

/// this is the postion at which the "Input Values" page will be found - if it is present at all, that is!
constexpr int c_indexOfInputValuePage = 2;



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

