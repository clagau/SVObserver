//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorToolPage
//* .File Name       : $Workfile:   SVTADlgColorToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgColorToolPage : public CPropertyPage
	{
	public:
		SVTADlgColorToolPage( uint32_t inspectionId, uint32_t taskObjectId );
		virtual ~SVTADlgColorToolPage();

		HRESULT SetInspectionData();

	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVTADlgColorToolPageClass)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(SVTADlgColorToolPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeCombo1();
		afx_msg void OnConvertToHsi();
		afx_msg void OnTrainColor();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void SetImage();

	protected:
		enum { IDD = IDD_TA_COLOR_TOOL_DIALOG };
		SvOgu::PictureDisplay	m_dialogImage;
		SvMc::AvailableObjectListComboBox<uint32_t> m_availableSourceImageListBox;

		BOOL	m_convertToHSI;
		std::string m_InputName;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		uint32_t m_ResultImageID = SvDef::InvalidObjectId;
		
		SvOgu::ImageController m_Images;
		SvOgu::ValueController m_values;
	};
} //namespace SvOg

