//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   TADlgAcquisitionSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:16:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvIe
{
class SVCameraImageTemplate;
}
namespace SvOi
{
class IObjectClass;
}

class TADlgSheetClass;

class TADlgAcquisitionSourcePage : public CPropertyPage
{
public:
	TADlgAcquisitionSourcePage(uint32_t inspectionID, uint32_t taskObjectID, TADlgSheetClass* PSheet);

	virtual ~TADlgAcquisitionSourcePage();

	enum { IDD = IDD_TA_ACQUISITION_DIALOG };

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	afx_msg void OnSelchangeCameraCombo();

	DECLARE_MESSAGE_MAP()
private:

	SvOi::IObjectClass* m_pTaskObject{ nullptr };
	SvIe::SVCameraImageTemplate* m_pMainImage{ nullptr };
	SvMc::AvailableObjectListComboBox<uint32_t> m_CameraListBox;
};

