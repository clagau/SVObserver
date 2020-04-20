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
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "AuxiliaryExtentsController.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVMFCControls\SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogGeneralPageClass : public CPropertyPage
	{
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> ValuesController;
	public:
		SVToolAdjustmentDialogGeneralPageClass(uint32_t inspectionId, uint32_t taskObjectId);
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
		enum { IDD = IDD_TA_GENERAL_DIALOG };

	protected:
		SvMc::AvailableObjectListComboBox<uint32_t> m_AvailableSourceImageCombo;
		SvMc::SVEnumerateComboClass m_drawToolCombo;
		BOOL m_bUpdateAuxiliaryExtents;

		bool m_bAuxExtentsAvailable;
		bool m_bIsImageTool;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;

		SvOg::AuxiliaryExtentsController m_AuxExtentsController;
		ValuesController m_Values;
	};
} //namespace SvOg
