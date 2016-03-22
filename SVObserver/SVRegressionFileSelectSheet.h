// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectSheet
// * .File Name       : $Workfile:   SVRegressionFileSelectSheet.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:56  $
// ******************************************************************************

#pragma once

#include "SVRegressionTestStruct.h"
#include "SVVirtualCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet

class CSVRegressionFileSelectSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSVRegressionFileSelectSheet)

// Construction
public:
	CSVRegressionFileSelectSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSVRegressionFileSelectSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	BOOL ValidateMask(CString sMask);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionFileSelectSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();

// Implementation
public:
	virtual ~CSVRegressionFileSelectSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	void CreatePages(CList<RegressionTestStruct*,RegressionTestStruct*>*RegessionList,SVVirtualCameraPtrSet CameraList );


private: //methods
	CString MakeFileNameMask(CString csFileName);
	RegressionFileSelectCode ValidateList();
	long GetNumberOfFilesMatchingMask(CString sMask);
	void ClearRegressionList();
	void FillFileList();

private: //data members
	CList<CString, CString&> m_listMasks;
	CList<RegressionTestStruct*,RegressionTestStruct*> *m_pRegressionList;

};

