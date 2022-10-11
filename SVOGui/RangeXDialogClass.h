//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "RangeController.h"
#include "LinkedValueWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
class RangeXDialogClass : public CDialog
{
public:
	RangeXDialogClass(uint32_t inspectionID, uint32_t toolId, uint32_t taskID, uint32_t rangeID, CWnd* parent = nullptr);
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
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog() override;
	void OnButtonRange(UINT nID);
	void OnKillFocusRange(UINT nID);

protected:
	std::array<std::unique_ptr<LinkedValueWidgetHelper>, RangeEnum::ER_COUNT> m_RangeWidgets;
	std::array<CButton, RangeEnum::ER_COUNT> m_RangeButtons;
	std::array<CEdit, RangeEnum::ER_COUNT> m_RangeEdits;

	RangeController m_rangeController;
	uint32_t m_toolId;
};
} //namespace SvOg

