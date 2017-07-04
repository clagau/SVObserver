//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:15:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "AuxiliaryExtentsController.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogGeneralPageClass : public CPropertyPage
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
		typedef SvOg::DataController<Values, Values::value_type> ValuesController;
	public:
		SVToolAdjustmentDialogGeneralPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
		virtual ~SVToolAdjustmentDialogGeneralPageClass();

	protected:
		//{{AFX_MSG(SVToolAdjustmentDialogGeneralPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeDrawToolCombo();
		afx_msg void OnUpdateAuxiliaryExtents();
		afx_msg void OnSelchangeSourceImageCombo();
		afx_msg void OnShowRelations();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVToolAdjustmentDialogGeneralPageClass)
	public:
		virtual BOOL OnSetActive() override;
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		void refresh();
		bool CheckAuxiliaryExtentsAvailable() const;
		HRESULT SetInspectionData();
		void SetupAuxExtents();
		void SetupDrawFlagComboBox();
		void SetImages();

	public:
		//{{AFX_DATA(SVToolAdjustmentDialogGeneralPageClass)
		enum { IDD = IDD_TA_GENERAL_DIALOG };
		SvMc::AvailableObjectListComboBox m_AvailableSourceImageCombo;
		CComboBox m_drawToolCombo;
		BOOL m_bUpdateAuxiliaryExtents;
		//}}AFX_DATA

	protected:
		bool m_bAuxExtentsAvailable;
		bool m_bIsImageTool;
		SVGUID m_InspectionID;
		SVGUID m_TaskObjectID;

		SvOg::AuxiliaryExtentsController m_AuxExtentsController;
		ValuesController m_Values;
	};
} //namespace SvOg
