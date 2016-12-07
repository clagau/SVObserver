//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDetailsSheet
//* .File Name       : $Workfile:   SVExternalToolDetailsSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:48  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVPropertySheetCancelImpl.h"
#include "SVExternalTool.h"
#pragma endregion Includes

class SVExternalToolTask;
class SVExternalToolDlg;
class SVIPDoc;

class SVExternalToolDetailsSheet : public CPropertySheet, public ISVCancel//, public SVPropertySheetCancelImpl
{
	DECLARE_DYNAMIC(SVExternalToolDetailsSheet)

// Construction
public:
	SVExternalToolDetailsSheet(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, long numImages, LPCTSTR pszCaption, SVExternalToolDlg* pParentWnd = nullptr, UINT iSelectPage = 0 );
	HRESULT CreatePages();

	SVIPDoc* GetIPDoc() const;

	// ISVCancel
	virtual bool CanCancel() override;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

// Attributes
public:
	SVExternalTool* m_pTool;
	SVExternalToolTask* m_pTask;
	SVExternalToolDlg* m_pSVExternalToolDlgParent;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolDetailsSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL
	void OnOK();

// Implementation
public:
	virtual ~SVExternalToolDetailsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVExternalToolDetailsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DestroyPages();

	SVGUID m_InspectionID;
	SVGUID m_TaskObjectID;
	long m_numImages;
};
