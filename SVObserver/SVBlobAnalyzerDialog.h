//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Nov 2014 05:09:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVBlobAnalyzer.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVToolClass;

class SVBlobAnalyzeFeatureDialogClass : public CDialog, public SVTaskObjectInterfaceClass
{
public:
protected:
// Generated message map functions
	//{{AFX_MSG(SVBlobAnalyzeFeatureDialogClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonSetRange();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSetSortBtn();
	afx_msg void OnAscending();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeList2();
	afx_msg void OnNbrOfBlobs();
	afx_msg void OnExcludeBlob();
	afx_msg void OnBtnFillBlobs();
	afx_msg void OnFillBlobs();
	afx_msg void OnBlobColor();
	afx_msg void OnChangeEditMaxNbrBlobs();
	afx_msg void OnKillFocusMaxNbrBlobs();
	afx_msg void OnButtonSetFeatureProperties();
	afx_msg void OnChangeEditMaxBlobDataArraySize(); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
// Standard constructor
	SVBlobAnalyzeFeatureDialogClass( SVToolClass* PTool, SVBlobAnalyzerClass* Analyzer, SVIPDoc* p_pIPDoc, CWnd* pParent = nullptr );

// Standard destructor
	virtual ~SVBlobAnalyzeFeatureDialogClass();

	virtual HRESULT SetInspectionData();

	void EnableButtons();

private:
	void initFeatureListBox(CListBox& listBox, const SvOi::NameValueList& list);

	//{{AFX_VIRTUAL(SVBlobAnalyzeFeatureDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVBlobAnalyzeFeatureDialogClass)
	enum { IDD = IDD_BA_FEATURE_DIALOG };
	CEdit	m_ebMaxBlobDataArraySize;
	CEdit	m_ctlMaxNbrBlobs;
	CButton	m_chkFillBlob;
	CButton	m_btnFillBlobs;
	CComboBox m_cbBlobColor;
	CListBox	m_lbSelectedFeatures;
	CListBox	m_lbAvailableFeatures;
	CString	msvSortFeatureEdt;
	BOOL	msvAscending;
	BOOL	m_bExclude;
	long	m_lMaxNumberBlobs;
	long	m_lMaxBlobDataArraySize;
	//}}AFX_DATA


/*- msvszOriginalFeaturesEnabled ---------------------------------------------------*/
/*- This holds the original list of enabled features so that the list can be        */
/*- restored if the operator presses cancel. ---------------------------------------*/
	TCHAR                           msvszOriginalFeaturesEnabled 
	                                    [SV_NUMBER_OF_BLOB_FEATURES + 1]; // Null Terminated
	SVToolClass*                    msvpTool;

	CString script;
	CString alias;
	long    m_colorBlobEnum;

protected:
	SVIPDoc* m_pIPDoc;
	SVBlobAnalyzerClass* m_pCurrentAnalyzer;
};

