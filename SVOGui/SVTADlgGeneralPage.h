//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/AuxiliaryExtentsController.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVMFCControls\SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgGeneralPage : public CPropertyPage
	{
	public:
		SVTADlgGeneralPage(uint32_t inspectionId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum subType);
		virtual ~SVTADlgGeneralPage();

	protected:
		//{{AFX_MSG(SVTADlgGeneralPage)
		virtual BOOL OnInitDialog() override;

		afx_msg void OnSelchangeDrawToolCombo();
		afx_msg void OnUpdateAuxiliaryExtents();
	
		afx_msg void OnUpdateUseUnitMapping();

		afx_msg void OnUpdateCloseTool();
		afx_msg void OnSelchangeSourceImageCombo();
		afx_msg void OnSelchangeToolForOverlayColorCombo();
		afx_msg void OnShowRelations();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVTADlgGeneralPage)
	public:
		virtual BOOL OnSetActive() override;
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		void refresh();
		bool CheckAuxiliaryExtentsAvailable() const;
		HRESULT SetInspectionData();
		void SetupDrawFlagComboBox();
		void SetImages();
		void initToolForOverlayColor();

	public:
		enum { IDD = IDD_TA_GENERAL_DIALOG };

	protected:
		SvMc::AvailableObjectListComboBox<uint32_t> m_AvailableSourceImageCombo;
		long   m_SelectedInputImage {0};
		BOOL  m_UseUnitMapping{FALSE}; // use 1:1 mapping if auxextants are enabled for the inspection 
		bool m_IsExternalTool {false};
		SvMc::SVEnumerateCombo m_drawToolCombo;
		BOOL m_bUpdateAuxiliaryExtents = false; //calculate Auxextents for this tool 
		BOOL m_bCloseTool = false;
		SvMc::AvailableObjectListComboBox<uint32_t> m_AvailableToolForColorOverlayCombo;
		SvUl::NameObjectIdList m_availableToolList;

		bool m_bAuxExtentsAvailable = false; //Are Auxextents calculation enabled for this Inspection 
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		const SvPb::SVObjectSubTypeEnum m_subType;

		SvOgu::AuxiliaryExtentsController m_AuxExtentsController;
		SvOgu::ValueController m_values;
	};
} //namespace SvOg
