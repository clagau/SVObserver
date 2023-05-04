//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTransformImagePage
//* .File Name       : $Workfile:   TADlgTransformImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOGuiUtility/DataController.h"
#include "TADlgImagePageClass.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgTransformImagePage : public TADlgImagePageClass
	{
		//{{AFX_VIRTUAL(TADlgTransformImagePage)
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		//}}AFX_VIRTUAL

		//{{AFX_MSG(TADlgTransformImagePage)
		afx_msg void OnSelchangeCombo1();
		afx_msg void OnUseExtentsOnly();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_DATA(TADlgTransformImagePage)
		enum { IDD = IDD_TA_TRANSFORM_IMAGE_DIALOG };
		CButton	m_useExtentsOnlyCheckBox;
		BOOL m_useExtentsOnly;
		//}}AFX_DATA

		void CheckSourceImage();

	public:
		TADlgTransformImagePage(uint32_t inspectionId, uint32_t toolID, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVImageMonoType, int id = IDD);
		virtual ~TADlgTransformImagePage();
		HRESULT SetInspectionData();

	private:
		SvOgu::ValueController m_values;
	};
} //namespace SvOg
