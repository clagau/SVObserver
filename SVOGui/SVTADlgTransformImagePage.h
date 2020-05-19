//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTransformImagePage
//* .File Name       : $Workfile:   SVTADlgTransformImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "DataController.h"
#include "SVToolAdjustmentDialogImagePageClass.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogTransformImagePageClass : public SVToolAdjustmentDialogImagePageClass
	{
		//{{AFX_VIRTUAL(SVToolAdjustmentDialogTransformImagePageClass)
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		//}}AFX_VIRTUAL

		//{{AFX_MSG(SVToolAdjustmentDialogTransformImagePageClass)
		afx_msg void OnSelchangeCombo1();
		afx_msg void OnUseExtentsOnly();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_DATA(SVToolAdjustmentDialogTransformImagePageClass)
		enum { IDD = IDD_TA_TRANSFORM_IMAGE_DIALOG };
		CButton	m_useExtentsOnlyCheckBox;
		BOOL m_useExtentsOnly;
		//}}AFX_DATA

		void CheckSourceImage();

	public:
		SVToolAdjustmentDialogTransformImagePageClass(uint32_t inspectionId, uint32_t toolID, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVImageMonoType, int id = IDD);
		virtual ~SVToolAdjustmentDialogTransformImagePageClass();
		HRESULT SetInspectionData();

	private:
		ValueController m_values;
	};
} //namespace SvOg
