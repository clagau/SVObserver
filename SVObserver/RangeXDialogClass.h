//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : SVRangeXDialog D
//* .File Name       : $Workfile:   RangeXDialogClass.h  $
//* .Description     : RangeXDialogClass this dialog is used instead of RangeDialogClass when indirect values for the rangeobjects are allowed
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 13:53:40  $
//******************************************************************************
#pragma once
#include "afxwin.h"
#include "SVContainerLibrary/SVVector.h"
#include "ISVCancel.h"
#include "SVOCore/SVTaskObjectList.h"
#include "RangeClassHelper.h"

class SVRangeClass;
class RangeClassHelper;
class SVIPDoc;


class RangeXDialogClass : public CDialog
{
	DECLARE_DYNAMIC(RangeXDialogClass)

public:
	RangeXDialogClass(SVRangeClass* range, CWnd* parent = nullptr);
	virtual ~RangeXDialogClass();

	//************************************
	// Description:  Set the values from RangeHelperClass to the Dialog
	// Returns:   void
	//************************************
	void SetDlgData();

	//************************************
	// Description:  Get the values from the Dialog and set them to the RangeHelper
	// Returns:  bool:  
	//************************************
	bool GetDlgData();

	// Dialogfelddaten
	enum { IDD = IDD_RANGE_X_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	//************************************
	// Description:  Shows the Object Selector
	// Parameter:  name<in/out>:  previous and new name of object
	// Returns:   bool:  true if a new object was selected
	//************************************
	bool ShowObjectSelector(CString& name);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFailHighIndirect();
	afx_msg void OnBnClickedWarnlHighIndirect();
	afx_msg void OnBnClickedWarnLowIndirect();
	afx_msg void OnBnClickedFailedLowIndirect();

protected:
	RangeEnum::ERange m_LastSelected;

	CEdit m_EditFailHigh;
	CEdit m_EditWarnHigh;
	CEdit m_EditWarnLow;
	CEdit m_EditFailLow;
	CButton m_ButtonFailHigh;
	CButton m_ButtonWarnHigh;
	CButton m_ButtonWarnLow;
	CButton m_ButtonFailLow;
	CBitmap m_downArrowBitmap;

	RangeClassHelper m_RangeClassHelper;
};

