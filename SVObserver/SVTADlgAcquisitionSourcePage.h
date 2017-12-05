//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   SVTADlgAcquisitionSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:16:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/AvailableObjectListComboBox.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVCameraImageTemplate;

class SVToolAdjustmentDialogAcquisitionSourcePageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogAcquisitionSourcePageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* PSheet );

	virtual ~SVToolAdjustmentDialogAcquisitionSourcePageClass();

	enum { IDD = IDD_TA_ACQUISITION_DIALOG };

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	afx_msg void OnSelchangeCameraCombo();

	DECLARE_MESSAGE_MAP()
private:

	SVToolAdjustmentDialogSheetClass* m_pSheet{ nullptr };
	SVToolClass* m_pTool{ nullptr };
	SVCameraImageTemplate* m_pMainImage{ nullptr };
	SvMc::AvailableObjectListComboBox m_CameraListBox;
};

