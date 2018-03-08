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
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "SVToolAdjustmentDialogImagePageClass.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogTransformImagePageClass : public SVToolAdjustmentDialogImagePageClass
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
		typedef SvOg::DataController<Values, Values::value_type> ValuesController;
		ValuesController m_Values;

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

		void GetData();
		void SetData();
		void CheckSourceImage();

	public:
		SVToolAdjustmentDialogTransformImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SvDef::SVObjectSubTypeEnum SubType = SvDef::SVImageMonoType, int id = IDD);
		virtual ~SVToolAdjustmentDialogTransformImagePageClass();
		HRESULT SetInspectionData();
	};
} //namespace SvOg
