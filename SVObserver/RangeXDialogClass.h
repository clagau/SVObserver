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

#pragma region Includes
#include "Definitions/RangeEnum.h"
#include "SVOGui/RangeController.h"
#include "SVOGui/ObjectSelectorController.h"
#pragma endregion Includes

class RangeXDialogClass : public CDialog
{
	DECLARE_DYNAMIC(RangeXDialogClass)

public:
	RangeXDialogClass(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rRangeID, CWnd* parent = nullptr);
	virtual ~RangeXDialogClass();

	//************************************
	// Description:  Set the values from RangeHelperClass to the Dialog
	// Returns:   void
	//************************************
	void SetDlgData();

	// Dialogfelddaten
	enum { IDD = IDD_RANGE_X_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung

	HRESULT SetInspectionData();

	DECLARE_MESSAGE_MAP()
public:
	//************************************
	// Description:  Shows the Object Selector
	// Parameter:  name<in/out>:  previous and new name of object
	// Returns:   bool:  true if a new object was selected
	//************************************
	bool ShowObjectSelector(std::string& rName, RangeEnum::ERange fieldEnum);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedFailHighIndirect();
	afx_msg void OnBnClickedWarnlHighIndirect();
	afx_msg void OnBnClickedWarnLowIndirect();
	afx_msg void OnBnClickedFailedLowIndirect();

protected:
	CString m_FailHigh;
	CString m_FailLow;
	CString m_WarnHigh;
	CString m_WarnLow;
	CButton m_ButtonFailHigh;
	CButton m_ButtonWarnHigh;
	CButton m_ButtonWarnLow;
	CButton m_ButtonFailLow;
	CBitmap m_downArrowBitmap;

	SvOg::RangeController m_rangeController;
	SvOg::ObjectSelectorController m_objectSelector;
};

