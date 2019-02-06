//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRemoteInputToolPage
//* .File Name       : $Workfile:   SVTADlgRemoteInputToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:17:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVValueObjectLibrary/SVValueObject.h"
#pragma endregion Includes

namespace SvTo
{
class SVRemoteInputTool;
}
class SVToolAdjustmentDialogSheetClass;

class SVTADlgRemoteInputToolPage : public CPropertyPage
{
	// Generated message map functions
	public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedClearInputButton();
	afx_msg void OnBnClickedSelectInputButton();
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;

public:
	enum { IDD = IDD_TA_REMOTE_INPUT_PAGE };

	SVTADlgRemoteInputToolPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD );
	virtual ~SVTADlgRemoteInputToolPage();

protected:
	void RefreshSelectedInputName();

	CStatic m_RemoteInputNameCtrl;
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SvTo::SVRemoteInputTool* m_pTool;
	std::string m_InputName;

};

